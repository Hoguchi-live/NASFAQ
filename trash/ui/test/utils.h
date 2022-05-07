#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

