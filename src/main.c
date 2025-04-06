#include <pthread.h>
#include <locale.h>
#include <string.h>
#include "curses.h"
#include "interface.h"
#include "core.h"
#include "service_functions.h"

int main(int argc, char* argv[]) {
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
    pthread_cond_init(&is_running_cond, NULL);
    pthread_cond_init(&state_changed_cond, NULL);

    pthread_t keys_listener_TID, term_size_listener_TID, state_listener_TID;
    pthread_create(&keys_listener_TID, NULL, keys_listener, NULL);
    pthread_create(&term_size_listener_TID, NULL, term_size_listener, NULL);
    pthread_create(&state_listener_TID, NULL, state_listener, NULL);
    
    if (argc > 1) {
        char* extension = strstr(argv[1], ".csv");

        // Если не оканчивается на ".csv"
        if (extension == NULL || extension[4] != '\0') {
            swprintf(BUFFER, MAX_BUFFER_LEN, L"%hs", argv[1]);
        } else {
            swprintf(BUFFER, MAX_BUFFER_LEN, L"%.*hs", (int)(extension - argv[1]), argv[1]);
        }

        if (file_exists()) {
            wcscpy(CURRENT_FILENAME, BUFFER);
            wcscat(CURRENT_FILENAME, FILE_EXTENSION);
            STATE = 3;
        }
    }
    
    while (IS_RUNNING) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&is_running_cond, &mutex);
        pthread_mutex_unlock(&mutex);
    }
    
    pthread_detach(keys_listener_TID);
    pthread_detach(term_size_listener_TID);
    pthread_detach(state_listener_TID);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&is_running_cond);
    pthread_cond_destroy(&state_changed_cond);

    endwin();

    return 0;
}
