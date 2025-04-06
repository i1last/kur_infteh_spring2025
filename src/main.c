#include <pthread.h>
#include <locale.h>
#include "curses.h"
#include "interface.h"
#include "core.h"

int main() {
    setlocale(LC_ALL, "");

    initscr();
    refresh();
    noecho();
    cbreak();
    curs_set(1);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_RED);

    make_tui();

    pthread_mutex_init(&mutex, NULL);

    pthread_t keys_listener_TID, term_size_listener_TID, state_listener_TID;
    pthread_create(&keys_listener_TID, NULL, keys_listener, NULL);
    pthread_create(&term_size_listener_TID, NULL, term_size_listener, NULL);
    pthread_create(&state_listener_TID, NULL, state_listener, NULL);
    while (IS_RUNNING);
    
    return 0;
}
/* TODO:
- Добавить название открытого файла в хеадер
- Добавить возможность открытия файла при запуске программы в качестве аргумента
*/