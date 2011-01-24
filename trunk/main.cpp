#include "options.h"
#include "partie.h"

#ifdef __LIB_ncurses__
	#include "moteur_ncurses.h"
#elif __LIB_SDL__
	#include "moteur_sdl.h"
#elif __LIB_SFML__
	#include "moteur_sfml.h"
#endif

#include "debug.h"

/*******************************************************************************
* CONVENTION DE PROGRAMATION !
* Toutes les variables d'une class doivent commencer par "c_"
* Toutes les variables globales doivent commencer par "G_" et être écrites en MAJ
*
* BUG:
*	- Lors de la colision de 2 flammes
*
* A FAIRE
*	- Bonus: super flamme
*	- fin partie
*	- menu mieux agencé
*	- inverseur de touche sur le client
*	- perso animer
*	- Finir de normaliser avec Marge_menu
*	- faire que les menus réagissent à un seul click au lieu de 2
*
*/


int main( int argc, char* arvg[] )
{
	(void)argc;
	(void)arvg;

	//  NE PAS OUBLIER DE CHANGER LE NOM "bug.log" EN BAS DANS LE remove() !
	freopen("bug.log", "w", stderr);// Redirection du flux d'erreur dans un fichier
	srand(time(NULL));// Make random

	// Initialisation des options général
	options::getInstance();

	// Init Moteur d'affichage
	#ifdef __LIB_ncurses__
		moteur_ncurses m;
	#elif __LIB_SDL__
		moteur_sdl& m = moteur_sdl::getInstance();
	#elif __LIB_SFML__
		moteur_sfml& m = moteur_sfml::getInstance();
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
	static const char* menu_online[] = {		// ATTENTION ! Si ajout / suppresion d'un menu,
		"Creer une partie (host)",				// NE PAS OUBLIER de modifier le for !
		"Joindre une partie (client)",
		"Retour"
	};
	char titreNomJoueurNumI[30] = {0};
	char nomJoueurNumI[21] = {0};
	char finPartie[70] = {0};
	char IP[21] = "127.0.0.1";// Taille max 16

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
			case 1: {
				do{
					switch( entry = m.menu("Type de partie", menu_jeu, 3) )
					{
						/*******************************************************
						* Offline
						*/
						case 1: {// Offline
							while( m.isWindowOpen() && m.getNombre("Entrez le nombre de joueur", 2, 2, 4, &tmp) != 3 && m.isWindowOpen() )
							{
								// Menu suivant
								partie jeu;
								jeu.def_nbJoueurs(tmp);

								for( int i=0; i<tmp && m.isWindowOpen(); i++ )
								{
									nomJoueurNumI[0] = 0;
									sprintf(titreNomJoueurNumI, "Joueur %d entrez votre nom", i+1);
									if( m.getTexte( titreNomJoueurNumI, nomJoueurNumI ) == 3 ){
										retourMenuAudessus = 1;
										break;// Permet de remonter au menu au dessus
									}

									jeu.joueur(i)->defNom(nomJoueurNumI);
								}

								if( !retourMenuAudessus && m.isWindowOpen() ){
									do{
										m.forcerRafraichissement();
										tmp = jeu.main( m.afficherMapEtEvent );
										if( tmp >= 1 )
											sprintf(finPartie, "Le joueur(%d) %s a gagne !", tmp, jeu.joueur(jeu.getWinnerID()-1)->nom()->c_str());
										if( tmp == 0 )
											sprintf(finPartie, "Match nul ! Pas de gagnant !");

									}while( tmp >= 0 && m.isWindowOpen() && m.menu(finPartie, menu_replay, 3) != 3 && m.isWindowOpen() );
								}
								retourMenuAudessus = 0;
							}
							break;
						}
						/*******************************************************
						* Online
						*/
						case 2: {// Online
							do{
								switch( entry = m.menu("Type de connection", menu_online, 3) )
								{
									/***********************************************
									* Host
									*/
									case 1: {// RESTE A DEF: le nb player total, nb player local
										/***************************************
										* Nombre de joueur TOTAL
										*/
										while( m.isWindowOpen() && m.getNombre("Nombre de joueur TOTAL", 2, 2, 4, &tmp) != 3 && m.isWindowOpen() )
										{
											partie jeu;
											jeu.def_connection( partie::CNX_Host );
											jeu.getServeur()->setPort(options::getInstance()->port());
											jeu.def_nbJoueurs(tmp);
											for( int i=0; i<tmp; i++ )// Par défaut, on met tout les joueur en mode NON LOCAL
												jeu.joueur(i)->defLocal( false );

											/***********************************
											* Nombre de joueur LOCAL
											*/
											while( m.isWindowOpen() && m.getNombre("Nombre de joueur LOCAL", 1, 1, tmp-1, &tmp) != 3 && m.isWindowOpen() )
											{
												for( int i=0; i<tmp && m.isWindowOpen(); i++ )
												{
													nomJoueurNumI[0] = 0;
													sprintf(titreNomJoueurNumI, "Joueur %d entrez votre nom", i+1);
													if( m.getTexte( titreNomJoueurNumI, nomJoueurNumI ) == 3 ){
														retourMenuAudessus = 1;
														break;// Permet de remonter au menu au dessus
													}

													jeu.joueur(i)->defNom(nomJoueurNumI);
													jeu.joueur(i)->defLocal( true );
												}
												if( !retourMenuAudessus && m.isWindowOpen() ){
													do{
														m.forcerRafraichissement();
														tmp = jeu.main( m.afficherMapEtEvent );
														if( tmp >= 1 )
															sprintf(finPartie, "Le joueur(%d) %s a gagne !", tmp, jeu.joueur(jeu.getWinnerID()-1)->nom()->c_str());
														if( tmp == 0 )
															sprintf(finPartie, "Match nul ! Pas de gagnant !");

													}while( tmp >= 0 && m.isWindowOpen() && m.menu(finPartie, menu_replay, 3) != 3 && m.isWindowOpen() );
												}
												retourMenuAudessus = 0;
											}
										}
										break;
									}
									/***********************************************
									* Client
									*/
									case 2: {
										while( m.isWindowOpen() && m.getTexte( "Veuillez entrez l'adresse IP du serveur", IP ) != 3 && m.isWindowOpen() )
										{
											partie jeu;
											jeu.def_connection( partie::CNX_Client );
											jeu.getClient()->setPort(options::getInstance()->port());
											jeu.getClient()->setServerAdress(IP);
											jeu.def_nbJoueurs(1);

											// On récup le nom du joueur
											nomJoueurNumI[0] = 0;
											if( m.getTexte("Joueur, Entrez votre nom", nomJoueurNumI ) == 3 ){
												retourMenuAudessus = 1;
												break;// Permet de remonter au menu au dessus
											}

											jeu.joueur(0)->defNom(nomJoueurNumI);
											jeu.joueur(0)->defLocal( false );

											if( !retourMenuAudessus && m.isWindowOpen() ){
												do{
													m.forcerRafraichissement();
													tmp = jeu.main( m.afficherMapEtEvent );
													if( tmp >= 1 )
														sprintf(finPartie, "Le joueur(%d) %s a gagne !", tmp, jeu.joueur(jeu.getWinnerID()-1)->nom()->c_str());
													if( tmp == 0 )
														sprintf(finPartie, "Match nul ! Pas de gagnant !");

												}while( tmp >= 0 && m.isWindowOpen() && m.menu(finPartie, menu_replay, 3) != 3 && m.isWindowOpen() );
											}
											retourMenuAudessus = 0;
										}
										break;
									}
									/***********************************************
									* Retour au menu précédant
									*/
									case 3: {// Retour
										break;
									}
								}
							}while( entry != 3 && m.isWindowOpen() );
							break;
						}
						/*******************************************************
						* Retour au menu précédant
						*/
						case 3: {// Retour
							break;
						}
					}
				}while( entry != 3 && m.isWindowOpen() );
				entry = 0;
				break;
			}
			/*******************************************************************
			* Menu des options
			*/
			case 2: {// Options
				do{
					switch( entry = m.menu("Options", menu_options, 3) )
					{
						/*******************************************************
						* Menu des options
						*/
						case 1: {// Port
								if( m.getNombre("Port pour le serveur", options::getInstance()->port(), 1, 9999, &tmp ) == 2 ){
									options::getInstance()->defPort( tmp );
									options::getInstance()->enregistrerConfig();
								}
							break;
						}

						/*******************************************************
						* Menu des claviers
						*/
						case 2: {// Claviers
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
									case 4: {// CONFIG CLAVIERS !
										m.afficherConfigurationClavier( entry );
										options::getInstance()->enregistrerConfig();
										break;
									}
									case 5: {// Retour
										break;
									}
								}
							}while( entry != 5 && m.isWindowOpen() );
							entry=0;
							break;
						}

						/*******************************************************
						* Retour au menu précédant
						*/
						case 3: {// Retour
							break;
						}
					}
				}while( entry != 3 && m.isWindowOpen() );
				entry=0;
				break;
			}

			case 3: {// Quiter
				break;
			}
		}
	}while( entry != 3 && m.isWindowOpen() );

	//**************************************************************************
	//**************************************************************************

	#ifdef __LIB_SDL__
		delete &m;
	#endif


	// Désinitialisation des options général
	options::uInit();


	fseek(stderr , 0, SEEK_END ); //On trouve la fin du fichier
	if( !ftell(stderr) ){
		fclose(stderr);
		remove("bug.log");
	}else{
		fclose(stderr);
	}

	return EXIT_SUCCESS;
}
