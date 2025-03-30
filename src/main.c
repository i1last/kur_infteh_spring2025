#include <pthread.h>
#include "curses.h"
#include "interface.h"
#include "core.h"

int main() {
    initscr();
    refresh();
    noecho();
    cbreak();
    curs_set(1);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_RED);

    make_tui();

    pthread_mutex_init(&mutex, NULL);

    pthread_t keys_listener_TID, term_size_listener_TID, state_listener_TID, regular_update_listener_TID;
    pthread_create(&keys_listener_TID, NULL, keys_listener, NULL);
    pthread_create(&term_size_listener_TID, NULL, term_size_listener, NULL);
    pthread_create(&state_listener_TID, NULL, state_listener, NULL);
    pthread_create(&regular_update_listener_TID, NULL, regular_update_listener, NULL);

    while (IS_RUNNING);
    
    return 0;
}

/*
- ИНогда может не хватать размера окна, это проблема
- if fopen(filename, "..") == NULL -- сделать обработчик, если не удалось открыть файл
*/





























// #include <curses.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <unistd.h>

// #define COLS 5
// #define MAX_ROWS 100
// #define MAX_CELL_WIDTH 20
// #define MAX_CELL_LEN 50

// typedef struct {
//     int start_row;
//     int selected_row;
//     int selected_col;
//     int scroll_offset;
//     int running_text_pos;
//     time_t last_update;
// } TableState;

// typedef struct {
//     char **text; // Динамический массив строк (столбцов)
// } TableRow;

// TableRow *data; // Динамический массив строк таблицы
// WINDOW *win, *table_win;
// int table_height, table_width;

// void init_data(int rows, int cols, int cell_len) {
//     data = malloc(rows * sizeof(TableRow));
//     for (int i = 0; i < rows; i++) {
//         data[i].text = malloc(cols * sizeof(char *));
//         for (int j = 0; j < cols; j++) {
//             data[i].text[j] = malloc(cell_len * sizeof(char));
//             snprintf(data[i].text[j], cell_len, "Row %d Col %d %*s", 
//                     i+1, j+1, (i+j) % 10 + 5, "++++");
//         }
//     }
// }

// void free_data(int rows, int cols) {
//     for (int i = 0; i < rows; i++) {
//         for (int j = 0; j < cols; j++) {
//             free(data[i].text[j]);
//         }
//         free(data[i].text);
//     }
//     free(data);
// }

// void draw_slider(TableState *state) {
//     int win_height = table_height;
//     int max_pos = MAX_ROWS - win_height;
//     float position = (float)state->start_row / max_pos;
//     int slider_height = win_height * (win_height / (float)MAX_ROWS);
//     slider_height = slider_height < 1 ? 1 : slider_height;
    
//     int slider_pos = position * (win_height - slider_height);
    
//     for(int i = 0; i < win_height; i++) {
//         mvwaddch(win, i+1, table_width + 1, ' ');
//         if(i >= slider_pos && i < slider_pos + slider_height) {
//             mvwaddch(win, i+1, table_width + 1, ACS_BLOCK);
//         } else {
//             mvwaddch(win, i+1, table_width + 1, ACS_VLINE);
//         }
//     }
// }

// void draw_table(TableState *state) {
//     werase(table_win);
    
//     int col_width = (table_width - 1) / COLS;
//     for(int i = 0; i < table_height; i++) {
//         int data_row = state->start_row + i;
//         if(data_row >= MAX_ROWS) break;
        
//         for(int j = 0; j < COLS; j++) {
//             int x = j * col_width;
            
//             if(i == state->selected_row - state->start_row && j == state->selected_col) {
//                 wattron(table_win, A_REVERSE);
//             }
            
//             char *text = data[data_row].text[j];
//             int len = strlen(text);
//             int offset = 0;
            
//             if(len > col_width - 1 && 
//                data_row == state->selected_row && 
//                j == state->selected_col) 
//             {
//                 offset = state->running_text_pos % (len + 3);
//             }
            
//             mvwaddnstr(table_win, i, x + 1, text + offset, col_width - 1);
//             if(len - offset < col_width - 1) {
//                 mvwaddnstr(table_win, i, x + 1 + (len - offset), 
//                           text, col_width - 1 - (len - offset));
//             }
            
//             wattroff(table_win, A_REVERSE);
//         }
//     }
    
//     for(int i = 0; i < COLS + 1; i++) {
//         mvwvline(table_win, 0, i * col_width, ACS_VLINE, table_height);
//     }
//     whline(table_win, ACS_HLINE, table_width);
    
//     wnoutrefresh(table_win);
// }

// void handle_input(TableState *state, int ch) {
//     switch(ch) {
//         case KEY_UP:
//             if(state->selected_row > 0) {
//                 state->selected_row--;
//                 state->scroll_offset = 0;
//                 if(state->selected_row < state->start_row) {
//                     state->start_row = state->selected_row;
//                 }
//             }
//             break;
//         case KEY_DOWN:
//             if(state->selected_row < MAX_ROWS - 1) {
//                 state->selected_row++;
//                 state->scroll_offset = 0;
//                 if(state->selected_row >= state->start_row + table_height) {
//                     state->start_row = state->selected_row - table_height + 1;
//                 }
//             }
//             break;
//         case KEY_LEFT:
//             if(state->selected_col > 0) {
//                 state->selected_col--;
//                 state->scroll_offset = 0;
//             }
//             break;
//         case KEY_RIGHT:
//             if(state->selected_col < COLS - 1) {
//                 state->selected_col++;
//                 state->scroll_offset = 0;
//             }
//             break;
//     }
// }

// int main() {
//     initscr();
//     cbreak();
//     noecho();
//     keypad(stdscr, TRUE);
//     curs_set(0);
//     timeout(50);

//     int parent_h, parent_w;
//     getmaxyx(stdscr, parent_h, parent_w);
    
//     // Создаем родительское окно с рамкой
//     win = newwin(parent_h - 4, parent_w - 4, 2, 2);
//     box(win, 0, 0);
    
//     // Размеры области для таблицы
//     table_height = parent_h - 6;
//     table_width = (parent_w - 6) ;
    
//     // Создаем окно для таблицы
//     table_win = derwin(win, table_height, table_width, 1, 1);
    
//     init_data(MAX_ROWS, 5, MAX_CELL_LEN);
    
//     TableState state = {
//         .start_row = 0,
//         .selected_row = 0,
//         .selected_col = 0,
//         .scroll_offset = 0,
//         .running_text_pos = 0,
//         .last_update = time(NULL)
//     };

//     while(1) {
//         werase(win);
//         box(win, 0, 0);
        
//         // Обновление позиции бегущего текста
//         state.running_text_pos++;
        
//         draw_table(&state);
//         draw_slider(&state);
        
//         wnoutrefresh(win);
//         doupdate();
        
//         int ch = getch();
//         if(ch == 'q') break;
//         handle_input(&state, ch);
//     }

//     free_data(MAX_ROWS, 5);
//     delwin(table_win);
//     delwin(win);
//     endwin();
//     return 0;
// }