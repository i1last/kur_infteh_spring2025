#ifndef INTERFACE_H
#define INTERFACE_H

#include <curses.h>

void make_widget(WINDOW* win, int STATE);

void make_box(WINDOW*);

void make_tui(int state);

#endif