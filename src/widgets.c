#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
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
        int option_len = len_of_string(options[i]);
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

        if (selected_option == i) wattroff(win, COLOR_PAIR(1));  // WIP Могу убрать IF?
    }
    wrefresh(win);

    return;
}

void make_widget_newfile(WINDOW* win) {
    if (ENTER_IS_PRESSED && (!file_is_exists()) && (BUFFER[0] != '\0')) {
        ENTER_IS_PRESSED = false;
        strcpy(CURRENT_FILENAME, BUFFER);
        strcat(CURRENT_FILENAME, FILE_EXTENSION);
        create_file();
        STATE = 3;
        return;
    } else ENTER_IS_PRESSED = false;

    WINDOW* box_win = subwin(win, 3, MAX_BUFFER_LEN + 2, getmaxy(win) / 2, getmaxx(win) / 2 - MAX_BUFFER_LEN / 2);
    box(box_win, 0, 0);
    
    mvwprintw(box_win, 0, 2, "[ Введите имя нового файла ]");

    if (file_is_exists()) mvwprintw(box_win, 2, 2, " (!) Файл уже существует ");
    if (NOT_ASCII_KEY_IS_PRESSED) {
        mvwprintw(box_win, 2, 2, " (!) Поддерживается только латиница ");
        NOT_ASCII_KEY_IS_PRESSED = false;
    }
    
    wrefresh(box_win);

    WINDOW* input_win = derwin(box_win, getmaxy(box_win) - 2, getmaxx(box_win) - 2, 1, 1);
    mvwaddstr(input_win, 0, 0, BUFFER);

    delwin(input_win);
    delwin(box_win);

    wrefresh(win);
    return;
}

void make_widget_openfile(WINDOW* win) {
    if (ENTER_IS_PRESSED && file_is_exists() && (BUFFER[0] != '\0')) {
        ENTER_IS_PRESSED = false;
        strcpy(CURRENT_FILENAME, BUFFER);
        strcat(CURRENT_FILENAME, FILE_EXTENSION);
        STATE = 3;
        return;
    } else ENTER_IS_PRESSED = false;

    WINDOW* box_win = subwin(win, 3, MAX_BUFFER_LEN + 2, getmaxy(win) / 2, getmaxx(win) / 2 - MAX_BUFFER_LEN / 2);
    box(box_win, 0, 0);
    
    mvwprintw(box_win, 0, 2, "[ Введите имя файла ]");

    if (!file_is_exists() && (BUFFER[0] != '\0')) mvwprintw(box_win, 2, 2, " (!) Файла не существует ");
    if (NOT_ASCII_KEY_IS_PRESSED) {
        mvwprintw(box_win, 2, 2, " (!) Поддерживается только латиница ");
        NOT_ASCII_KEY_IS_PRESSED = false;
    }
    
    wrefresh(box_win);

    WINDOW* input_win = derwin(box_win, getmaxy(box_win) - 2, getmaxx(box_win) - 2, 1, 1);
    mvwaddstr(input_win, 0, 0, BUFFER);

    delwin(input_win);
    delwin(box_win);

    wrefresh(win);
    return;
}

