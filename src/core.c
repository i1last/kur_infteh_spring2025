#include <stdlib.h>
#include <stdbool.h>
#include "curses.h"
#include "core.h"
#include "interface.h"

bool IS_RUNNING = true;
bool ENTER_IS_PRESSED = false;
int STATE = 0;
int VERTICAL_SELECTED_OPTION = 0;
int HORIZONTAL_SELECTED_OPTION = 0;

void* keys_listener(void* arg) {
    while (1) {
        switch (getch()) {
        case KEY_UP:
            VERTICAL_SELECTED_OPTION--;
            if (VERTICAL_SELECTED_OPTION == 0) VERTICAL_SELECTED_OPTION--;
            make_tui();
            break;
        case KEY_DOWN:
            VERTICAL_SELECTED_OPTION++;
            if (VERTICAL_SELECTED_OPTION == 0) VERTICAL_SELECTED_OPTION++;
            make_tui();
            break;
        case KEY_RIGHT:
            HORIZONTAL_SELECTED_OPTION++;
            if (HORIZONTAL_SELECTED_OPTION == 0) HORIZONTAL_SELECTED_OPTION++;
            make_tui();
            break;
        case KEY_LEFT:
            HORIZONTAL_SELECTED_OPTION--;
            if (HORIZONTAL_SELECTED_OPTION == 0) HORIZONTAL_SELECTED_OPTION--;
            make_tui();
            break;
        case 10: // ENTER button == 10
            ENTER_IS_PRESSED = true;
            make_tui();
            break;
        case 27:  // ESC button == 27
            IS_RUNNING = false;
            break;
        }
    }

    return NULL;
}

void* term_size_listener(void* arg) {
    while (1) if (is_termresized()) make_tui();

    return NULL;
}

void* state_listener(void* arg) {
    int prev_state = STATE;

    while (1) if (STATE != prev_state) {
        prev_state = STATE;
        make_tui();
    }

    return NULL;
}