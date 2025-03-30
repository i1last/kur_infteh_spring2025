#ifndef SERVICE_FUNCTIONS_H
#define SERVICE_FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include "core.h"

unsigned smooth_selected_option(int selected_option, int condition);

int get_start_x_cord_of_cell(int cell_index, int cols_width[MAX_COLS_IN_TABLE], int string_len, int align);

unsigned len_of_string(const char* str);

bool file_is_exists(void);

int create_file(void);

TableInfo read_csv(FILE* file);

#endif