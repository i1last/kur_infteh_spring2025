#ifndef SERVICE_FUNCTIONS_H
#define SERVICE_FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include "core.h"

int convert_strlen_to_strlen_utf8(int condition, char** text);

WINDOW* create_box_input_window(WINDOW** win);

void create_input_menu(WINDOW** box_win);

unsigned smooth_selected_option(int selected_option, int condition);

int get_start_x_cord_of_cell(int cell_index, int cols_width[MAX_COLS_IN_TABLE], int string_len, int align);

unsigned strlen_utf8(const char* str);

bool file_is_exists(void);

int create_file(void);

TableInfo read_csv(FILE* file);

#endif