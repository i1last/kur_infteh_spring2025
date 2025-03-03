#include <stdlib.h>
#include <stdbool.h>
#include "curses.h"
#include "core.h"
#include "interface.h"

bool IS_RUNNING = true;
int STATE = 0;
int VERTICAL_SELECTED_OPTION = 0;
int HORIZONTAL_SELECTED_OPTION = 0;

void* keys_listener(void* arg) {
    while (1) {
        switch (getch()) {
        case KEY_UP:
            VERTICAL_SELECTED_OPTION--;
            if (VERTICAL_SELECTED_OPTION == 0) VERTICAL_SELECTED_OPTION--;
            break;
        case KEY_DOWN:
            VERTICAL_SELECTED_OPTION++;
            if (VERTICAL_SELECTED_OPTION == 0) VERTICAL_SELECTED_OPTION++;
            break;
        case KEY_RIGHT:
            HORIZONTAL_SELECTED_OPTION++;
            if (HORIZONTAL_SELECTED_OPTION == 0) HORIZONTAL_SELECTED_OPTION++;
            break;
        case KEY_LEFT:
            HORIZONTAL_SELECTED_OPTION--;
            if (HORIZONTAL_SELECTED_OPTION == 0) HORIZONTAL_SELECTED_OPTION--;
            break;
        case 27:  // ESC button == 27
            IS_RUNNING = false;
            break;
        }

        // make_tui(STATE);
    }

    return NULL;
}

void* term_size_listener(void* arg) {
    while (1) if (is_termresized()) make_tui(STATE);

    return NULL;
}
