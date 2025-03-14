#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <pthread.h>

#define MAX_BUFFER_LEN 64
#define MAX_COLS_IN_TABLE 5

extern pthread_mutex_t mutex;

extern bool IS_RUNNING;

extern bool ENTER_IS_PRESSED;

extern bool NOT_ASCII_KEY_IS_PRESSED;

extern int STATE;

extern int VERTICAL_SELECTED_OPTION;

extern int HORIZONTAL_SELECTED_OPTION;

extern int CURRENT_BUFFER_LEN;

extern char BUFFER[MAX_BUFFER_LEN];

extern char* CURRENT_FILENAME;

typedef struct {
    char* text[MAX_COLS_IN_TABLE];
} TableRow;


void* keys_listener(void* arg);

void* term_size_listener(void* arg);

void* state_listener(void* arg);

#endif