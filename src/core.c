#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <wchar.h>
#include "curses.h"
#include "core.h"
#include "interface.h"

bool IS_RUNNING = true;
bool ENTER_IS_PRESSED = false;
bool UNDEFINED_KEY_IS_PRESSED = false;

int STATE = 0;
int SUB_STATE = 0;
int VERTICAL_SELECTED_OPTION = 0;
int HORIZONTAL_SELECTED_OPTION = 0;
int CURRENT_BUFFER_LEN = 0;
int CURSOR_POS = 0;

wchar_t BUFFER[MAX_BUFFER_LEN] = { 0 };
wchar_t CURRENT_FILENAME[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN] = { 0 };

pthread_mutex_t mutex;

void* keys_listener(void* arg) {
    wint_t pressed_char = 0;

    while (1) {
        get_wch(&pressed_char);
        switch (pressed_char) {
        case 27:  // ESC button == 27
            if (STATE) STATE = 0;
            else IS_RUNNING = false;
            break;
        case KEY_UP:
            VERTICAL_SELECTED_OPTION--;
            break;
        case KEY_DOWN:
            VERTICAL_SELECTED_OPTION++;
            break;
        case KEY_RIGHT:
            HORIZONTAL_SELECTED_OPTION++;
            CURSOR_POS = MIN(CURRENT_BUFFER_LEN, CURSOR_POS + 1);
            break;
        case KEY_LEFT:
            HORIZONTAL_SELECTED_OPTION--;
            CURSOR_POS = MAX(1, CURSOR_POS - 1);
            break;
        case 10: // ENTER button == 10
            ENTER_IS_PRESSED = true;
            break;
        case 8:  // BACKSPACE
            if (CURRENT_BUFFER_LEN > 0) {
                CURSOR_POS--;
                
                wchar_t temp_buffer[MAX_BUFFER_LEN] = { 0 };
                for (int i = 0; i < CURRENT_BUFFER_LEN; ++i) {
                    if (i < CURSOR_POS) temp_buffer[i] = BUFFER[i];
                    else if (i == CURSOR_POS) continue;
                    else temp_buffer[i - 1] = BUFFER[i];
                    
                }
                wcscpy(BUFFER, temp_buffer);
                CURRENT_BUFFER_LEN--;
            }
            break;
        default:
            if (CURRENT_BUFFER_LEN < MAX_BUFFER_LEN && (
                    48   <= pressed_char && pressed_char <= 57   ||  // 0..9
                    65   <= pressed_char && pressed_char <= 90   ||  // A..Z
                    97   <= pressed_char && pressed_char <= 122  ||  // a..z
                    1040 <= pressed_char && pressed_char <= 1103 ||  // а..Я
                    pressed_char == 1105                         ||  // ё
                    pressed_char == 95                           ||  // _
                    pressed_char == 45                           ||  // -
                    pressed_char == 46                               // .
                )) {
                CURRENT_BUFFER_LEN++;
                
                wchar_t temp_buffer[MAX_BUFFER_LEN] = { 0 };
                for (int i = 0; i < CURRENT_BUFFER_LEN; ++i) {
                    if (i < CURSOR_POS) temp_buffer[i] = BUFFER[i];
                    else if (i == CURSOR_POS) temp_buffer[i] = pressed_char;
                    else temp_buffer[i] = BUFFER[i - 1];
                }
                wcscpy(BUFFER, temp_buffer);

                CURSOR_POS++;
            } else UNDEFINED_KEY_IS_PRESSED = true;
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
    int prev_sub_state = SUB_STATE;

    while (1) if (STATE != prev_state || SUB_STATE != prev_sub_state) {
        prev_state = STATE;
        
        memset(BUFFER, '\0', sizeof(BUFFER));
        CURRENT_BUFFER_LEN = 0;
        CURSOR_POS = 0;

        VERTICAL_SELECTED_OPTION = 0;
        HORIZONTAL_SELECTED_OPTION = 0;
        
        ENTER_IS_PRESSED = false;
        
        pthread_mutex_lock(&mutex);
        make_tui();
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
