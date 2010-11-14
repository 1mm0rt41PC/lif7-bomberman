#include "moteur_ncurses.h"
#include "options.h"

/*******************************************************************************
* CONVENTION DE PROGRAMATION !
* Toutes les variables d'une class doivent commencer par "c_"
* Toutes les variables globales doivent commencer par "G_" et être écrites en MAJ
*
*
* A FAIRE
*	- Mettre des const la où il faut dans les *.h
*
*/



int main()
{
	freopen("bug.txt", "w", stderr);// Redirection du flux d'erreur dans un fichier
	srand(time(NULL));// Make random

	// Initialisation des options général
	options::getInstance();

	// Init Moteur d'affichage
	moteur_ncurses m;

	// Appel des menus
	m.main();

	/* Ce truc très basic, ne passe pas Valgrind
	initscr();

	delwin(stdscr);
	endwin();
	*/

	// Désinitialisation des options général
	options::uInit();

	fclose(stderr);
	return EXIT_SUCCESS;
}
