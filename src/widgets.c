#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "widgets.h"
#include "service_functions.h"
#include "core.h"
#include "curses.h"
#include "interface.h"

void make_widget_homepage(WINDOW* win) {
    char* options[] = {
        " Создать файл  ",  // STATE 1
        " Открыть файл  ",  // STATE 2
        " О работе      ",  // STATE 4
        " Выход         "   // EXIT
    };
    
    unsigned options_len = sizeof(options) / sizeof(options[0]);
    int selected_option = smooth_selected_option(VERTICAL_SELECTED_OPTION, options_len);

    if (ENTER_IS_PRESSED) {
        ENTER_IS_PRESSED = false;
        switch (selected_option) {
        case 0:
            STATE = 1;
            break;
        case 1:
            STATE = 2;
            break;
        case 2:
            STATE = 4;
            break;
        default:
            IS_RUNNING = false;
            break;
        }
        return;
    }

    int y_width = getmaxy(win),
        x_width = getmaxx(win);

    for (int i = 0; i < options_len; i++) {
        int option_len = strlen_utf8(options[i]);
        char* option = (char*)malloc(strlen(options[i]) + 2);


        if (selected_option == i) {
            wattron(win, COLOR_PAIR(1));
            option[0] = '*';
            strcpy(option + 1, options[i]);
        } else {
            option[0] = ' ';
            strcpy(option + 1, options[i]);
        }
        
        mvwaddstr(win,
            y_width / 2 - options_len + i,
            x_width / 2 - option_len / 2,
            option);

        wattroff(win, COLOR_PAIR(1));
    }
    wrefresh(win);

    return;
}

void make_widget_newfile(WINDOW* win) {
    if (ENTER_IS_PRESSED && (!file_exists()) && (BUFFER[0] != '\0')) {
        ENTER_IS_PRESSED = false;
        
        wcsncpy(CURRENT_FILENAME, BUFFER, MAX_BUFFER_LEN);
        wcscat(CURRENT_FILENAME, FILE_EXTENSION);
        
        create_file();
        
        STATE = 3;
        return;
    } else ENTER_IS_PRESSED = false;

    WINDOW* box_win = create_box_input_window(&win);
    
    mvwprintw(box_win, 0, 2, "[ Введите имя нового файла ]");

    if (file_exists()) mvwprintw(box_win, 2, 2, " (!) Файл уже существует ");
    if (UNDEFINED_KEY_IS_PRESSED) {
        mvwprintw(box_win, 2, 2, __INPUTMENU__CHAR_NOT_SUPPORTED);
        UNDEFINED_KEY_IS_PRESSED = false;
    }
    
    create_input_menu(&box_win);
    
    wrefresh(box_win);
    delwin(box_win);

    wrefresh(win);
    return;
}

void make_widget_openfile(WINDOW* win) {
    if (ENTER_IS_PRESSED && file_exists() && (BUFFER[0] != '\0')) {
        ENTER_IS_PRESSED = false;
        wcsncpy(CURRENT_FILENAME, BUFFER, MAX_BUFFER_LEN);
        wcscat(CURRENT_FILENAME, FILE_EXTENSION);
        STATE = 3;
        return;
    } else ENTER_IS_PRESSED = false;

    WINDOW* box_win = create_box_input_window(&win);
    
    mvwprintw(box_win, 0, 2, "[ Введите имя файла ]");

    if (!file_exists() && (BUFFER[0] != '\0')) mvwprintw(box_win, 2, 2, " (!) Файла не существует ");
    if (UNDEFINED_KEY_IS_PRESSED) {
        mvwprintw(box_win, 2, 2, __INPUTMENU__CHAR_NOT_SUPPORTED);
        UNDEFINED_KEY_IS_PRESSED = false;
    }
    
    create_input_menu(&box_win);
    
    wrefresh(box_win);
    delwin(box_win);

    wrefresh(win);
    return;
}

