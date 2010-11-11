#include "moteur_ncurses.h"
#include "options.h"

/*******************************************************************************
* CONVENTION DE PROGRAMATION !
* Toutes les variables d'une class doivent commencer par "c_"
* Toutes les variables globales doivent commencer par "G_" et être écrites en MAJ
*
*
* A FAIRE
* Mettre des const la où il faut dans les *.h
* rand !!!!
* Backspace mal géré par linux !?
* Bug dans getTexte()
*
* TODO or NOT
* Mode singleton pour options
*/

// Variables globales
options G_OPTIONS;// On peut pas avoir plusieurs instance d'options



int main()
{
	freopen("bug.txt", "w", stderr);
	srand(time(NULL));// Make random

	// Init Moteur d'affichage
	moteur_ncurses m;


	m.main();

	/* Ce truc très basic, ne passe pas Valgrind
	initscr();

	delwin(stdscr);
	endwin();
	*/

	fclose(stderr);
	return EXIT_SUCCESS;
}
