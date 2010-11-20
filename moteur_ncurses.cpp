#include "moteur_ncurses.h"

extern SCREEN* SP;// <- Salté de NCurses ( Compiler NCurses avec NO_LEAK ! )

/***************************************************************************//*!
* @fn moteur_ncurses::moteur_ncurses()
* @brief Initialise la class moteur_ncurses
*
* Initialise l'écran ncurses et prépare le "terrain" pour les traitements future
*/
moteur_ncurses::moteur_ncurses()
{
	if( !initscr() )// Passe l'écran texte en mode NCurses
		stdErrorE("Erreur lors de l'initialisation de ncurses !");

	clear();		/* Efface l'écran */
	noecho();		/* Lorsqu'une touche est préssée au clavier, elle n'apparait pas à  l'écran */
	cbreak();		/* Un caractère est directement affiché, il ne passe pas dans un buffer */
					/* In the cbreak() mode these control characters are interpreted as any other character by the terminal driver. */
	/*raw();*/		/* In the raw() mode these characters are directly passed to the program without generating a signal. */
	keypad(stdscr, TRUE);		/* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	if( !has_colors() ){
		this->~moteur_ncurses();
		stdErrorE("Votre terminal ne supporte pas les couleurs ! STOP ALL...");
	}
	start_color();
	init_pair(MU_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
	init_pair(MU_RED_BLACK, COLOR_RED, COLOR_BLACK);
	init_pair(MU_BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
	init_pair(MU_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
	init_pair(MU_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);

	// init_color(COLOR_RED, 700, 0, 0);
	/* param 1     : color name
	 * param 2, 3, 4 : rgb content min = 0, max = 1000 */

	curs_set(0);// Cache le cursor tout moche

	// Permet de changer la taille du terminal
	//resize_term(50, 100);

	#if defined(WIN32) || defined(WIN64)
		PDC_set_title("Bomberman");
	#endif

/*
	halfdelay( 2 );
	notimeout( c_win, true );
	wtimeout( c_win, 500 );
	nodelay( c_win, true );
*/
}


/***************************************************************************//*!
* @fn moteur_ncurses::~moteur_ncurses()
* @brief Désinitialise la class moteur_ncurses
*
* -# Rétablis le cursor orginel
* -# Nétoie l'écran
* -# Supprime l'écran NCurses
* -# Désactive NCurses
*/
moteur_ncurses::~moteur_ncurses()
{
	curs_set(1);// <- j'aime mon curseur dans mon term !!!
	clear();
	refresh(); /* Print it on to the real screen */
	delwin(stdscr);
	endwin();/* End curses mode */
	delscreen(SP);// <- Salté de NCurses ( Compiler NCurses avec NO_LEAK ! )
}


/***************************************************************************//*!
* @fn void moteur_ncurses::cadre()
* @brief Affiche un cadre au tour (interieur) de la fenêtre
*/
void moteur_ncurses::cadre()
{
	clear();// Efface l'écran

	attron(COLOR_PAIR(MU_YELLOW_BLACK));
	mvaddstr(1, (getmaxx(stdscr)-9/*Taille du mot*/)/2, "Bomberman");
	attroff(COLOR_PAIR(MU_YELLOW_BLACK));

	box(stdscr, 0, 0);

	refresh(); /* Print it on to the real screen */
}



