#include <pthread.h>
#include "curses.h"
#include "interface.h"
#include "core.h"

int main() {
    initscr();
    refresh();
    noecho();
    cbreak();
    curs_set(1);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_BLUE);

    make_tui(STATE);

    pthread_t keys_listener_TID, term_size_listener_TID;
    pthread_create(&keys_listener_TID, NULL, keys_listener, NULL);
    pthread_create(&term_size_listener_TID, NULL, term_size_listener, NULL);

    while (IS_RUNNING);

    return 0;
}
