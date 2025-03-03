#include "service_functions.h"

unsigned len_of_string(const char* str) {
    unsigned len = 0;
    while (*str) {
        // Проверяем, является ли текущий байт началом символа UTF-8
        if ((*str & 0xC0) != 0x80) len++;
        str++;
    }
    return len;
}