/***************************************************************************//*!
* @fn void moteur_ncurses::main()
* @brief Point d'accès général.
*
* Appeler cette fonction pour lancer l'interface ncurses.<br />
* C'est dans cette fonction que l'on gère les menu
* @todo Déplacer cette fonction et la mettre en commun avec toutes les lib d'affichage
*/
void moteur_ncurses::main()
{
	static const char* menu_main[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Jouer",			// NE PAS OUBLIER de modifier le for !
		"Options",
		"Quitter"
	};
	static const char* menu_options[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Port",					// NE PAS OUBLIER de modifier le for !
		"Clavier",
		"Retour"
	};
	static const char* menu_options_claviers[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Clavier Joueur 1",				// NE PAS OUBLIER de modifier le for !
		"Clavier Joueur 2",
		"Clavier Joueur 3",
		"Clavier Joueur 4",
		"Retour"
	};
	static const char* menu_jeu[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Offline",			// NE PAS OUBLIER de modifier le for !
		"Online",
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
		switch( entry = menu("", menu_main, 3) )
		{
			/*******************************************************************
			* Jouer :: Type de partie
			*/
			case 1:{
				do{
					switch( entry = menu("Type de partie", menu_jeu, 3) )
					{
						/*******************************************************
						* Offline
						*/
						case 1:{// Offline
							while( getNombre("Entrez le nombre de joueur", 2, 2, 4, &tmp) != 3 )
							{
								// Menu suivant
								partie jeu;
								jeu.def_nbJoueurs(tmp);

								nomJoueurNumI[0] = 0;
								for( int i=0; i<tmp; i++ )
								{
									sprintf(titreNomJoueurNumI, "Joueur %d entrez votre nom", i+1);
									if( getTexte( titreNomJoueurNumI, nomJoueurNumI ) == 3 ){
										retourMenuAudessus = 1;
										break;// Permet de remonter au menu au dessus
									}

									jeu.joueur(i)->defNom(nomJoueurNumI);
									nomJoueurNumI[0] = 0;
								}

								if( !retourMenuAudessus ){
									nodelay( stdscr, true );
									jeu.main( afficherMapEtEvent );
									nodelay( stdscr, false );
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
					switch( entry = menu("Options", menu_options, 3) )
					{
						/*******************************************************
						* Menu des options
						*/
						case 1:{// Port
								if( getNombre("Port pour le serveur", options::getInstance()->port(), 1, 9999, &tmp ) == 2 ){
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
								switch( entry = menu("Options des Claviers", menu_options_claviers, 5) )
								{
									/*******************************************************
									* Config d'un clavier
									*/
									case 1:
									case 2:
									case 3:
									case 4:{// CONFIG CLAVIERS !
										afficherConfigurationClavier( entry );
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
	//entry = menu( menu_options, 3 );
}


/***************************************************************************//*!
* @fn unsigned int moteur_ncurses::menu( const char titre[], const char *choix[], unsigned int nb_choix )
* @brief Créé une sous fenêtre centré.
* @param[in] titre		Titre de la sous fenêtre
* @param[in] choix		Liste des choix a mettre dans la sous fenêtre. ( Tableau de chaine de caractères )
* @param[in] nb_choix	Nombre d'élement dans la liste des choix
* @return Le choix selectionné (sous forme d'un nombre: le ième element de la liste a été selectionné)
*
* La sous fenêtre propose de naviguer parmit une liste ( @a choix )<br />
* Puis renvoie le choix (sous forme d'un nombre: ième element de la liste)<br />
* @code
* // Exemple
* const char* choix[] = {
*	"Choix 1",
*	"Choix 2",
*	"Choix 3",
*	"Choix 4",
*	"Choix 5"
* };
* unsigned int choix_selectionner = menu( "Liste de mes choix", choix, 5 );
* @endcode
*/
unsigned int moteur_ncurses::menu( const char titre[], const char *choix[], unsigned int nb_choix )
{
	int	key=0;
	WINDOW* win_menu = 0;
	unsigned int	titre_taille = strlen(titre),
					highLight=1,
					i;// i : parcours des choix

	// On affiche le super cadre :D
	cadre();

	win_menu = newwin(5+nb_choix/*HEIGHT*/, 30/*WIDTH*/, (getmaxy(stdscr)-5-nb_choix)/2, (getmaxx(stdscr)-30)/2);
	keypad(win_menu, TRUE); /* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	/***************************************************************************
	* Boucle EVENT
	*/
	do{
		//wclear(win_menu);

		// Création d'une boite
		box(win_menu, 0, 0);

		// Affichage du titre
		mvwaddstr(win_menu, 1, (getmaxx(win_menu)-titre_taille)/2, titre);
		/************************************************************************
		* Affichage du menu
		*/
		for( i=0; i<nb_choix; i++ )
		{
			if( highLight == i+1 ){ /* High light the present choice */
				wattron(win_menu, A_REVERSE);
				mvwaddstr(win_menu, i+3, 2, choix[i]);
				wattroff(win_menu, A_REVERSE);
			}else{
				mvwaddstr(win_menu, i+3, 2, choix[i]);
			}
		}
		wrefresh(win_menu);


		/************************************************************************
		* EVENT
		*/
		key = wgetch(win_menu);
		switch( key )
		{
			case KEY_UP:
				if( highLight == 1 )
					highLight = nb_choix;
				else
					--highLight;
				break;

			case KEY_DOWN:
				if( highLight == nb_choix )
					highLight = 1;
				else
					++highLight;
				break;

			default:
				break;
		}
	}while( key != KEY_ENTER_bis && key != KEY_ESCAP );

	if( key == KEY_ESCAP )
		highLight = nb_choix;

	delwin(win_menu);

	return highLight;
}


/***************************************************************************//*!
* @fn void moteur_ncurses::afficherConfigurationClavier( unsigned char joueur )
* @brief Menu permettant d'afficher et de modifier la configuration du clavier d'un joueur
* @param[in] joueur		Le numéro du joueur ( de 1 à ... )
*/
void moteur_ncurses::afficherConfigurationClavier( unsigned char joueur )
{
	int	key=0;
	char titre[] = "Configuration du clavier du joueur x";
	unsigned int	titre_taille = strlen(titre),
					highLight=1;
	unsigned char	i;// i : parcours des choix


	WINDOW* win_menu = 0;

	if( joueur == 0 )
		stdErrorE("void moteur_ncurses::afficherConfigurationClavier( 0 ) INTERDIT !");

	// On récupère la config du joueur
	clavier* cl = options::getInstance()->clavierJoueur( joueur-1 );

	const char* texte[] = {						// ATTENTION ! Si ajout / suppresion d'un menu,
		"Haut :					",				// NE PAS OUBLIER de modifier { nbLiens } !
		"Bas :						",			// RESPECTER L'ORDRE DEFINIE DANS t_touche ( clavier.h )
		"Droite :					",
		"Gauche :					",
		"Lancer une bombe :				",
		"Detonateur :					"
	};
	unsigned char nbLiens = 6;

	// Modification du titre ( joueur x => joueur 1 )
	titre[titre_taille-1] = '0'+joueur;

	// Si pas de clavier retourné => stop la fonction
	if( !cl )
		stdErrorVarE("Erreur lors de la tentative d'affichage de la configuration du clavier du joueur:: afficherConfigurationClavier(%u)", (unsigned int)joueur);

	// On affiche le super cadre :D
	cadre();

	win_menu = newwin(12/*HEIGHT*/, 50/*WIDTH*/, (getmaxy(stdscr)-12)/2, (getmaxx(stdscr)-50)/2);
	keypad(win_menu, TRUE); /* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	/***************************************************************************
	* Boucle EVENT
	*/
	do{
		//wclear(win_menu);

		// Création d'une boite
		box(win_menu, 0, 0);

		// Affichage du titre
		mvwaddstr(win_menu, 1, (getmaxx(win_menu)-titre_taille)/2, titre);
		/************************************************************************
		* Affichage du menu
		*/
		for( i=0; i<nbLiens; i++ )
		{
			if( highLight == (unsigned int)i+1 ){ /* High light the present choice */
				wattron(win_menu, A_REVERSE);
				mvwaddstr(win_menu, 3+i, 2, texte[i]);
				affichageTouche(win_menu, 3+i, 24, cl->touche((clavier::t_touche)(clavier::haut+i)));
				wattroff(win_menu, A_REVERSE);
			}else{
				mvwaddstr(win_menu, 3+i, 2, texte[i]);
				affichageTouche(win_menu, 3+i, 24, cl->touche((clavier::t_touche)(clavier::haut+i)));
			}
		}

		// Lien de retour
		if( highLight == (unsigned int)i+1 ){ /* High light the present choice */
			wattron(win_menu, A_REVERSE);
			mvwaddstr(win_menu, 3+i, 2, "Retour");
			wattroff(win_menu, A_REVERSE);
		}else{
			mvwaddstr(win_menu, 3+i, 2, "Retour");
		}

		wrefresh(win_menu);



		/************************************************************************
		* EVENT
		*/
		key = wgetch(win_menu);
		switch( key )
		{
			case KEY_UP:
				if( highLight == 1 )
					highLight = nbLiens+1;
				else
					--highLight;
				break;

			case KEY_DOWN:
				if( highLight == (unsigned int)nbLiens+1 )
					highLight = 1;
				else
					++highLight;
				break;

			// Touche Entrer pressée
			case KEY_ENTER_bis:{
				if( highLight <= (unsigned int)nbLiens ){// On a selectionné un lien de modification de touche
					cleanline(win_menu, 2);
					wattron(win_menu, COLOR_PAIR(MU_RED_BLACK));
					mvwaddstr(win_menu, 2, (getmaxx(win_menu)-34)/2, "Donnez la touche pour cette action");
					wattroff(win_menu, COLOR_PAIR(MU_RED_BLACK));
					wrefresh(win_menu);

					key = wgetch(win_menu);// On récup la nouvelle touche
					// Nétoyage de l'écran
					//wclear(win_menu);
					// ReCréation d'une boite
					box(win_menu, 0, 0);

					if( key != KEY_ESCAP ){// Toutes les touches sont accèpté SAUF ECHAP !
						cl->defTouche((clavier::t_touche)(clavier::haut+highLight-1), key);
						options::getInstance()->enregistrerConfig();
					}else{// Touche ECHAP pressée
						key = 0;// pas envie d'exit completement
					}
				}else{
					// On a selectionné "Retour"
					// Donc on quit.
					key = KEY_ESCAP;
				}
				break;
			}

			default:
				break;
		}
	}while( key != KEY_ESCAP );

	delwin(win_menu);
}


/***************************************************************************//*!
* @fn void moteur_ncurses::affichageTouche( WINDOW* win, int y, int x, int key )
* @brief Affiche une touche. Si nécessaire, un affichage spécifique est fait.
*
* Exemple d'affichage necessaire la touche KEY_UP ( flèche du haut )
*/
void moteur_ncurses::affichageTouche( WINDOW* win, int y, int x, int key )
{
	const int touches[] = {
		KEY_ENTER_bis,
		KEY_DC,
		KEY_END
	};
	unsigned char nbTouches = 3;
	const char* affichage[] = {
		"{ENTRER}",
		"{Suppr}",
		"{Fin}"
	};

	// Affichage spécial pour les flèches directionnelles
	switch( key )
	{
		case KEY_UP:
			mvwaddch(win, y, x, ACS_UARROW);
			mvwaddstr(win, y, x+1, "(Fleche directionnelle)");
			return ;

		case KEY_DOWN:
			mvwaddch(win, y, x, ACS_DARROW);
			mvwaddstr(win, y, x+1, "(Fleche directionnelle)");
			return ;

		case KEY_LEFT:
			mvwaddch(win, y, x, ACS_LARROW);
			mvwaddstr(win, y, x+1, "(Fleche directionnelle)");
			return ;

		case KEY_RIGHT:
			mvwaddch(win, y, x, ACS_RARROW);
			mvwaddstr(win, y, x+1, "(Fleche directionnelle)");
			return ;

		default:
			break;
	}

	// On parcours les touches non affichables
	for( unsigned char i=0; i<nbTouches; i++ )
	{
		if( touches[i] == key ){
			mvwaddstr(win, y, x, affichage[i]);
			return ;
		}
	}

	if( '!' <= key && key <= '~' ){// Caracètres affichables en ASCII ( non étendu )
		mvwaddch(win, y, x, key);
		return ;
	}

	// On a rien trouvé dans la base de donnée
	mvwaddstr(win, y, x, "{? INCONNU ?}");
}


/***************************************************************************//*!
* @fn int moteur_ncurses::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* returnValue )
* @brief Créer un menu pour récupérer un nombre entré par un utilisateur
* @param[in] titre			 Le titre du menu
* @param[in] valeurParDefaut La valeur par défaut
* @param[in] valeurMin		 La valeur minimum
* @param[in] valeurMax		 La valeur maximum
* @param[out] returnValue	 Dans cette variable sera stocké, le nombre obtenu a la fin de la fonction
* @return
*	- 2 : Nombre validé et accèpté
*	- 3 : Action annulée
*/
int moteur_ncurses::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* returnValue )
{
	int	key=0;
	unsigned int	titre_taille = strlen(titre),
					highLight=2;
	WINDOW* win_menu = 0;

	*returnValue = valeurParDefaut;

	// On affiche le super cadre :D
	cadre();

	win_menu = newwin(8/*HEIGHT*/, 35/*WIDTH*/, (getmaxy(stdscr)-8)/2, (getmaxx(stdscr)-30)/2);
	keypad(win_menu, TRUE); /* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	/***************************************************************************
	* Boucle EVENT
	*/
	do{
		//wclear(win_menu);

		// Création d'une boite
		box(win_menu, 0, 0);

		// Affichage du titre
		mvwaddstr(win_menu, 1, (getmaxx(win_menu)-titre_taille)/2, titre);
		/************************************************************************
		* Affichage du menu
		*/
		cleanline(win_menu, 3);
		// Le nombre en cours de modif
		if( highLight == 1 ){ /* High light the present choice */
			wattron(win_menu, A_REVERSE);
			mvwprintw(win_menu, 3, (getmaxx(win_menu)-8-getTailleNombre(*returnValue))/2, "<-- %d -->", *returnValue);
			wattroff(win_menu, A_REVERSE);
		}else{
			mvwprintw(win_menu, 3, (getmaxx(win_menu)-8-getTailleNombre(*returnValue))/2, "<-- %d -->", *returnValue);
		}
		// Lien de pour valider le choix
		if( highLight == 2 ){ /* High light the present choice */
			wattron(win_menu, A_REVERSE);
			mvwaddstr(win_menu, 4, 2, "Ok");
			wattroff(win_menu, A_REVERSE);
		}else{
			mvwaddstr(win_menu, 4, 2, "Ok");
		}
		// Lien de retour
		if( highLight == 3 ){ /* High light the present choice */
			wattron(win_menu, A_REVERSE);
			mvwaddstr(win_menu, 5, 2, "Retour");
			wattroff(win_menu, A_REVERSE);
		}else{
			mvwaddstr(win_menu, 5, 2, "Retour");
		}

		wrefresh(win_menu);


		/************************************************************************
		* EVENT
		*/
		key = wgetch(win_menu);
		switch( key )
		{
			case KEY_UP:
				if( highLight == 1 )
					highLight = 3;
				else
					--highLight;
				break;

			case KEY_DOWN:
				if( highLight == 3 )
					highLight = 1;
				else
					++highLight;
				break;

			case KEY_RIGHT:
				if( *returnValue+1 > valeurMax )
					*returnValue = valeurMin;
				else
					(*returnValue)++;
				break;

			case KEY_LEFT:
				if( *returnValue-1 < valeurMin )
					*returnValue = valeurMax;
				else
					(*returnValue)--;
				break;

			// Touche Entrer pressée
			case KEY_ENTER_bis:{
				if( highLight == 1 ){// On a selectionné un lien de modification de texte
					do{
						wattron(win_menu, COLOR_PAIR(MU_RED_BLACK));
						mvwaddstr(win_menu, 2, (getmaxx(win_menu)-29)/2, "Entrez le nombre manuellement");
						wattroff(win_menu, COLOR_PAIR(MU_RED_BLACK));

						cleanline(win_menu, 3);
						mvwprintw(win_menu, 3, (getmaxx(win_menu)-8-getTailleNombre(*returnValue))/2, "<-- %d -->", *returnValue);
						wrefresh(win_menu);

						key = wgetch(win_menu);// On récup la nouvelle touche

						switch( key )
						{
							case KEY_RIGHT:
							case KEY_UP:{
								if( *returnValue+1 > valeurMax )
									*returnValue = valeurMin;
								else
									(*returnValue)++;
								break;
							}
							case KEY_LEFT:
							case KEY_DOWN:{
								if( *returnValue-1 < valeurMin )
									*returnValue = valeurMax;
								else
									(*returnValue)--;
								break;
							}
							case KEY_BACKSPACE:
							case KEY_BACKSPACE_bis:{
								*returnValue /= 10;
								break;
							}
							case KEY_ESCAP:{
								*returnValue = valeurParDefaut;
								break;
							}
							case KEY_ENTER_bis:{
								if( !(valeurMin <= *returnValue && *returnValue <= valeurMax) ){
									key = 0;
									*returnValue = valeurMin;
								}
								break;
							}
							#if defined(WIN32) || defined(WIN64)// Windows gère différament le '+' et le '-' s'ils sont sur le pad ou pas...
							case PADPLUS:
							case PADMINUS:
							#endif
							case '+':
							case '-':{
								if( valeurMin <= *returnValue*(-1) && *returnValue*(-1) <= valeurMax )
									*returnValue *= -1;
								break;
							}
							default:{
								// Ajout de chiffre à la main
								if( '0' <= key && key <= '9' && *returnValue*10+(key-'0') <= valeurMax )
									*returnValue = *returnValue*10+(key-'0');
								break;
							}
						}
					}while( key != KEY_ESCAP && key != KEY_ENTER_bis );
					cleanline(win_menu, 2);
					key = 0;// Pas d'exit du menu accidentel
				}
				if( highLight == 3 )// On a selectionné "Retour"
					key = KEY_ESCAP;// Donc on quit.

				break;
			}

			case KEY_ESCAP:
				highLight = 3;
				break;

			default:
				break;
		}
	}while( key != KEY_ESCAP && !(key == KEY_ENTER_bis && highLight == 2) );

	delwin(win_menu);

	return highLight;
}


/***************************************************************************//*!
* @fn int moteur_ncurses::getTexte( const char titre[], char texteRetour[21] )
* @brief Permet d'obtenir du texte
* @param[in] titre				Le titre du menu
* @param[out] texteRetour		Dans cette variable sera stocké, le texte obtenu a la fin de la fonction
* @return
*	- 2 : Texte validé et accèpté
*	- 3 : Action annulée
*/
int moteur_ncurses::getTexte( const char titre[], char texteRetour[21] )
{
	int	key=0;
	unsigned int	titre_taille = strlen(titre),
					highLight=1;
	WINDOW* win_menu = 0;
	bool erreurTexteVide = 0;

	// On affiche le super cadre :D
	cadre();

	win_menu = newwin(8/*HEIGHT*/, 40/*WIDTH*/, (getmaxy(stdscr)-8)/2, (getmaxx(stdscr)-40)/2);
	keypad(win_menu, TRUE); /* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	/***************************************************************************
	* Boucle EVENT
	*/
	do{
		//wclear(win_menu);

		// Création d'une boite
		box(win_menu, 0, 0);

		// Affichage du titre
		mvwaddstr(win_menu, 1, (getmaxx(win_menu)-titre_taille)/2, titre);

		// Affichage erreur
		if( erreurTexteVide ){
			wattron(win_menu, COLOR_PAIR(MU_RED_BLACK));
			mvwaddstr(win_menu, 2, (getmaxx(win_menu)-35)/2, "Le texte dois avoir 1 caractere min");
			wattroff(win_menu, COLOR_PAIR(MU_RED_BLACK));
		}

		/************************************************************************
		* Affichage du menu
		*/
		cleanline(win_menu, 3);
		// Le texte en cours de modification
		if( highLight == 1 ){ /* High light the present choice */
			wattron(win_menu, A_REVERSE);
			cleanline(win_menu, 3);
			mvwaddch(win_menu, 3, 2, ACS_RARROW);
			mvwprintw(win_menu, 3, (getmaxx(win_menu)-strlen(texteRetour))/2, "%s", texteRetour);
			wattroff(win_menu, A_REVERSE);
		}else{
			cleanline(win_menu, 3);
			mvwaddch(win_menu, 3, 2, ACS_RARROW);
			mvwprintw(win_menu, 3, (getmaxx(win_menu)-strlen(texteRetour))/2, "%s", texteRetour);
		}
		// Lien de pour valider le choix
		if( highLight == 2 ){ /* High light the present choice */
			wattron(win_menu, A_REVERSE);
			mvwaddstr(win_menu, 4, 2, "Ok");
			wattroff(win_menu, A_REVERSE);
		}else{
			mvwaddstr(win_menu, 4, 2, "Ok");
		}
		// Lien de retour
		if( highLight == 3 ){ /* High light the present choice */
			wattron(win_menu, A_REVERSE);
			mvwaddstr(win_menu, 5, 2, "Retour");
			wattroff(win_menu, A_REVERSE);
		}else{
			mvwaddstr(win_menu, 5, 2, "Retour");
		}

		wrefresh(win_menu);


		/************************************************************************
		* EVENT
		*/
		key = wgetch(win_menu);
		switch( key )
		{
			case KEY_UP:
				if( highLight == 1 )
					highLight = 3;
				else
					--highLight;
				break;

			case KEY_DOWN:
				if( highLight == 3 )
					highLight = 1;
				else
					++highLight;
				break;

			// Touche Entrer pressée
			case KEY_ENTER_bis:{
				if( highLight == 1 ){// On a selectionné un lien de modification de texte
					do{
						cleanline(win_menu, 2);
						wattron(win_menu, COLOR_PAIR(MU_RED_BLACK));
						mvwaddstr(win_menu, 2, (getmaxx(win_menu)-18)/2, "Entrez votre texte");
						wattroff(win_menu, COLOR_PAIR(MU_RED_BLACK));

						wattron(win_menu, A_REVERSE);
						cleanline(win_menu, 3);
						mvwprintw(win_menu, 3, (getmaxx(win_menu)-strlen(texteRetour))/2, "%s", texteRetour);
						wattroff(win_menu, A_REVERSE);
						wrefresh(win_menu);

						key = wgetch(win_menu);// On récup la nouvelle touche

						switch( key )
						{
							case KEY_BACKSPACE:
							case KEY_BACKSPACE_bis:{
								if( strlen(texteRetour) > 0 )
									texteRetour[strlen(texteRetour)-1] = 0;
								break;
							}
							case KEY_ENTER_bis:{
								trimString(texteRetour);
								erreurTexteVide = 0;
								break;
							}
							default:{
								// Ajout d'une lettre à la main
								if( ' ' <= key && key <= '~' && strlen(texteRetour)+1 < 20 )
									texteRetour[strlen(texteRetour)+1] = 0;
									texteRetour[strlen(texteRetour)] = key;
								break;
							}
						}
					}while( key != KEY_ESCAP && key != KEY_ENTER_bis );
					cleanline(win_menu, 2);
					key = 0;// Pas d'exit du menu accidentel
				}
				if( highLight == 2 && strlen(trimString(texteRetour)) < 1 ){
					// IL FAUT AU MOINS 1 CARACTRERES
					erreurTexteVide = 1;
					key = 0;
				}
				if( highLight == 3 )// On a selectionné "Retour"
					key = KEY_ESCAP;// Donc on quit.

				break;
			}

			case KEY_ESCAP:
				highLight = 3;
				break;

			default:
				break;
		}
	}while( key != KEY_ESCAP && !(key == KEY_ENTER_bis && highLight == 2) );

	delwin(win_menu);

	return highLight;
}


/***************************************************************************//*!
* @fn void moteur_ncurses::cleanline( WINDOW* win, int y, int x_begin, int x_end )
* @brief Permet d'effacer une ligne
* @fn void moteur_ncurses::cleanline( WINDOW* win, int y );
* @brief Alias de cleanline( win, y, 1, getmaxx(win)-2 );
*/
void moteur_ncurses::cleanline( WINDOW* win, int y, int x_begin, int x_end )
{
	if( x_begin > x_end )
		stdErrorVarE("Taille incorrect x_begin(=%d) > x_end(=%d)", x_begin, x_end);

	for( int x=x_begin; x<=x_end; x++ )
		mvwaddch(win, y, x, ' ');
}


/***************************************************************************//*!
* @fn unsigned char moteur_ncurses::getTailleNombre( int nb )
* @brief Calcule la longueur d'un nombre.
* @param[in] nb Le nombre dont on veut savoir la taille
* @return Renvoie le nombre de chiffre qui compose le nombre
*
* Exemple getTailleNombre(-10) => 3<br />
*/
unsigned char moteur_ncurses::getTailleNombre( int nb )
{
	if( nb < 0 )// On met le nombre en positif si ce n'est pas le cas
		return getTailleNombre( (unsigned int) (nb*(-1)) )+1;// +1 pour le symbole négatif

	return getTailleNombre( (unsigned int) nb );
}


/***************************************************************************//*!
* @fn unsigned char moteur_ncurses::getTailleNombre( unsigned int nb )
* @brief Calcule la longueur d'un nombre.
* @param[in] nb Le nombre dont on veut savoir la taille
* @return Renvoie le nombre de chiffre qui compose le nombre
*
* Exemple getTailleNombre(122) => 3
*/
unsigned char moteur_ncurses::getTailleNombre( unsigned int nb )
{
	unsigned char taille_NB;
	for( taille_NB=1; nb>=10; taille_NB++ )
		nb /=10;

	return taille_NB;
}


/***************************************************************************//*!
* @fn char* moteur_ncurses::trimString( char texte[] )
* @brief Vire les espaces avant et après une chaine de caractère
* @param[in,out] texte Le texte a trim. Le texte est directement modifié
* @return Renvoie le pointeur @e texte
*
* Exemple : "  toto " => "toto"
*/
char* moteur_ncurses::trimString( char texte[] )
{
	unsigned int	posDebut=0,
					posFin=strlen(texte)-2;// -2 car -1 => '\0' et 0 => out du tableau

	// On remplis tous les espaces qui sont en fin de chaine de '\0'
	while( isspace(*(texte+posFin)) && texte[posFin] && posFin>0 )
	{
		texte[posFin] = 0;
		posFin--;
	}

	// On trouve la 1ère lettre qui n'est pas un espace
	while( isspace(*(texte+posDebut)) && texte[posDebut] && ++posDebut>0 );

	// On déplace les caractères
	return strcpy(texte, texte+posDebut);
}


/***************************************************************************//*!
* @fn chtype moteur_ncurses::getCouleurJoueur( unsigned char joueur )
* @brief Renvoie la couleur d'un joueur
* @param[in] Numéro du joueur de 1 à ...
* @return Renvoie la couleur à utiliser ( couleur directement utilisable dans wattron() )
*
* Exemple:
* @code
* wattron(win, getCouleurJoueur( 1 ));
* @endcode
*
* @note Si le numéro de joueur n'a pas de correspondance, alors :
* - un message d'erreur ( sur le cannal d'erreur ) est envoyé
* - La couleur blanche sera utilisée
*/
chtype moteur_ncurses::getCouleurJoueur( unsigned char joueur )
{
	switch( joueur )
	{
		case 1:
			return COLOR_PAIR(MU_JOUEUR1);
		case 2:
			return COLOR_PAIR(MU_JOUEUR2);
		case 3:
			return COLOR_PAIR(MU_JOUEUR3);
		case 4:
			return COLOR_PAIR(MU_JOUEUR4);
		default:{
			stdErrorVar("Problème d'id joueur ! id_joueur=%d", joueur);
			return COLOR_PAIR(MU_WHITE_BLACK);
		}
	}
	return COLOR_PAIR(MU_JOUEUR4);
}


/***************************************************************************//*!
* @fn SYS_CLAVIER moteur_ncurses::afficherMapEtEvent( const partie* p )
* @brief Affiche une map
* @param[in] p	La partie en cours a afficher
* @return La touche actuellement appuyé
*/
SYS_CLAVIER moteur_ncurses::afficherMapEtEvent( const partie* p )
{
	map* l_map = p->getMap();// l_map pour local variable map
	// Décalage
	unsigned int	xpos=(getmaxx(stdscr)-l_map->X())/2,
					ypos=(getmaxy(stdscr)-l_map->Y())/2;
	// Fenetre d'affichage
	WINDOW* win = stdscr;

	if( (unsigned int)getmaxx(stdscr) < l_map->X() || (unsigned int)getmaxy(stdscr) < l_map->Y() )
		stdErrorVarE("La map est trop grande pour l'affichage ! SCREEN:(Xmax=%d, Ymax=%d), MAP:(Xmax=%u, Ymax=%u)", getmaxx(stdscr), getmaxy(stdscr), l_map->X(), l_map->Y());

	//halfdelay( 2 );
	//notimeout( c_win, true );
	//wtimeout( c_win, 500 );
	//nodelay( stdscr, true );

	attron(COLOR_PAIR(MU_YELLOW_BLACK));
	mvwaddstr(stdscr, 1, (getmaxx(stdscr)-9)/2, "Bomberman");// -9 pour la taille de "Bomberman"
	attroff(COLOR_PAIR(MU_YELLOW_BLACK));
	box(stdscr, 0, 0);
	refresh(); /* Print it on to the real screen */

	// Sert à afficher les couleurs pour les joueurs
	chtype couleur;

	for( unsigned int x,y=0; y<l_map->Y(); y++ )
	{
		for( x=0; x<l_map->X(); x++ )
		{
			switch( l_map->getBlock(x,y)->element )
			{
				case map::vide: {
					mvwaddch( win, ypos+y, xpos+x, ' ');
					break;
				}
				case map::Mur_destrucible: {
					mvwaddch( win, ypos+y, xpos+x, '%');
					break;
				}
				case map::Mur_INdestrucible: {
					mvwaddch( win, ypos+y, xpos+x, '#');
					break;
				}
				case map::flamme: {
					mvwaddch( win, ypos+y, xpos+x, '*');
					break;
				}
				case map::UN_joueur:
				case map::plusieurs_joueurs: {
					if( !l_map->getBlock(x,y)->joueur )
						stdErrorVarE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y)->joueur=0", x, y);

					couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
					wattron(win, couleur);
					mvwaddch( win, ypos+y, xpos+x, '@');
					wattroff(win, couleur);

					break;
				}
				case map::bombe_poser: {
					couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
					wattron(win, couleur);
					mvwaddch( win, ypos+y, xpos+x, '¤');
					wattroff(win, couleur);
					break;
				}
				case map::bombe_poser_AVEC_UN_joueur: {
					if( !l_map->getBlock(x,y)->joueur )
						stdErrorVarE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y).joueur=0", x, y);

					couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
					wattron(win, couleur);
					mvwaddch( win, ypos+y, xpos+x, 'à');
					wattroff(win, couleur);

					break;
				}
				default:{
					stdErrorVar("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(x,y)->element);
					break;
				}
			}
		}
	}
	refresh(); /* Print it on to the real screen */
	wrefresh(win);

	return wgetch(win);
}
