#include <string.h>
#include "interface.h"
#include "curses.h"

void init_curses(void) {
    initscr();
    cbreak();
    noecho();
}

void end_curses(void) {
    endwin();
}

void center(const char* text) {
    int height, width;
    getmaxyx(stdscr, height, width);

    int center_x = (width - strlen(text)) / 2;
    int center_y = height / 2;

    mvprintw(center_y, center_x, "%s", text);
    refresh();
}
