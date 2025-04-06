#include <string.h>
#include <stdlib.h>
#include "interface.h"
#include "curses.h"
#include "widgets.h"
#include "core.h"
#include "service_functions.h"


void make_widget(WINDOW* win) {
    switch (STATE) {
    case 1:  // new file
        make_widget_newfile(win);
        break;
    case 2:  // open file
        make_widget_openfile(win);
        break;
    case 3:  // write file
        make_widget_writefile(win);
        break;
    case 4:  // about
        make_widget_about(win);
        break;

    default:  // homepage
        make_widget_homepage(win);
        break;
    }

    return;
}

void make_box(WINDOW* win) {
    box(win, 0, 0);

    char* header = "[ К/Р: Рахметов А. Р., гр. 4494 ]";
    mvwaddstr(win, 0, getmaxx(win) / 2 - strlen_utf8(header) / 2, header);

    char* footer = "[ ESC | Enter | ←→↑↓ | Ctrl + S | Ctrl + N ]";
    mvwaddstr(win, getmaxy(win) - 1, 2, footer);

    if (STATE == 3) {
        const unsigned footer_filename_size = 16;
        char footer_filename[footer_filename_size];

        wchar_t truncated_name[16] = { 0 }; // 12 символов + "..." + '\0'
        if (wcslen(CURRENT_FILENAME) > footer_filename_size - 4) {
            wcsncpy(truncated_name, CURRENT_FILENAME, footer_filename_size - 4);
            wcscat(truncated_name, L"...");
            truncated_name[footer_filename_size - 1] = '\0';
        } else {
            wcscpy(truncated_name, CURRENT_FILENAME);
        }

        snprintf(footer_filename, footer_filename_size + 5, "[ %ls%s ]", 
                truncated_name, 
                (EDITED_TABLE_INFO.cells_count > 0) ? "*" : ""
        );
        
        mvwaddstr(
            win,
            getmaxy(win) - 1,
            getmaxx(win) - strlen_utf8(footer_filename) - 2,
            footer_filename
        );
    }
    return;
}

void make_tui() {
    static WINDOW* bg_win = NULL;
    static WINDOW* main_win = NULL;

    delwin(bg_win);
    delwin(main_win);

    resize_term(0, 0);
    
    bg_win = newwin(LINES, COLS, 0, 0);
    main_win = subwin(bg_win, 0, 0, 1, 1);
    
    if (COLS >= MIN_TERM_COLS && LINES >= MIN_TERM_ROWS){
        make_box(bg_win);
        make_widget(main_win);
    } else {
        char* attention = "Размер окна слишком мал!";
        mvwaddstr(bg_win, LINES / 2, (COLS - strlen_utf8(attention)) / 2, attention);
    }
    
    wrefresh(bg_win);
    wrefresh(main_win);

    return;
}
