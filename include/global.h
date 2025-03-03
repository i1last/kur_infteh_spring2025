#ifndef GLOBAL_H
#define GLOBAL_H

extern bool IS_RUNNING;

extern int STATE;

extern int VERTICAL_SELECTED_OPTION;

extern int HORIZONTAL_SELECTED_OPTION;

unsigned len_of_string(const char* str);

void* keys_listener(void* arg);

void* term_size_listener(void* arg);

#endif