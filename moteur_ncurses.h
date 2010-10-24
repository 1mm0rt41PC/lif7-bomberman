#ifndef MOTEUR_NCURSES_h
#define MOTEUR_NCURSES_h
#include "config.h"
#include "options.h"
#include <stdlib.h>// Pour exit()
#include <string.h>// Pour strlen()

//#define WINDOW_HEIGHT 50
#define WINDOW_HEIGHT 7
//#define WINDOW_WIDTH 110
#define WINDOW_WIDTH 30

extern options G_OPTIONS;// Variable globale

/* curses.h
	COLOR_BLACK		0
	COLOR_RED		1
	COLOR_GREEN		2
	COLOR_YELLOW	3
	COLOR_BLUE		4
	COLOR_MAGENTA	5
	COLOR_CYAN		6
	COLOR_WHITE		7 // GRIS BLANC
	// OUT OF CURSES
	8 = GRIS
	9 = BLEU
	10 = vert, claire
	11 = bleu, claire
	12 = rouge, claire
	13 = violet, claire
	14 = jaune, claire
	15 = blanc
*/
#define COLOR_LIGHT_YELLOW 14
#define COLOR_LIGHT_RED 12

// Couleurs Multiplexées
#define MU_YELLOW_BLACK 1
#define MU_RED_BLACK 2

// Touches
#define KEY_ESCAP 27
#define KEY_ENTER_bis 10

class moteur_ncurses
{
	private:
		//WINDOW *c_win;

		void menuAccueil( int highlight );
		void cadre();
		void affichageTouche( WINDOW *win, int y, int x, int key );
		int obtenirNombre( char titre[], int valeurParDefaut, int ValeurMin, int ValeurMax );

	public:
		moteur_ncurses();
		~moteur_ncurses();

		void main();
		unsigned int menu( char titre[], char *choix[], unsigned int nb_choix );
		void afficherConfigurationClavier( unsigned char joueur );
};

#endif
