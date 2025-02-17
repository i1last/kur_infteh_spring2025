#include <stdio.h>
#include "curses.h"

int main()
{
    // Инициализация PDCurses
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    // Проверка наличия цветов
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK); // Настройка цветовой пары
        attron(COLOR_PAIR(1));                // Включаем цвет
    }

    // Вывод текста
    printw("Hello, PDCurses!\n");
    printw("Press any key to exit...");

    // Обновляем экран
    refresh();

    // Ожидаем ввода от пользователя
    int ch = getch();
    if (ch == KEY_F(2))
    {
        printw("F1 pressed");
    }
    refresh();
    getch();

    // Завершаем работу с PDCurses
    endwin();

    return 0;
}
