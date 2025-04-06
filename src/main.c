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

    pthread_t keys_listener_TID, term_size_listener_TID, state_listener_TID;
    pthread_create(&keys_listener_TID, NULL, keys_listener, NULL);
    pthread_create(&term_size_listener_TID, NULL, term_size_listener, NULL);
    pthread_create(&state_listener_TID, NULL, state_listener, NULL);
    
    if (argc > 1) {
        char* extension = strstr(argv[1], ".csv");

        // Если .csv нет или не в конце строки
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
    
    while (IS_RUNNING);
    
    return 0;
}
/*
- Возможность сортировки записей по возрастанию и убыванию
- Поиск записей по названию
- численную обработку данных по выбору учащегося (подсчет среднего, поиск максимального и т. п.)
- простое шифрование данных
- Приведение wchar_t к char и char к wchar_t обобщить?
*/