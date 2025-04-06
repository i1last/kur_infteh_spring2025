#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <pthread.h>
#include <wchar.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define MAX_BUFFER_LEN 64
#define MAX_COLS_IN_TABLE 5
#define MAX_FILE_EXTENSION_LEN 4
#define MIN_TERM_COLS 75
#define MIN_TERM_ROWS 20
#define MAX_LEN_IN_FILE 1024


#define FILE_EXTENSION L".csv"
#define __INPUTMENU__CHAR_NOT_SUPPORTED "[ (!) Данные символы не поддерживаются ]"

#define KEY "super_secret_key"
#define ENCRYPTION_SIGNATURE "ENC1"
#define SIGNATURE_LENGTH 4

typedef struct {
    char* text[MAX_COLS_IN_TABLE];
} TableRow;

typedef struct {
    TableRow* rows;
    unsigned row_count;
    bool edited_cells;
    wchar_t filename[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN];
    bool search_mode;
    char* search_text;
} TableInfo;

extern pthread_mutex_t mutex;

extern pthread_cond_t is_running_cond;

extern pthread_cond_t state_changed_cond;

extern bool IS_RUNNING;

extern bool ENTER_IS_PRESSED;

extern bool UNDEFINED_KEY_IS_PRESSED;

extern int COMMAND_KEY_IS_PRESSED;

extern int STATE;

extern int SUB_STATE;

extern int VERTICAL_SELECTED_OPTION;

extern int HORIZONTAL_SELECTED_OPTION;

extern int CURRENT_BUFFER_LEN;

extern int CURSOR_POS;

extern wchar_t BUFFER[MAX_BUFFER_LEN];

extern wchar_t CURRENT_FILENAME[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN];

extern TableInfo TABLE_INFO;

void* keys_listener(void* arg);

void* term_size_listener(void* arg);

void* state_listener(void* arg);

#endif
