#include <stdlib.h>
#include <pthread.h>
#include "widgets.h"
#include "service_functions.h"
#include "core.h"
#include "curses.h"
#include "interface.h"
#include "service_widgets.h"

void make_widget_homepage(WINDOW* win) {
    int selected_option = VERTICAL_SELECTED_OPTION;
    char* options[] = {
        "  Создать файл  ",  // STATE 1
        "  Открыть файл  ",  // STATE 2
        "      Выход     "          // EXIT
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

        if (selected_option == i) wattron(win, COLOR_PAIR(1));

        mvwaddstr(win,
            y_width / 2 - options_len + i,
            x_width / 2 - option_len / 2,
            options[i]);

        if (selected_option == i) wattroff(win, COLOR_PAIR(1));
    }
    wrefresh(win);

    return;
}

void make_widget_newfile(WINDOW* win) {
    if (ENTER_IS_PRESSED) {
        ENTER_IS_PRESSED = false;
        // create_file()
        // STATE: make_widget_editfile()
        return;
    }

    const int
        y_width = getmaxy(win),
        x_width = getmaxx(win);

    WINDOW* box_win = subwin(win, 3, (int)(x_width * .75), y_width / 2, (int)(x_width * .125));
    box(box_win, 0, 0);
    mvwprintw(box_win, 0, 2, "[ Введите имя нового файла ]");
    wrefresh(box_win);

    WINDOW* input_win = derwin(box_win, 1, getmaxx(box_win) - 2, 1, 1);
    keypad(input_win, TRUE);

    pthread_t input_handler_TID;
    input_handler_args args;
    args.win = input_win;
    args.buffer = (char*)calloc((int)(getmaxx(box_win) - 2), sizeof(char));
    pthread_create(&input_handler_TID, NULL, input_handler, &args);

    free(args.buffer);
    delwin(input_win);
    delwin(box_win);

    wrefresh(win);
    return;
}

void make_widget_openfile(WINDOW* win) {

    return;
}

void make_widget_about(WINDOW* win) {

    return;
}
