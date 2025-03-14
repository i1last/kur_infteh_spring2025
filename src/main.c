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
    // timeout(50);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_BLUE);

    make_tui();

    pthread_mutex_init(&mutex, NULL);

    pthread_t keys_listener_TID, term_size_listener_TID, state_listener_TID;
    pthread_create(&keys_listener_TID, NULL, keys_listener, NULL);
    pthread_create(&term_size_listener_TID, NULL, term_size_listener, NULL);
    pthread_create(&state_listener_TID, NULL, state_listener, NULL);

    while (IS_RUNNING);

    return 0;
}
// #include <curses.h>

// #define WIDTH 30
// #define HEIGHT 3

// int main(void) {
//     int ch;
//     int pos = 0;
//     char buffer[WIDTH - 2] = { 0 };

//     initscr();
//     cbreak();
//     noecho();
//     keypad(stdscr, TRUE);

//     // Создание основного окна с рамкой
//     WINDOW* win = newwin(HEIGHT, WIDTH, (LINES - HEIGHT) / 2, (COLS - WIDTH) / 2);
//     box(win, 0, 0);
//     mvwprintw(win, 0, 2, "[ Input ]");
//     wrefresh(win);

//     // Подокно для ввода текста
//     WINDOW* input = derwin(win, 1, WIDTH - 2, 1, 1);
//     keypad(input, TRUE); // Включаем обработку клавиш для подокна
//     curs_set(1);

//     while ((ch = wgetch(input)) != '\n') {
//         switch (ch) {
//         case KEY_BACKSPACE:
//         case 127:
//         case 8:
//             if (pos > 0) {
//                 pos--;
//                 buffer[pos] = '\0';
//                 mvwaddch(input, 0, pos, ' '); // Стираем символ
//                 wmove(input, 0, pos); // Возвращаем курсор
//             }
//             break;
//         default:
//             if (pos < (WIDTH - 3) && ch >= 32 && ch <= 126) {
//                 buffer[pos] = ch;
//                 mvwaddch(input, 0, pos, ch); // Выводим символ на экран! ← ключевое исправление
//                 pos++;
//                 wmove(input, 0, pos); // Перемещаем курсор
//             }
//             break;
//         }
//         wrefresh(input);
//     }

//     curs_set(0);
//     delwin(input);
//     delwin(win);
//     endwin();

//     printf("You entered: %s\n", buffer);
//     return 0;
// }