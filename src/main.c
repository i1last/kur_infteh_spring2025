#include "curses.h"
#include "interface.h"

int main() {
    init_curses();

    center("Привет, мир!");

    getch();

    init_curses();

    return 0;
}
