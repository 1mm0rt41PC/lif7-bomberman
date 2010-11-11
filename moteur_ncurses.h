#ifndef MOTEUR_NCURSES_h
#define MOTEUR_NCURSES_h

#include "config.h"
#include "options.h"
#include "partie.h"
#include <string.h>// Pour strlen()

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
#define MU_BLUE_BLACK 3
#define MU_GREEN_BLACK 4
#define MU_WHITE_BLACK 5

// Couleur des joueurs
#define MU_JOUEUR1 3
#define MU_JOUEUR2 2
#define MU_JOUEUR3 4
#define MU_JOUEUR4 1


class moteur_ncurses
{
	private:
		void cadre();
		unsigned int menu( const char titre[], const char *choix[], unsigned int nb_choix );
		void menuAccueil( int highlight );
		int obtenirNombre( const char titre[], int valeurParDefaut, int ValeurMin, int ValeurMax );
		void afficherConfigurationClavier( unsigned char joueur );
		void affichageTouche( WINDOW *win, int y, int x, int key );
		int getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* returnValue );
		int getTexte( const char titre[], char texteRetour[21] );
		void cleanline( WINDOW *win, int y, int x_begin, int x_end );
		inline void cleanline( WINDOW *win, int y ){ cleanline( win, y, 1, getmaxx(win)-2 ); }

	public:
		moteur_ncurses();
		~moteur_ncurses();

		// Autre
		void main();
		static SYS_CLAVIER afficherMapEtEvent( const partie* p );


		static chtype getCouleurJoueur( unsigned char joueur );
		static unsigned char getTailleNombre( int nb );
		static unsigned char getTailleNombre( unsigned int nb );
		static char* trimString( char texte[] );
};

#endif
