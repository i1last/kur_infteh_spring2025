#include <string.h>
#include "interface.h"
#include "global.h"
#include "curses.h"

void init_curses(void) {
    initscr();
    refresh();
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);
    return;
}

void end_curses(void) {  // condition: ESC (27) button is pressed
    endwin();
    return;
}

void make_widget(int STATE) {
    switch (STATE) {
    case 0:
        printw("HOME");
        break;

    default:
        printw("DEFAULT");
        break;
    }
    return;
}

void make_window(void) {
    int x_size, y_size;
    getmaxyx(stdscr, y_size, x_size);

    WINDOW* board_win = newwin(y_size, x_size, 0, 0);
    box(board_win, 0, 0);
    wrefresh(board_win);

    char header[] = "[ К/Р: Рахметов А. Р., гр. 4494 ]";
    move(0, x_size / 2 - strlen(header) / 4);
    addstr(header);

    make_widget(STATE);

    char footer[] = "[ [Esc] - выход | [Enter] - подтвердить | Используйте стрелочки для управления ]";
    move(y_size - 1, 2);
    addstr(footer);

    return;
}

void update_tui(void) {
    clear();
    refresh();
    make_window();
    return;
}