/*
SUB_STATEs:
0 - просмотр таблицы
1 - изменение ячейки
*/
void make_widget_writefile(WINDOW* win) {
    FILE* file = _wfopen(CURRENT_FILENAME, L"r+");
    static TableInfo data = { NULL, 0, { 0 } };
    if (wcscmp(data.filename, CURRENT_FILENAME)) 
        data = read_csv(file);

    WINDOW* header_win = derwin(win, 2, getmaxx(win), 1, 0);
    WINDOW* table_win  = derwin(win, getmaxy(win) - 4, getmaxx(win), 3, 0);
    WINDOW* scroll_win = subwin(win, getmaxy(win) - 4, 1, 4, getmaxx(win));
    
    int table_width = getmaxx(table_win),
        table_height = getmaxy(table_win);
    
        
    /********************************* HEADER WIN ******************************************/
    char* header[MAX_COLS_IN_TABLE] = { "Кодовый номер", "Деталь", "Формат", "Автор", "Год" };
    int table_options[MAX_COLS_IN_TABLE] = { 1, 0, 1, 0, 1 }; // 1 == center; 0 == left
    int reserved = 2 + 3 * (MAX_COLS_IN_TABLE - 1) + 2; // Слева 2 клетки, между ячейками 3 клетки и справа 2 клетки
    int cols_width[MAX_COLS_IN_TABLE] = { 
        (table_width - reserved) * 30 / 100,
        (table_width - reserved) * 35 / 100,
        (table_width - reserved) * 10 / 100,
        (table_width - reserved) * 20 / 100,
        (table_width - reserved) * 5 / 100,
    };
    
    int total_rows = MAX(data.row_count, 1);
    int visible_rows = table_height;
    
    for (int i = 0; i < MAX_COLS_IN_TABLE; i++) {
        int start_x_cord = get_start_x_cord_of_cell(i, cols_width, strlen_utf8(header[i]), table_options[i]);

        mvwaddstr(header_win, 0, start_x_cord, header[i]);

        if (i < (MAX_COLS_IN_TABLE - 1)) 
            mvwvline(header_win, 0, get_start_x_cord_of_cell(i + 1, cols_width, 0, 0) - 2, ACS_VLINE, getmaxy(header_win));
    }
    
    mvwhline(header_win, 1, 2, ACS_HLINE, getmaxx(header_win) - 4);

    /********************************* SCROLL LOGIC ******************************************/
    int absolute_v_selected = smooth_selected_option(VERTICAL_SELECTED_OPTION, total_rows);    
    static unsigned first_row = 0;

    // Основная логика прокрутки таблицы
    if (absolute_v_selected >= first_row + visible_rows) {
        first_row = absolute_v_selected - visible_rows + 1;
    } else if (absolute_v_selected < first_row) {
        first_row = absolute_v_selected;
    }

    // Защита от изменения размера окна
    if (first_row + visible_rows > total_rows) {
        first_row = total_rows - visible_rows;
    }
    if (first_row < 0 || total_rows < visible_rows) first_row = 0;
    
    int v_selected = absolute_v_selected - first_row;
    int h_selected = smooth_selected_option(HORIZONTAL_SELECTED_OPTION, MAX_COLS_IN_TABLE);

    /********************************* TABLE  WIN ******************************************/
    for (int i = 0; i < table_height; i++) {
        if (i >= total_rows) break;

        for (int j = 0; j < MAX_COLS_IN_TABLE; j++) {
            unsigned text_row_index = i + first_row;
            unsigned text_col_index = j;

            char* text = data.rows[text_row_index].text[text_col_index];
            if (text == NULL) text = " ";
            unsigned text_len = strlen_utf8(text);
            
            int start_x_cord = get_start_x_cord_of_cell(j, cols_width, text_len, table_options[j]);

            // if current cell is selected option
            if ((i == v_selected) && (j == h_selected)) wattron(table_win, A_REVERSE);
            
            int condition = convert_strlen_to_strlen_utf8(cols_width[j], &text);
            mvwaddnstr(table_win, i, start_x_cord, text, condition);
            
            wattroff(table_win, A_REVERSE);

            if (j < (MAX_COLS_IN_TABLE - 1))
                mvwvline(
                    table_win, 
                    0, 
                    get_start_x_cord_of_cell(j + 1, cols_width, 0, 0) - 2, 
                    ACS_VLINE, 
                    MIN(visible_rows, total_rows)
                );
        }
    }

    /********************************* SCROLL WIN ******************************************/
    int scroll_height = table_height;
    if (total_rows > table_height) {
        int scrollbar_size = (total_rows > visible_rows) ? (visible_rows * scroll_height / total_rows) : scroll_height;
        if (scrollbar_size < 1) scrollbar_size = 1;

        int scrollbar_pos = (first_row * (scroll_height - scrollbar_size)) / (total_rows - visible_rows);
        if (scrollbar_pos < 0) scrollbar_pos = 0;
        if (scrollbar_pos + scrollbar_size > scroll_height) scrollbar_pos = scroll_height - scrollbar_size;

        for (int i = 0; i < scroll_height; i++) {
            if (i >= scrollbar_pos && i < scrollbar_pos + scrollbar_size) {
                mvwaddch(scroll_win, i, 0, ACS_VLINE | A_REVERSE); // Ползунок
            } else {
                mvwaddch(scroll_win, i, 0, ACS_VLINE); // Фон полосы прокрутки
            }
        }
    }

    /********************************* KEYS LOGIC ******************************************/
    if (ENTER_IS_PRESSED && SUB_STATE == 0) {
        ENTER_IS_PRESSED = false;
        SUB_STATE = 1;
    } else if (ENTER_IS_PRESSED && SUB_STATE == 1) { // Изменение единичной ячейки
        ENTER_IS_PRESSED = false;

        unsigned last_cell = EDITED_TABLE_INFO.cells_count;

        // Выделяем память под новую ячейку
        if (last_cell >= EDITED_TABLE_INFO.cells_size) { // TODO: добавить проверку realloc_ptr и wcsdup на NULL
            unsigned new_size = EDITED_TABLE_INFO.cells_size + 1;
            TableCell* realloc_ptr = (TableCell*)realloc(EDITED_TABLE_INFO.cells, new_size * sizeof(TableCell));
            EDITED_TABLE_INFO.cells = realloc_ptr;
            EDITED_TABLE_INFO.cells_size = new_size;
        }
   
        // Записываем данные в новую ячейку
        EDITED_TABLE_INFO.cells[last_cell].col = h_selected;
        EDITED_TABLE_INFO.cells[last_cell].row = absolute_v_selected;
        EDITED_TABLE_INFO.cells[last_cell].text = wcsdup(BUFFER);
        
        write_widestr_to_table(
            EDITED_TABLE_INFO.cells[last_cell].text,
            &data,
            EDITED_TABLE_INFO.cells[last_cell].row,
            EDITED_TABLE_INFO.cells[last_cell].col
        );
    
        EDITED_TABLE_INFO.cells_count += 1;

        SUB_STATE = 0;
    } else if (CTRL_N_IS_PRESSED) { // Добавление новой строки
        CTRL_N_IS_PRESSED = false;

        unsigned last_cell = EDITED_TABLE_INFO.cells_count;

        // Выделяем память под новую строку
        if (last_cell + MAX_COLS_IN_TABLE - 1 >= EDITED_TABLE_INFO.cells_size) {
            unsigned new_size = EDITED_TABLE_INFO.cells_size + MAX_COLS_IN_TABLE;
            TableCell* realloc_ptr = (TableCell*)realloc(EDITED_TABLE_INFO.cells, new_size * sizeof(TableCell));
            EDITED_TABLE_INFO.cells = realloc_ptr;
            EDITED_TABLE_INFO.cells_size = new_size;
        }

        // Добавляем новую строку в отображаемую таблицу
        data.row_count++;
        data.rows = (TableRow*)realloc(data.rows, data.row_count * sizeof(TableRow));

        // Записываем данные о новой строке
        for (int i = 0; i < MAX_COLS_IN_TABLE; i++) {
            data.rows[data.row_count - 1].text[i] = '\0';
            EDITED_TABLE_INFO.cells[last_cell + i].col = i;
            EDITED_TABLE_INFO.cells[last_cell + i].row = data.row_count - 1;
            EDITED_TABLE_INFO.cells[last_cell + i].text = '\0';
        }

        EDITED_TABLE_INFO.cells_count += MAX_COLS_IN_TABLE;
        EDITED_TABLE_INFO.row_count_in_table = data.row_count;
    } else if (DELETE_IS_PRESSED) { // Удаление строки
        DELETE_IS_PRESSED = false;
        
        unsigned last_cell = EDITED_TABLE_INFO.cells_count;
        unsigned affected_rows = data.row_count - absolute_v_selected - 1;
        unsigned affected_cols = affected_rows * MAX_COLS_IN_TABLE;

        // Выделяем память под измененные ячейки
        if (last_cell + MAX_COLS_IN_TABLE - 1 >= EDITED_TABLE_INFO.cells_size) {
            EDITED_TABLE_INFO.cells_size += affected_cols;
            TableCell* realloc_ptr = (TableCell*)realloc(
                EDITED_TABLE_INFO.cells,
                EDITED_TABLE_INFO.cells_size * sizeof(TableCell)
            );
            EDITED_TABLE_INFO.cells = realloc_ptr;
        }

        // Сдвигаем строки вверх, замещая удаленную строку
        for (int i = absolute_v_selected; i < data.row_count - 1; i++) {
            data.rows[i] = data.rows[i + 1];

            for (int j = 0; j < MAX_COLS_IN_TABLE; j++) {
                unsigned edit_index = last_cell + ((i - absolute_v_selected) * MAX_COLS_IN_TABLE) + j;
                EDITED_TABLE_INFO.cells[edit_index].col = j;
                EDITED_TABLE_INFO.cells[edit_index].row = i;

                // Требуется приведение от char* к wchar_t*
                char_to_wide(&data.rows[i].text[j], &EDITED_TABLE_INFO.cells[edit_index].text);
            }
        }

        // Уменьшаем количество строк в отображаемой таблице
        data.row_count--;
        data.rows = (TableRow*)realloc(data.rows, data.row_count * sizeof(TableRow));

        // Обновляем количество отслеживаемых ячеек
        EDITED_TABLE_INFO.cells_count += affected_cols;
        EDITED_TABLE_INFO.row_count_in_table = data.row_count;
    }

    /********************************* EDIT MENU ******************************************/
    if (SUB_STATE == 1) {
        WINDOW* box_win = create_box_input_window(&win);
        
        if (UNDEFINED_KEY_IS_PRESSED) {
            mvwprintw(box_win, 2, 2, __INPUTMENU__CHAR_NOT_SUPPORTED);
            UNDEFINED_KEY_IS_PRESSED = false;
        }
        
        create_input_menu(&box_win);
        
        wrefresh(box_win);
        delwin(box_win);
    }
        
    wrefresh(table_win);
    wrefresh(scroll_win);
    
    delwin(table_win);
    delwin(scroll_win);
    
    fclose(file);

    return;
}

void make_widget_about(WINDOW* win) {

    return;
}
