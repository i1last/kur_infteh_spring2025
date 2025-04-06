#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "curses.h"
#include "service_functions.h"
#include "core.h"

int _CR_sort_col;
bool _CR_ascending;

int compare_rows(const void* a, const void* b) {
    const TableRow* row1 = (const TableRow*)a;
    const TableRow* row2 = (const TableRow*)b;

    int result = strcmp(
        row1->text[_CR_sort_col], 
        row2->text[_CR_sort_col]
    );
    
    return _CR_ascending ? result : -result;
}

void sort_table(TableInfo* data, int sort_col, bool ascending) {
    if (data->rows == NULL || data->row_count <= 1) return;
    
    _CR_sort_col = sort_col;
    _CR_ascending = ascending;

    qsort(data->rows, data->row_count, sizeof(TableRow), compare_rows);
}

void wide_to_char(wchar_t** _sorce_wide_text, char** _dest_char_text) {
    size_t required_size = WideCharToMultiByte(CP_UTF8, 0, *_sorce_wide_text, -1, NULL, 0, NULL, NULL);
    *_dest_char_text = (char*)malloc(required_size * sizeof(char));
    WideCharToMultiByte(CP_UTF8, 0, *_sorce_wide_text, -1, *_dest_char_text, required_size, NULL, NULL);
    return;
}

void char_to_wide(char** _source_char_text, wchar_t** _dest_wide_text) {
    size_t required_size = MultiByteToWideChar(CP_UTF8, 0, *_source_char_text, -1, NULL, 0);
    *_dest_wide_text = (wchar_t*)malloc(required_size * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, *_source_char_text, -1, *_dest_wide_text, required_size);
    return;
}

/*
mvwaddnstr использует для счетчика символов в строке strlen, который некорректно считает количество
символов в строке с кириллицей (не умеет работать с utf8). Именно поэтому используется переменная
condition, которая реализует следующее уравнение:
strlen_utf8(text) = condition
strlen(text) = x,
где strlen_utf8(text) корректно считает количество символов в строке (собственная функция).
Решаем уравнения накрест и получаем результат. 
*/
int convert_strlen_to_strlen_utf8(int condition, char** text) {
    int text_len = strlen_utf8(*text);
    if (text_len == 0) return 0;
    else return condition * strlen(*text) / text_len;
}

WINDOW* create_box_input_window(WINDOW** win) {
    WINDOW* box_win = subwin(*win, 3, MAX_BUFFER_LEN + 2 + 1, getmaxy(*win) / 2, getmaxx(*win) / 2 - MAX_BUFFER_LEN / 2);
    //                       для рамок слева и справа -^   ^- для отображения курсора
    box(box_win, 0, 0);

    return box_win;
}

void create_input_menu(WINDOW** box_win) {
    WINDOW* input_win = derwin(*box_win, getmaxy(*box_win) - 2, getmaxx(*box_win) - 2, 1, 1);
    
    // Заполняем текст из буфера
    for (int i = 0; i < CURRENT_BUFFER_LEN; i++) {
        if (i == CURSOR_POS) wattron(input_win, A_REVERSE);
        waddch(input_win, BUFFER[i]);
        wattroff(input_win, A_REVERSE);
    }

    // Если курсор стоит в конце, выделяем пробел
    if (CURSOR_POS == CURRENT_BUFFER_LEN) {
        wattron(input_win, A_REVERSE);
        waddch(input_win, ' ');
        wattroff(input_win, A_REVERSE);
    }

    // Заполняем оставшееся пространство пробелами
    for (int i = CURRENT_BUFFER_LEN; i <= MAX_BUFFER_LEN; i++) {
        waddch(input_win, ' ');
    }

    wrefresh(input_win);
    delwin(input_win);
}

unsigned smooth_selected_option(int selected_option, int condition) {
    selected_option %= condition;
    if (selected_option < 0) selected_option += condition;
    return selected_option;
}

int get_start_x_cord_of_cell(int cell_index, int cols_width[MAX_COLS_IN_TABLE], int string_len, int align) {
    int start_x_cord = 2;
    for (int i = 0; i < cell_index; i++) {
        start_x_cord += cols_width[i] + 3;
    }

    if (align == 1) // center
        start_x_cord += MAX((cols_width[cell_index] - string_len) / 2, 0);
    
    return start_x_cord;
}

unsigned strlen_utf8(const char* str) {
    unsigned len = 0;
    if (str != NULL) while (*str) {
        // Проверяем, является ли текущий байт началом символа UTF-8
        if ((*str & 0xC0) != 0x80) len++;
        str++;
    }
    return len;
}

