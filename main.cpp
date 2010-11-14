#include "moteur_ncurses.h"
#include "options.h"

/*******************************************************************************
* CONVENTION DE PROGRAMATION !
* Toutes les variables d'une class doivent commencer par "c_"
* Toutes les variables globales doivent commencer par "G_" et �tre �crites en MAJ
*
*
* A FAIRE
*	- Mettre des const la o� il faut dans les *.h
*
*/



int main()
{
	freopen("bug.txt", "w", stderr);// Redirection du flux d'erreur dans un fichier
	srand(time(NULL));// Make random

	// Initialisation des options g�n�ral
	options::getInstance();

	// Init Moteur d'affichage
	moteur_ncurses m;

	// Appel des menus
	m.main();

	/* Ce truc tr�s basic, ne passe pas Valgrind
	initscr();

	delwin(stdscr);
	endwin();
	*/

	// D�sinitialisation des options g�n�ral
	options::uInit();

	fclose(stderr);
	return EXIT_SUCCESS;
}
