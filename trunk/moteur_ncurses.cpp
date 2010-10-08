#include "moteur_ncurses.h"

#include <stdio.h>
#ifdef WIN32
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

/*******************************************************************************
*
*/
moteur_ncurses::moteur_ncurses()
{
	initscr();		/* passe l'écran texte en mode NCurses */
	clear();		/* efface l'écran */
	noecho();		/* Lorsqu'une touche est préssée au clavier, elle n'apparait pas à l'écran */
	cbreak();		/* un caractère est directement affiché, il ne passe pas dans un buffer */

	/* Creation d'une nouvelle fenetre en mode texte */
	/* => fenetre de dimension et position ( WIDTH, HEIGHT, STARTX,STARTY) */
	c_win = newwin( 500, 500, 10, 5 );
	keypad(c_win, TRUE);		/* pour que les flèches soient traitées (il faut le faire après création de la fenêtre) */

	halfdelay( 10 );
	/* notimeout(win,true); */
	/* wtimeout( win, 500 ); */
	/* nodelay(win,true); */

}


/*******************************************************************************
*
*/
void moteur_ncurses::getkey(int *key)
{
	*key = wgetch(c_win);
}


/*******************************************************************************
*
*/
void moteur_ncurses::ncursBcl()
{
	int key=0;

	do
	{
		//ncursAff( win, pJeu);

		key = wgetch(c_win);
		/* jeuActionClavier( jeu, 'd'); */
		switch(key)
		{
			case KEY_LEFT:
				//jeuActionClavier( pJeu, 'g');
				break;
			case KEY_RIGHT:
				//jeuActionClavier( pJeu, 'd');
				break;
			case KEY_UP:
				//jeuActionClavier( pJeu, 'h');
				break;
			case KEY_DOWN:
				//jeuActionClavier( pJeu, 'b');
				break;
		}
	} while (key != 27);/* ECHAP */

	endwin();
}