bool file_exists(void) {
    wchar_t filename[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN] = { 0 };
    wcscpy(filename, BUFFER);
    wcscat(filename, FILE_EXTENSION);

    WIN32_FIND_DATA find_data;
    HANDLE hFind = FindFirstFile("*", &find_data);

    do {
        wchar_t c_filename[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN] = { 0 };
        char* windows_c_filename = find_data.cFileName;
        for (int i = 0; windows_c_filename[i] != '\0'; i++) {
            c_filename[i] = (wchar_t)windows_c_filename[i];
        }
        
        if (!wcscmp(c_filename, filename)) {
            FindClose(hFind);
            return true;
        }
        
    } while (FindNextFile(hFind, &find_data));

    FindClose(hFind);
    
    return false;
}

TableInfo read_csv(FILE* file) {
    char* delim = ";,";

    int capacity = 20,
        row_count = 0;
    char buffer[1024];

    TableRow* rows = calloc(capacity, sizeof(TableRow));

    while ((fgets(buffer, sizeof(buffer), file)) != NULL) {
        if (row_count >= capacity) {
            capacity += 20;
            rows = realloc(rows, capacity * sizeof(TableRow));
        }

        char* token = strtok(buffer, delim);
        int cell_count = 0;
        while (cell_count != MAX_COLS_IN_TABLE) {
            rows[row_count].text[cell_count] = strdup(token);
            cell_count++;
            token = strtok(NULL, delim);
        }
        row_count++;
    }

    TableInfo data = {
        .rows = rows,
        .row_count = row_count,
    };
    wcsncpy(data.filename, CURRENT_FILENAME, MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN);

    return data;
}

int create_file(void) {
    if (file_exists()) return 1;
    
    FILE* file = _wfopen(CURRENT_FILENAME, L"w");
    fclose(file);

    return 0;
}

void write_widestr_to_table(wchar_t* wide_text, TableInfo* data, int row, int col) {
    char* text;
    wide_to_char(&wide_text, &text);

    if (data->rows[row].text[col] != NULL) {
        free(data->rows[row].text[col]);
        data->rows[row].text[col] = NULL;
    }

    data->rows[row].text[col] = text;
    return;
}

void save_file(void) { // TODO: добавить проверку на существование файла
    FILE* file = _wfopen(CURRENT_FILENAME, L"r");
    TableInfo data = read_csv(file);
    fclose(file);

    file = _wfopen(CURRENT_FILENAME, L"w");

    // Добавляем измененные ячейки в таблицу
    for (int i = 0; i < EDITED_TABLE_INFO.cells_count; i++) {
        int row = EDITED_TABLE_INFO.cells[i].row;
        int col = EDITED_TABLE_INFO.cells[i].col;

        if (row > data.row_count - 1) {
            data.row_count++;
            data.rows = (TableRow*)realloc(data.rows, data.row_count * sizeof(TableRow));
            for (int j = 0; j < MAX_COLS_IN_TABLE; j++) {
                data.rows[data.row_count - 1].text[j] = NULL;
            }
        }

        if (EDITED_TABLE_INFO.row_count_in_table < data.row_count) {
            data.row_count = EDITED_TABLE_INFO.row_count_in_table;
            data.rows = (TableRow*)realloc(data.rows, data.row_count * sizeof(TableRow));
        }

        wchar_t* wide_text = EDITED_TABLE_INFO.cells[i].text;
        write_widestr_to_table(wide_text, &data, row, col);
    }

    // Записываем измененную таблицу в файл
    for (int i = 0; i < data.row_count; i++) {
        for (int j = 0; j < MAX_COLS_IN_TABLE; j++) {
            char* cell_text = data.rows[i].text[j];
            if (cell_text == NULL) cell_text = "";

            fprintf(file, "%s", cell_text);

            if (j < MAX_COLS_IN_TABLE - 1) fprintf(file, ",");
        }
    }

    free(EDITED_TABLE_INFO.cells);
    EDITED_TABLE_INFO.cells = NULL;
    EDITED_TABLE_INFO.cells_count = 0;
    EDITED_TABLE_INFO.cells_size = 0;

    fclose(file);

    return;
}

bool ask_user(char* question) {
    char* full_question;
    strcpy(full_question, question);
    strcat(full_question, " [Y / n]");
    int full_question_len = strlen_utf8(full_question);

    WINDOW* ask_win = derwin(stdscr, 3, full_question_len + 2, getmaxy(stdscr) / 2, (getmaxx(stdscr) - full_question_len) / 2);
    
    wattron(ask_win, COLOR_PAIR(1));
    box(ask_win, 0, 0);
    mvwprintw(ask_win, 1, 1, full_question);
    wattroff(ask_win, COLOR_PAIR(1));
    
    wrefresh(ask_win);
    delwin(ask_win);

    while (1) {
        int pressed_char = getch();

        if (pressed_char == 'y' || pressed_char == 'Y') {
            return true;
        } else if (pressed_char == 'n' || pressed_char == 'N') {
            return false;
        }
    }
}
