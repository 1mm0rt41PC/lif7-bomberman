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
*
* TODO or NOT
* Mode singleton pour options
*/

// Variables globales
options G_OPTIONS;// On peut pas avoir plusieurs instance d'options



int main()
{
	// Init Moteur d'affichage
	moteur_ncurses m;


	m.main();


	return EXIT_SUCCESS;
}
