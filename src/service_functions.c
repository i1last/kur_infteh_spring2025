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
    // 8. Картотека чертежей (деталь, формат, автор, год, кодовый номер).
    
    WIN32_FIND_DATA find_data;
    HANDLE hFind = FindFirstFile("*", &find_data);
    
    do {
        if (!strcmp(find_data.cFileName, BUFFER)) {
            FindClose(hFind);
            return 1;
        }
    } while (FindNextFile(hFind, &find_data));
    
    FindClose(hFind);
    
    FILE* file = fopen(BUFFER, "w");
    fclose(file);

    return 0;
}