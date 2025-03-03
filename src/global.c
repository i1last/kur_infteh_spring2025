#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "curses.h"
#include "global.h"
#include "interface.h"

bool IS_RUNNING = true;
int STATE = 0;
int VERTICAL_SELECTED_OPTION = 0;
int HORIZONTAL_SELECTED_OPTION = 0;

unsigned len_of_string(const char* str) {
    unsigned len = 0;
    while (*str) {
        // Проверяем, является ли текущий байт началом символа UTF-8
        if ((*str & 0xC0) != 0x80) len++;
        str++;
    }
    return len;
}

void* keys_listener(void* arg) {
    while (1) {
        switch (getch()) {
        case KEY_UP:
            VERTICAL_SELECTED_OPTION++;
            break;
        case KEY_DOWN:
            VERTICAL_SELECTED_OPTION--;
            break;
        case KEY_RIGHT:
            HORIZONTAL_SELECTED_OPTION++;
            break;
        case KEY_LEFT:
            HORIZONTAL_SELECTED_OPTION--;
            break;
        case 27:  // ESC button == 27
            IS_RUNNING = false;
            break;
        }
    }

    return NULL;
}

void* term_size_listener(void* arg) {
    while (1) if (is_termresized()) {
        make_tui(STATE);
    }

    return NULL;
}

