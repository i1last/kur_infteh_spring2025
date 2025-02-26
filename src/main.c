#include "curses.h"
#include "interface.h"

int main() {
    init_curses();


    int ch;
    do {
        if (is_termresized()) resize_term(0, 0);

        update_tui();
    } while ((ch = getch()) != 27);


    end_curses();
    return 0;
}
