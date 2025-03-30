#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "curses.h"
#include "core.h"
#include "interface.h"

bool IS_RUNNING = true;
bool ENTER_IS_PRESSED = false;
bool NOT_ASCII_KEY_IS_PRESSED = false;
bool REGULAR_UPDATE = false;

int STATE = 0;
int VERTICAL_SELECTED_OPTION = 0;
int HORIZONTAL_SELECTED_OPTION = 0;
int CURRENT_BUFFER_LEN = 0;

unsigned TICK_COUNTER = 0;

char BUFFER[MAX_BUFFER_LEN] = { 0 };
char CURRENT_FILENAME[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN] = { 0 };

struct TableState TABLE_STATE = {
    .FIRST_ROW = 0,
    .SCROLL_OFFSET = 0,
    .RUNNING_TEXT_POS = 0,
    .LAST_UPDATE = 0
};

pthread_mutex_t mutex;

void* keys_listener(void* arg) {
    int pressed_char = 0;

    while (1) {
        switch (pressed_char = getch()) {
        case 27:  // ESC button == 27
            if (STATE) STATE = 0;
            else IS_RUNNING = false;
            break;
        case KEY_UP:
            TICK_COUNTER = 0;
            VERTICAL_SELECTED_OPTION--;
            break;
        case KEY_DOWN:
            TICK_COUNTER = 0;
            VERTICAL_SELECTED_OPTION++;
            break;
        case KEY_RIGHT:
            TICK_COUNTER = 0;
            HORIZONTAL_SELECTED_OPTION++;
            break;
        case KEY_LEFT:
            TICK_COUNTER = 0;
            HORIZONTAL_SELECTED_OPTION--;
            break;
        case 10: // ENTER button == 10
            ENTER_IS_PRESSED = true;
            break;
        case 8:  // BACKSPACE
            if (CURRENT_BUFFER_LEN > 0) {
                CURRENT_BUFFER_LEN--;
                BUFFER[CURRENT_BUFFER_LEN] = '\0';
            }
            break;
        default:
            if (CURRENT_BUFFER_LEN < MAX_BUFFER_LEN && (
                    48 <= pressed_char && pressed_char <= 57  ||  // 0..9
                    65 <= pressed_char && pressed_char <= 90  ||  // A..Z
                    97 <= pressed_char && pressed_char <= 122 ||  // a..z
                    pressed_char == 95                        ||  // _
                    pressed_char == 45                            // -
                )) {
                
                BUFFER[CURRENT_BUFFER_LEN] = pressed_char;
                CURRENT_BUFFER_LEN++;
            } else if (pressed_char > 128) NOT_ASCII_KEY_IS_PRESSED = true;
            break;
        }
        pthread_mutex_lock(&mutex);
        make_tui();
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* term_size_listener(void* arg) {
    while (1) if (is_termresized()) {
        pthread_mutex_lock(&mutex);
        make_tui();
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* state_listener(void* arg) {
    int prev_state = STATE;

    while (1) if (STATE != prev_state) {
        prev_state = STATE;
        memset(BUFFER, '\0', sizeof(BUFFER));
        CURRENT_BUFFER_LEN = 0;
        VERTICAL_SELECTED_OPTION = 0;
        HORIZONTAL_SELECTED_OPTION = 0;
        TICK_COUNTER = 0;
        ENTER_IS_PRESSED = false;
        REGULAR_UPDATE = false;
        
        pthread_mutex_lock(&mutex);
        make_tui();
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* regular_update_listener(void* arg) {
    static struct timespec ts;

    while (1) {
        if (REGULAR_UPDATE) {
            pthread_mutex_lock(&mutex);
            make_tui();
            pthread_mutex_unlock(&mutex);

            TICK_COUNTER++;

            ts.tv_sec = 0;
            ts.tv_nsec = 100 * 1000000L; // 200 мс = 200 000 000 нс
            nanosleep(&ts, NULL);
        }
    }

    return NULL;
}