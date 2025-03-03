#ifndef WIDGETS_H
#define WIDGETS_H

#include "curses.h"

void make_widget_homepage(WINDOW* win, unsigned selected_option);

void make_widget_openfile(WINDOW* win);

void make_widget_newfile(WINDOW* win);

void make_widget_writefile(WINDOW* win);

void make_widget_about(WINDOW* win);

#endif