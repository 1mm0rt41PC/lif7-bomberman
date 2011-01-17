#ifndef MOTEUR_NCURSES_h
#define MOTEUR_NCURSES_h

#include <string.h>// Pour strlen()
#include "partie.h"
#include "config.h"


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

// Couleurs Multiplexées
#define MU_YELLOW_BLACK 1
#define MU_RED_BLACK 2
#define MU_BLUE_BLACK 3
#define MU_GREEN_BLACK 4
#define MU_WHITE_BLACK 5
#define MU_BLACK_YELLOW 6
#define MU_BLACK_MAGENTA 7

// Couleur des joueurs
#define MU_JOUEUR1 3
#define MU_JOUEUR2 2
#define MU_JOUEUR3 4
#define MU_JOUEUR4 1


/*!
* @class moteur_ncurses
* @brief Gère l'affichage via le moteur "graphique" NCurses
*
* @note Aucune variable n'est membre de ce module
*/
class moteur_ncurses
{
	private:
		static bool c_premierAffichage;

	private:
		void cadre();
		void affichageTouche( WINDOW* win, int y, int x, int key );
		static void cleanline( WINDOW* win, int y, int x_begin, int x_end );
		inline void cleanline( WINDOW* win, int y ){ cleanline( win, y, 1, getmaxx(win)-2 ); }
		static chtype getCouleurJoueur( unsigned char joueur );

	public:
		moteur_ncurses();
		~moteur_ncurses();

		// Fonction d'affichage de menu
		unsigned int menu( const char titre[], const char *choix[], unsigned int nb_choix );
		void afficherConfigurationClavier( unsigned char joueur );
		int getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* returnValue );
		int getTexte( const char titre[], char texteRetour[21] );
		inline bool isWindowOpen(){ return true; }

		// Modificateur
		inline void forcerRafraichissement() { c_premierAffichage = 1; }

		// Autres :: Affichage map
		static SYS_CLAVIER afficherMapEtEvent( partie* p );
};

#endif
