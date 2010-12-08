#include "options.h"
#include "partie.h"

#ifdef __LIB_ncurses__
	#include "moteur_ncurses.h"
#elif __LIB_SDL__
	#include "moteur_sdl.h"
#endif

#include "debug.h"
#include "debug_memory.h"

/*******************************************************************************
* CONVENTION DE PROGRAMATION !
* Toutes les variables d'une class doivent commencer par "c_"
* Toutes les variables globales doivent commencer par "G_" et être écrites en MAJ
*
* BUG:
*	- Lors de la colision de 2 flammes
*
* A FAIRE
*	- Mettre des const la où il faut dans les *.h
*	- Optimiser avec des inline
*
*/


int main( int argc, char* arvg[] )
{
	freopen("bug.txt", "w", stderr);// Redirection du flux d'erreur dans un fichier
	srand(time(NULL));// Make random

	// Initialisation des options général
	options::getInstance();

	// Init Moteur d'affichage
	#ifdef __LIB_ncurses__
		moteur_ncurses m;
	#elif __LIB_SDL__
		moteur_sdl& m = moteur_sdl::getInstance();
	#endif

	// Appel des menus
	//**************************************************************************
	//**************************************************************************
	static const char* menu_main[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Jouer",						// NE PAS OUBLIER de modifier le for !
		"Options",
		"Quitter"
	};
	static const char* menu_options[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Port",								// NE PAS OUBLIER de modifier le for !
		"Clavier",
		"Retour"
	};
	static const char* menu_options_claviers[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Clavier Joueur 1",							// NE PAS OUBLIER de modifier le for !
		"Clavier Joueur 2",
		"Clavier Joueur 3",
		"Clavier Joueur 4",
		"Retour"
	};
	static const char* menu_jeu[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Offline",						// NE PAS OUBLIER de modifier le for !
		"Online",
		"Retour"
	};
	static const char* menu_replay[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Refaire une partie",				// NE PAS OUBLIER de modifier le for !
		"Changer de map",
		"Retour"
	};
	char titreNomJoueurNumI[30] = {0};
	char nomJoueurNumI[21] = {0};

	unsigned int entry = 0;
	int tmp = 0;
	bool retourMenuAudessus = 0;
	do{
		/***********************************************************************
		* Menu d'accueil
		*/
		switch( entry = m.menu("", menu_main, 3) )
		{
			/*******************************************************************
			* Jouer :: Type de partie
			*/
			case 1:{
				do{
					switch( entry = m.menu("Type de partie", menu_jeu, 3) )
					{
						/*******************************************************
						* Offline
						*/
						case 1:{// Offline
							while( m.getNombre("Entrez le nombre de joueur", 2, 2, 4, &tmp) != 3 )
							{
								// Menu suivant
								partie jeu;
								jeu.def_nbJoueurs(tmp);

								nomJoueurNumI[0] = 0;
								for( int i=0; i<tmp; i++ )
								{
									sprintf(titreNomJoueurNumI, "Joueur %d entrez votre nom", i+1);
									if( m.getTexte( titreNomJoueurNumI, nomJoueurNumI ) == 3 ){
										retourMenuAudessus = 1;
										break;// Permet de remonter au menu au dessus
									}

									jeu.joueur(i)->defNom(nomJoueurNumI);
									nomJoueurNumI[0] = 0;
								}

								if( !retourMenuAudessus ){
									do{
										m.forcerRafraichissement();
										//jeu.start( m, &CLASS_TO_USE::afficherMapEtEvent );
										jeu.main( m.afficherMapEtEvent );
									}while( m.menu("Un joueur a gagne !", menu_replay, 3) != 3 );
								}
								retourMenuAudessus = 0;
							}
							break;
						}
						/*******************************************************
						* Online
						*/
						case 2:{// Online
							break;
						}
						/*******************************************************
						* Retour au menu précédant
						*/
						case 3:{// Retour
							break;
						}
					}
				}while( entry != 3 );
				entry = 0;
				break;
			}
			/*******************************************************************
			* Menu des options
			*/
			case 2:{// Options
				do{
					switch( entry = m.menu("Options", menu_options, 3) )
					{
						/*******************************************************
						* Menu des options
						*/
						case 1:{// Port
								if( m.getNombre("Port pour le serveur", options::getInstance()->port(), 1, 9999, &tmp ) == 2 ){
									options::getInstance()->defPort( tmp );
									options::getInstance()->enregistrerConfig();
								}
							break;
						}

						/*******************************************************
						* Menu des claviers
						*/
						case 2:{// Claviers
							/*******************************************************
							* Menu des claviers
							*/
							do{
								switch( entry = m.menu("Options des Claviers", menu_options_claviers, 5) )
								{
									/*******************************************************
									* Config d'un clavier
									*/
									case 1:
									case 2:
									case 3:
									case 4:{// CONFIG CLAVIERS !
										m.afficherConfigurationClavier( entry );
										options::getInstance()->enregistrerConfig();
										break;
									}
									case 5:{// Retour
										break;
									}
								}
							}while( entry != 5 );
							entry=0;
							break;
						}

						/*******************************************************
						* Retour au menu précédant
						*/
						case 3:{// Retour
							break;
						}
					}
				}while( entry != 3 );
				entry=0;
				break;
			}

			case 3:{// Quiter
				break;
			}
		}
	}while( entry != 3 );

	//**************************************************************************
	//**************************************************************************

	#ifdef __LIB_SDL__
		delete &m;
	#endif


	// Désinitialisation des options général
	options::uInit();

	fclose(stderr);
	return EXIT_SUCCESS;
}
