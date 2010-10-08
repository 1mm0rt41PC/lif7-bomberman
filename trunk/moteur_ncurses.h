#ifndef MOTEUR_NCURSES_h
#define MOTEUR_NCURSES_h

#include <stdio.h>
#ifdef WIN32
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

class moteur_ncurses
{
	private:
		WINDOW *c_win;

	public:
		moteur_ncurses();
		void getkey(int *key);
		void ncursBcl();
		//void ncursAff( WINDOW *win, pJeu);
};

#endif
