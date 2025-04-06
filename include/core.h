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

#define FILE_EXTENSION L".csv"
#define __INPUTMENU__CHAR_NOT_SUPPORTED "[ (!) Данные символы не поддерживаются ]"

extern pthread_mutex_t mutex;

extern bool IS_RUNNING;

extern bool ENTER_IS_PRESSED;

extern bool UNDEFINED_KEY_IS_PRESSED;

extern bool CTRL_N_IS_PRESSED;

extern bool DELETE_IS_PRESSED;

extern int STATE;

extern int SUB_STATE;

extern int VERTICAL_SELECTED_OPTION;

extern int HORIZONTAL_SELECTED_OPTION;

extern int CURRENT_BUFFER_LEN;

extern int CURSOR_POS;

extern wchar_t BUFFER[MAX_BUFFER_LEN];

extern wchar_t CURRENT_FILENAME[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN];

typedef struct {
    int col, row;
    wchar_t* text;
} TableCell;

typedef struct {
    TableCell* cells;
    unsigned cells_size;
    unsigned cells_count;
    unsigned row_count_in_table;
} EditedTableInfo;

typedef struct {
    char* text[MAX_COLS_IN_TABLE];
} TableRow;

typedef struct {
    TableRow* rows;
    int row_count;
    wchar_t filename[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN];
} TableInfo;

extern EditedTableInfo EDITED_TABLE_INFO;

void* keys_listener(void* arg);

void* term_size_listener(void* arg);

void* state_listener(void* arg);

#endif
