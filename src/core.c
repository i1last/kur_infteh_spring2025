#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
#include "curses.h"
#include "core.h"
#include "interface.h"
#include "service_functions.h"

bool IS_RUNNING = true;
bool ENTER_IS_PRESSED = false;
bool UNDEFINED_KEY_IS_PRESSED = false;

int COMMAND_KEY_IS_PRESSED = 0;
int STATE = 0;
int SUB_STATE = 0;
int VERTICAL_SELECTED_OPTION = 0;
int HORIZONTAL_SELECTED_OPTION = 0;
int CURRENT_BUFFER_LEN = 0;
int CURSOR_POS = 0;

wchar_t BUFFER[MAX_BUFFER_LEN] = { 0 };
wchar_t CURRENT_FILENAME[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN] = { 0 };

TableInfo TABLE_INFO = { NULL, 0, false, 0, false, 0 };

pthread_mutex_t mutex;
pthread_cond_t is_running_cond;
pthread_cond_t state_changed_cond;

void* keys_listener(void* arg) {
    wint_t pressed_char = 0;

    while (IS_RUNNING) {
        get_wch(&pressed_char);
        pthread_mutex_lock(&mutex);

        switch (pressed_char) {
        case 27: // ESC
            if (STATE == 3 && TABLE_INFO.edited_cells) {
                if (ask_user("Сохранить файл?")) {
                    save_file();
                }
                STATE = 0;
                pthread_cond_signal(&state_changed_cond);
            } else if (STATE) {
                STATE = 0;
                pthread_cond_signal(&state_changed_cond);
            } else {
                IS_RUNNING = false;
                pthread_cond_broadcast(&is_running_cond);
            }
            break;

        case 330: // DELETE
            if (STATE == 3 && ask_user("Удалить строку?")) COMMAND_KEY_IS_PRESSED = pressed_char;
            break;

        case 19: // ctrl + s
            if (STATE == 3 && TABLE_INFO.edited_cells) save_file();
            break;

        case 14: // ctrl + n
            if (STATE == 3) COMMAND_KEY_IS_PRESSED = pressed_char;
            break;

        case 21: // ctrl + u
        case 4: // ctrl + d
        case 6: // ctrl + f
        case 9: // ctrl + i
            COMMAND_KEY_IS_PRESSED = pressed_char;
            break;

        case KEY_UP:
            VERTICAL_SELECTED_OPTION--;
            if (SUB_STATE) VERTICAL_SELECTED_OPTION++;
            break;

        case KEY_DOWN:
            VERTICAL_SELECTED_OPTION++;
            if (SUB_STATE) VERTICAL_SELECTED_OPTION--;
            break;

        case KEY_RIGHT:
            HORIZONTAL_SELECTED_OPTION++;
            CURSOR_POS = MIN(CURRENT_BUFFER_LEN, CURSOR_POS + 1);
            if (SUB_STATE) HORIZONTAL_SELECTED_OPTION--;
            break;

        case KEY_LEFT:
            HORIZONTAL_SELECTED_OPTION--;
            CURSOR_POS = MAX(1, CURSOR_POS - 1);
            if (SUB_STATE) HORIZONTAL_SELECTED_OPTION++;
            break;

        case 10: // ENTER
            ENTER_IS_PRESSED = true;
            pthread_cond_signal(&state_changed_cond);
            break;

        case 8: // BACKSPACE
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
                    pressed_char == 46                           ||  // .
                    pressed_char == 32 && STATE == 3                 // space; " " (only for table)
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

        make_tui();
        pthread_mutex_unlock(&mutex);
        Sleep(50);
    }

    return NULL;
}

void* term_size_listener(void* arg) {
    while (IS_RUNNING) {
        if (is_termresized()) {
            pthread_mutex_lock(&mutex);
            make_tui();
            pthread_mutex_unlock(&mutex);
        }
        Sleep(200);
    }

    return NULL;
}

void* state_listener(void* arg) {
    int prev_state = STATE;
    int prev_sub_state = SUB_STATE;

    while (IS_RUNNING) {
        pthread_mutex_lock(&mutex);

        while (IS_RUNNING && STATE == prev_state && SUB_STATE == prev_sub_state) {
            pthread_cond_wait(&state_changed_cond, &mutex);
        }

        if (STATE != prev_state || SUB_STATE != prev_sub_state) {
            memset(BUFFER, '\0', sizeof(BUFFER));
            CURRENT_BUFFER_LEN = 0;
            CURSOR_POS = 0;
            COMMAND_KEY_IS_PRESSED = 0;

            // if only STATE changed
            if (STATE != prev_state) {
                VERTICAL_SELECTED_OPTION = 0;
                HORIZONTAL_SELECTED_OPTION = 0;

                TABLE_INFO = (TableInfo){ NULL, 0, false, 0, false, 0 };
            }
            
            ENTER_IS_PRESSED = false;
            
            make_tui();
        }
        
        if (SUB_STATE == -1) SUB_STATE = 0;
        prev_state = STATE;
        prev_sub_state = SUB_STATE;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
