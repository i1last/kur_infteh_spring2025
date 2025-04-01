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
/*
- Нет функции редактирования ячеек. Как будет очищаться буффер?
- Нет функции добавления новой строки в таблице
- Нет функции проверки, сохранен ли файл (перед выходом)
- Нет функции принудительного сохранения файла
- Ограничить минимальный размер окна
*/