void make_widget_writefile(WINDOW* win) {
    FILE* file = fopen(CURRENT_FILENAME, "r+");
    TableInfo data = read_csv(file);

    WINDOW* header_win = derwin(win, 2, getmaxx(win), 1, 0);
    // WINDOW* table_win  = derwin(win, getmaxy(win) - 4, getmaxx(win) - 2, 3, 1);
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
    
    for (int i = 0; i < MAX_COLS_IN_TABLE; i++) {
        int start_x_cord = get_start_x_cord_of_cell(i, cols_width, len_of_string(header[i]), table_options[i]);

        mvwaddstr(header_win, 0, start_x_cord, header[i]);

        if (i < (MAX_COLS_IN_TABLE - 1)) mvwvline(header_win, 0, get_start_x_cord_of_cell(i + 1, cols_width, 0, 0) - 2, ACS_VLINE, getmaxy(header_win));
    }
    
    mvwhline(header_win, 1, 2, ACS_HLINE, getmaxx(header_win) - 4);

    /********************************* TABLE  WIN ******************************************/
    for (int i = 0; i < table_height; i++) {
        if (i == data.row_count) break;

        for (int j = 0; j < MAX_COLS_IN_TABLE; j++) {
            unsigned text_row_index = i + TABLE_STATE.FIRST_ROW;
            unsigned text_col_index = j;
            char* text = data.rows[text_row_index].text[text_col_index];
            unsigned text_len = len_of_string(text);
            int start_x_cord = get_start_x_cord_of_cell(j, cols_width, text_len, table_options[j]);
            unsigned offset = 0;

            // if current cell is selected option
            if ((i == smooth_selected_option(VERTICAL_SELECTED_OPTION, data.row_count)) && 
                (j == smooth_selected_option(HORIZONTAL_SELECTED_OPTION, MAX_COLS_IN_TABLE))) {
                    wattron(table_win, A_REVERSE);
            }
            
            /*
            mvwaddnstr использует для счетчика символов в строке strlen, который некорректно считает количество
            символов в строке с кириллицей (не умеет работать с utf8). Именно поэтому используется переменная
            condition, которая реализует следующее уравнение:
                            len_of_string(text) = cols_width[j]
                            strlen(text) = x,
            где len_of_string(text) корректно считает количество символов в строке (собственная функция).
            Решаем уравнения накрест и получаем результат. 
            */
            int condition = cols_width[j] * strlen(text) / text_len;
            mvwaddnstr(table_win, i, start_x_cord, text, condition);
            
            if (j < (MAX_COLS_IN_TABLE - 1))
                mvwvline(table_win, 0, get_start_x_cord_of_cell(j + 1, cols_width, 0, 0) - 2, ACS_VLINE, getmaxy(table_win));
            // if ((text_len > cols_width - 1) &&
            //     (i == smooth_selected_option(VERTICAL_SELECTED_OPTION, data.row_count)) && 
            //     (j == smooth_selected_option(HORIZONTAL_SELECTED_OPTION, MAX_COLS_IN_TABLE))) {
            //     offset = offset_counter % (text_len + 3);
            // }

            // mvwaddnstr(table_win, i - TABLE_STATE.FIRST_ROW, start_x_cord, data.rows[i].text[j] + offset, cols_width - 1);
            // if (text_len - offset < cols_width - 1) {
            //     mvwaddnstr(table_win, i - TABLE_STATE.FIRST_ROW, start_x_cord + (text_len - offset), data.rows[i].text[j], cols_width - 1 - (text_len - offset));
            // }

            wattroff(table_win, A_REVERSE);
        }
    }

    wrefresh(table_win);
        
        
    delwin(table_win);
    delwin(scroll_win);

    wrefresh(win);
    
    fclose(file);
    return;
/****************************************************************************************************************************************************** */
        
    // werase(table_win);

    // int col_width = (table_width - 1) / COLS;
    // for(int i = 0; i < table_height; i++) {
    //     int data_row = state->start_row + i;
    //     if(data_row >= MAX_ROWS) break;
        
    //     for(int j = 0; j < COLS; j++) {
    //         int x = j * col_width;
            
    //         if(i == state->selected_row - state->start_row && j == state->selected_col) {
    //             wattron(table_win, A_REVERSE);
    //         }
            
    //         char *text = data[data_row].text[j];
    //         int len = strlen(text);
    //         int offset = 0;
            
    //         if(len > col_width - 1 && 
    //            data_row == state->selected_row && 
    //            j == state->selected_col) 
    //         {
    //             offset = state->running_text_pos % (len + 3);
    //         }
            
    //         mvwaddnstr(table_win, i, x + 1, text + offset, col_width - 1);
    //         if(len - offset < col_width - 1) {
    //             mvwaddnstr(table_win, i, x + 1 + (len - offset), 
    //                       text, col_width - 1 - (len - offset));
    //         }
            
    //         wattroff(table_win, A_REVERSE);
    //     }
    // }
    
    // for(int i = 0; i < COLS + 1; i++) {
    //     mvwvline(table_win, 0, i * col_width, ACS_VLINE, table_height);
    // }
    // whline(table_win, ACS_HLINE, table_width);
    
    // wnoutrefresh(table_win);
    
/****************************************************************************************************************************************************** */
    /********************************* SCROLL WIN ******************************************/


}

void make_widget_about(WINDOW* win) {

    return;
}
