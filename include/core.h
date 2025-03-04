#ifndef CORE_H
#define CORE_H

#include <stdbool.h>

extern bool IS_RUNNING;

extern bool ENTER_IS_PRESSED;

extern int STATE;

extern int VERTICAL_SELECTED_OPTION;

extern int HORIZONTAL_SELECTED_OPTION;

void* keys_listener(void* arg);

void* term_size_listener(void* arg);

void* state_listener(void* arg);

#endif