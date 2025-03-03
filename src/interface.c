#include <string.h>
#include <stdlib.h>
#include "interface.h"
#include "curses.h"
#include "widgets.h"
#include "global.h"


void make_widget(WINDOW* win, int STATE) {
    switch (STATE) {
    case 1:  // new file
        // make_widget_newfile(win, y_size, x_size);
        break;
    case 2:  // open file
        // make_widget_openfile(win, y_size, x_size);
        break;
    case 3:  // write file
        // make_widget_writefile(win, y_size, x_size);
        break;
    case 4:  // about
        // make_widget_about(win, y_size, x_size);
        break;

    default:  // homepage
        // int selected_option = 0;
        // do {
        //     make_widget_homepage(win, selected_option);
        //     switch (getch()) {
        //     case KEY_UP:
        //         selected_option--;
        //         break;
        //     case KEY_DOWN:
        //         selected_option++;
        //         break;
        //     case 27:
        //         // end_curses();
        //         break;
        //     }
        // } while (1);
        break;
    }

    return;
}

void make_box(WINDOW* win) {
    box(win, 0, 0);

    char header[] = "[ К/Р: Рахметов А. Р., гр. 4494 ]";
    mvwaddstr(win, 0, getmaxx(win) / 2 - len_of_string(header) / 2, header);

    char footer[] = "[ ESC | Enter | ←→↑↓ ]";
    mvwaddstr(win, getmaxy(win) - 1, 2, footer);

    wrefresh(win);
    return;
}

void make_tui(int STATE) {
    static WINDOW* bg_win = NULL;
    static WINDOW* main_win = NULL;

    werase(bg_win);
    wrefresh(bg_win);
    delwin(bg_win);
    delwin(main_win);

    resize_term(0, 0);

    bg_win = newwin(LINES, COLS, 0, 0);
    main_win = subwin(bg_win, 0, 0, 1, 1);

    make_box(bg_win);
    make_widget(main_win, STATE);
    box(main_win, 0, 0);

    refresh();

    return;
}
