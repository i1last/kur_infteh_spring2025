#include <stdio.h>
#include <windows.h>
#include "service_functions.h"
#include "core.h"

unsigned len_of_string(const char* str) {
    unsigned len = 0;
    while (*str) {
        // Проверяем, является ли текущий байт началом символа UTF-8
        if ((*str & 0xC0) != 0x80) len++;
        str++;
    }
    return len;
}

int create_file(void) {
    char filename[MAX_BUFFER_LEN + 4] = { 0 };
    strcpy(filename, BUFFER);
    strcat(filename, ".csv");
    
    WIN32_FIND_DATA find_data;
    HANDLE hFind = FindFirstFile("*", &find_data);
    
    do {
        if (!strcmp(find_data.cFileName, filename)) {
            FindClose(hFind);
            return 1;
        }
    } while (FindNextFile(hFind, &find_data));
    
    FindClose(hFind);
    
    FILE* file = fopen(filename, "w");
    fclose(file);

    return 0;
}

TableRow* read_csv(FILE* file) {
    char* delim = ";,";

    TableRow* data;

    int capacity = 20,
        row_count = 0;
    char buffer[1024];
    data = malloc(capacity * sizeof(TableRow));

    while ((fgets(buffer, sizeof(buffer), file)) != NULL) {
        if (row_count >= capacity) {
            capacity += 20;
            data = realloc(data, capacity * sizeof(TableRow));
        }

        char* token = strtok(buffer, delim);
        int cell_count = 0;
        while (cell_count != MAX_COLS_IN_TABLE) {
            data[row_count].text[cell_count] = strdup(token);
            cell_count++;
            token = strtok(NULL, delim);
        }
        row_count++;
    }

    return data;
}