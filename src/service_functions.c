#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "curses.h"
#include "service_functions.h"
#include "core.h"

WINDOW* create_box_input_window(WINDOW** win) {
    WINDOW* box_win = subwin(*win, 3, MAX_BUFFER_LEN + 2, getmaxy(*win) / 2, getmaxx(*win) / 2 - MAX_BUFFER_LEN / 2);
    box(box_win, 0, 0);

    return box_win;
}

void create_input_menu(WINDOW** box_win) {
    WINDOW* input_win = derwin(*box_win, getmaxy(*box_win) - 2, getmaxx(*box_win) - 2, 1, 1);
    mvwaddstr(input_win, 0, 0, BUFFER);
    delwin(input_win);
    return;
}

unsigned smooth_selected_option(int selected_option, int condition) {
    selected_option %= condition;
    if (selected_option < 0) selected_option += condition;
    return selected_option;
}

int get_start_x_cord_of_cell(int cell_index, int cols_width[MAX_COLS_IN_TABLE], int string_len, int align) {
    int start_x_cord = 2;
    for (int i = 0; i < cell_index; i++) {
        start_x_cord += cols_width[i] + 3;
    }

    if (align == 1) // center
        start_x_cord += MAX((cols_width[cell_index] - string_len) / 2, 0);
    
    return start_x_cord;
}

unsigned len_of_string(const char* str) {
    unsigned len = 0;
    while (*str) {
        // Проверяем, является ли текущий байт началом символа UTF-8
        if ((*str & 0xC0) != 0x80) len++;
        str++;
    }
    return len;
}

bool file_is_exists(void) {
    char filename[MAX_BUFFER_LEN + MAX_FILE_EXTENSION_LEN] = { 0 };
    strcpy(filename, BUFFER);
    strcat(filename, FILE_EXTENSION);

    WIN32_FIND_DATA find_data;
    HANDLE hFind = FindFirstFile("*", &find_data);

    do {
        if (!strcmp(find_data.cFileName, filename)) {
            FindClose(hFind);
            return true;
        }
    } while (FindNextFile(hFind, &find_data));

    FindClose(hFind);
    
    return false;
}

int create_file(void) {
    if (file_is_exists()) return 1;
    
    FILE* file = fopen(CURRENT_FILENAME, "w");
    fclose(file);

    return 0;
}

TableInfo read_csv(FILE* file) {
    char* delim = ";,";

    TableRow* rows;

    int capacity = 20,
        row_count = 0;
    char buffer[1024];
    rows = malloc(capacity * sizeof(TableRow));

    while ((fgets(buffer, sizeof(buffer), file)) != NULL) {
        if (row_count >= capacity) {
            capacity += 20;
            rows = realloc(rows, capacity * sizeof(TableRow));
        }

        char* token = strtok(buffer, delim);
        int cell_count = 0;
        while (cell_count != MAX_COLS_IN_TABLE) {
            rows[row_count].text[cell_count] = strdup(token);
            cell_count++;
            token = strtok(NULL, delim);
        }
        row_count++;
    }

    TableInfo data = {
        .rows = rows,
        .row_count = row_count
    };

    return data;
}
