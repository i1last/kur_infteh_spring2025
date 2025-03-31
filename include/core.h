#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX_BUFFER_LEN 64
#define MAX_COLS_IN_TABLE 5
#define MAX_FILE_EXTENSION_LEN 4
#define FILE_EXTENSION ".csv"

extern pthread_mutex_t mutex;

extern bool IS_RUNNING;

extern bool ENTER_IS_PRESSED;

extern bool NOT_ASCII_KEY_IS_PRESSED;

extern bool REGULAR_UPDATE;

extern int STATE;

extern int VERTICAL_SELECTED_OPTION;

extern int HORIZONTAL_SELECTED_OPTION;

extern int CURRENT_BUFFER_LEN;

extern int CURSOR_POS;

extern char BUFFER[MAX_BUFFER_LEN];

extern char CURRENT_FILENAME[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN];

extern struct TableState TABLE_STATE;

typedef struct {
    char* text[MAX_COLS_IN_TABLE];
} TableRow;

typedef struct {
    TableRow* rows;
    int row_count;
} TableInfo;

void* keys_listener(void* arg);

void* term_size_listener(void* arg);

void* state_listener(void* arg);

#endif