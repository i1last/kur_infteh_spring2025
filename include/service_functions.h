#ifndef SERVICE_FUNCTIONS_H
#define SERVICE_FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include "core.h"

void sort_table(TableInfo* data, int sort_col, bool ascending);

void wide_to_char(wchar_t** _sorce_wide_text, char** _dest_char_text);

void char_to_wide(char** _source_char_text, wchar_t** _dest_wide_text);

int convert_strlen_to_strlen_utf8(int condition, char** text);

WINDOW* create_box_input_window(WINDOW** win);

void create_input_menu(WINDOW** box_win);

unsigned smooth_selected_option(int selected_option, int condition);

int get_start_x_cord_of_cell(int cell_index, int cols_width[MAX_COLS_IN_TABLE], int string_len, int align);

unsigned strlen_utf8(const char* str);

bool file_exists(void);

TableInfo read_csv(FILE* file);

int create_file(void);

void write_widestr_to_table(wchar_t* wide_text, TableInfo* data, int row, int col);

void save_file(void);

bool ask_user(char* question);

#endif