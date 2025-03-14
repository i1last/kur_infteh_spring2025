#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <pthread.h>

#define MAX_BUFFER_LEN 64

extern pthread_mutex_t mutex;

extern bool IS_RUNNING;

extern bool ENTER_IS_PRESSED;

extern int STATE;

extern int VERTICAL_SELECTED_OPTION;

extern int HORIZONTAL_SELECTED_OPTION;

extern char BUFFER[MAX_BUFFER_LEN];

extern int CURRENT_BUFFER_LEN;

void* keys_listener(void* arg);

void* term_size_listener(void* arg);

void* state_listener(void* arg);

#endif