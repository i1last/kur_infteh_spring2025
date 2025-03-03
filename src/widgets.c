#include <string.h>
#include "widgets.h"
#include "global.h"
#include "curses.h"

void make_widget_homepage(WINDOW* win, unsigned selected_option) {
    char* options[] = { "Открыть файл", "Создать файл", "Выход" };

    unsigned options_len = sizeof(options) / sizeof(options[0]);
    selected_option %= options_len;
    if (selected_option < 0) selected_option += options_len;

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

void make_widget_openfile(WINDOW* win) {

    return;
}

void make_widget_newfile(WINDOW* win) {

    return;
}

void make_widget_writefile(WINDOW* win) {

    return;
}

void make_widget_about(WINDOW* win) {

    return;
}
