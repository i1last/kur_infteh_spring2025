#ifndef SERVICE_FUNCTIONS_H
#define SERVICE_FUNCTIONS_H

#include <stdio.h>
#include "core.h"

unsigned len_of_string(const char* str);

int create_file(void);

TableRow* read_csv(FILE* file);

#endif