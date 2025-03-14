#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "widgets.h"
#include "service_functions.h"
#include "core.h"
#include "curses.h"
#include "interface.h"

void make_widget_homepage(WINDOW* win) {
    int selected_option = VERTICAL_SELECTED_OPTION;
    char* options[] = {
        "  Создать файл  ",  // STATE 1
        "  Открыть файл  ",  // STATE 2
        "  Выход         "   // EXIT
    };

    unsigned options_len = sizeof(options) / sizeof(options[0]);
    selected_option %= options_len;
    if (selected_option < 0) selected_option += options_len;

    if (ENTER_IS_PRESSED) {
        ENTER_IS_PRESSED = false;
        switch (selected_option) {
        case 0:
            STATE = 1;
            break;
        case 1:
            STATE = 2;
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
        } else strcpy(option, options[i]);
        
        mvwaddstr(win,
            y_width / 2 - options_len + i,
            x_width / 2 - option_len / 2,
            option);

        if (selected_option == i) wattroff(win, COLOR_PAIR(1));
    }
    wrefresh(win);

    return;
}

void make_widget_newfile(WINDOW* win) {
    bool file_is_exists = false;

    if (ENTER_IS_PRESSED && (BUFFER[0] != '\0')) {
        ENTER_IS_PRESSED = false;
        if (!create_file()) {
            STATE = 2;
            CURRENT_FILENAME = BUFFER;
            return;
        } else file_is_exists = true;
    } else if (ENTER_IS_PRESSED) ENTER_IS_PRESSED = false;

    const int
        y_width = getmaxy(win),
        x_width = getmaxx(win);

    WINDOW* box_win = subwin(win, 3, MAX_BUFFER_LEN + 2, y_width / 2, x_width / 2 - MAX_BUFFER_LEN / 2);
    box(box_win, 0, 0);
    
    mvwprintw(box_win, 0, 2, "[ Введите имя нового файла ]");

    if (file_is_exists) mvwprintw(box_win, 2, 2, " (!) Файл уже существует ");
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
    CURRENT_FILENAME = "data";
    char filename[MAX_BUFFER_LEN + 4] = { 0 };
    strcpy(filename, CURRENT_FILENAME);
    strcat(filename, ".csv");

    FILE* file = fopen(filename, "r+");
    
    TableRow* data = read_csv(file);
    

    fclose(file);
    return;
}

void make_widget_about(WINDOW* win) {

    return;
}
