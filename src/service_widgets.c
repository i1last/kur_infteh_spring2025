#include "service_widgets.h"
#include "core.h"
#include "curses.h"

void* input_handler(void* args) {
    input_handler_args* arg = (input_handler_args*)args;

    int ch, pos = 0;
    while ((ch = getch()) != '\n') {
        switch (ch) {
        case KEY_BACKSPACE:
        case 127:  // DEL
        case 8:  // BACKSPACE
            if (pos > 0) {
                pos--;
                arg->buffer[pos] = '\0';
                mvwaddch(arg->win, 0, pos, ' ');
                wmove(arg->win, 0, pos);
            }
            break;
        default:
            if (pos < (-3) && ch >= 32 && ch <= 126) {
                arg->buffer[pos] = ch;
                mvwaddch(arg->win, 0, pos, ch);
                pos++;
                wmove(arg->win, 0, pos);
            }
            break;
        }
        wrefresh(arg->win);
    }

    return NULL;
}