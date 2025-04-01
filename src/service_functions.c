#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "curses.h"
#include "service_functions.h"
#include "core.h"

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

bool file_is_exists(void) {
    wchar_t filename[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN] = { 0 };
    wcscpy(filename, BUFFER);
    wcscat(filename, FILE_EXTENSION);

    WIN32_FIND_DATA find_data;
    HANDLE hFind = FindFirstFile("*", &find_data);

    do {
        if (!wcscmp((wchar_t*)find_data.cFileName, filename)) {
            FindClose(hFind);
            return true;
        }
    } while (FindNextFile(hFind, &find_data));

    FindClose(hFind);
    
    return false;
}

int create_file(void) {
    if (file_is_exists()) return 1;
    
    FILE* file = _wfopen(CURRENT_FILENAME, L"w");
    fclose(file);

    return 0;
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
        .row_count = row_count
    };

    return data;
}
