#include "nc.h"

int main(int argc, char *argv[])
{
    WINDOW *my_win;
    int startx, starty, width, height;
    int ch;
    int margin_x, margin_y;
    int MAX_ROW, MAX_COL;
    int MAX_WIN_ROW, MAX_WIN_COL;

    initscr();			/* Start curses mode 		*/
    raw();			/* Line buffering disabled, Pass on*/
    noecho();
    keypad(stdscr, TRUE);	/* I need that nifty F2 	*/

    /* Window parameters */
    height = 10;
    width = 20;
    starty = (LINES - height) / 2;	/* Calculating for a center placement */
    startx = (COLS - width) / 2;	/* of the window		*/

    /* Grid parameters */
    getmaxyx(stdscr, MAX_ROW, MAX_COL);         /* get the number of rows and columns */
    margin_x = 1;
    margin_y = 1;
    MAX_WIN_ROW = MAX_ROW / (height + margin_y);
    MAX_WIN_COL = MAX_COL / (width + margin_x);
    WINDOW *win_array[MAX_WIN_COL][MAX_WIN_ROW];

    printw("Press F2 to exit");

    mvprintw(2, 0, "Nbr of win col: %d, nbr of win lines: %d", MAX_WIN_COL, MAX_WIN_ROW);
    refresh();

	do {
	for( int i = 0; i < MAX_WIN_COL; i ++ ){
		for( int j = 0; j < MAX_WIN_ROW; j ++ ){

			win_array[i][j] = create_newwin(height, width, j * (height + margin_y), i * (width + margin_x));
			refresh();
		}
	}

	}while( (ch = getch()) != KEY_F(2));

	//for( int i = 0; i <= MAX_WIN_COL; i ++ ){
	//	for( int j = 0; j <= MAX_WIN_ROW; j ++ ){

	//		destroy_win(win_array[i][j]);
	//	}
	//}

    //while((ch = getch()) != KEY_F(2))
    //{
    //    switch(ch)
    //    {
    //        case KEY_LEFT:
    //    	destroy_win(my_win);
    //    	my_win = create_newwin(height, width, starty,--startx);
    //    	break;

    //        case KEY_RIGHT:
    //    	destroy_win(my_win);
    //    	my_win = create_newwin(height, width, starty,++startx);
    //    	break;

    //        case KEY_UP:
    //    	destroy_win(my_win);
    //    	my_win = create_newwin(height, width, --starty,startx);
    //    	break;

    //        case KEY_DOWN:
    //    	destroy_win(my_win);
    //    	my_win = create_newwin(height, width, ++starty,startx);
    //    	break;
    //    }
    //}
    endwin();			/* End curses mode		  */
    return 0;
}

