#include "moteur_ncurses.h"

//subwin(stdscr,y,x,begin_y,begin_x);

/*******************************************************************************
*
*/
moteur_ncurses::moteur_ncurses()
{

	stdscr = initscr();		/* Passe l'écran texte en mode NCurses */
	clear();		/* Efface l'écran */
	noecho();		/* Lorsqu'une touche est préssée au clavier, elle n'apparait pas à  l'écran */
	cbreak();		/* Un caractère est directement affiché, il ne passe pas dans un buffer */
					/* In the cbreak() mode these control characters are interpreted as any other character by the terminal driver. */
	/*raw();*/		/* In the raw() mode these characters are directly passed to the program without generating a signal. */
	/* Creation d'une nouvelle fenetre en mode texte */
	/* => fenetre de dimension et position ( HEIGHT, WIDTH, STARTY, STARTX) */
//	c_win = newwin( WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT/2, WINDOW_WIDTH/2 );
	keypad( stdscr, TRUE );		/* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	if(has_colors() == FALSE)
	{
		endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
	start_color();
	init_pair(MU_YELLOW_BLACK, COLOR_LIGHT_YELLOW, COLOR_BLACK);
	init_pair(MU_RED_BLACK, COLOR_LIGHT_RED, COLOR_BLACK);

	// init_color(COLOR_RED, 700, 0, 0);
	/* param 1     : color name
	 * param 2, 3, 4 : rgb content min = 0, max = 1000 */

	curs_set(0);// Cache le cursor tout moche

	// Permet de changer la taille du terminal
	resize_term(50, 100);

	PDC_set_title("Bomberman");

/*
	halfdelay( 2 );
	notimeout( c_win, true );
	wtimeout( c_win, 500 );
	nodelay( c_win, true );
*/
}


/*******************************************************************************
*
*/
moteur_ncurses::~moteur_ncurses()
{
	curs_set(1);// <- j'aime mon curseur dans mon term !!!
	clear();
	refresh(); /* Print it on to the real screen */
	delwin(stdscr);
	endwin();/* End curses mode */
}


/*******************************************************************************
*
*/
void moteur_ncurses::cadre()
{
	clear();// Efface l'écran
/*
	// Bordure du haut
	for(int i=0; i<=WINDOW_WIDTH*2; i++)
		mvprintw(0, i, "*");

	// Bordure du bas
	for(int i=0; i<=WINDOW_WIDTH*2; i++)
		mvprintw(WINDOW_HEIGHT*2, i, "*");

	// Bordure de droite
	for(int i=0; i<=WINDOW_HEIGHT*2; i++)
		mvprintw(i, WINDOW_WIDTH*2, "*");

	// Bordure de gauche
	for(int i=0; i<=WINDOW_HEIGHT*2; i++)
		mvprintw(i, 0, "*");
*/

	attron(COLOR_PAIR(MU_YELLOW_BLACK));
	mvprintw(WINDOW_HEIGHT/2 -2, WINDOW_WIDTH-4, "Bomberman");
	attroff(COLOR_PAIR(MU_YELLOW_BLACK));

	box(stdscr, 0, 0);

	refresh(); /* Print it on to the real screen */
}


/*******************************************************************************
* Menus généraux
*/
void moteur_ncurses::main()
{
	char *menu_main[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Jouer",			// NE PAS OUBLIER de modifier le for !
		"Options",
		"Quitter"
	};
	char *menu_options[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Port",					// NE PAS OUBLIER de modifier le for !
		"Clavier",
		"Retour"
	};
	char *menu_options_claviers[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Clavier Joueur 1",				// NE PAS OUBLIER de modifier le for !
		"Clavier Joueur 2",
		"Clavier Joueur 3",
		"Clavier Joueur 4",
		"Retour"
	};
	char *menu_jeu[] = {	// ATTENTION ! Si ajout / suppresion d'un menu,
		"Offline",			// NE PAS OUBLIER de modifier le for !
		"Online",
		"Retour"
	};

	unsigned int entry=0;
	do{
		entry = menu( "", menu_main, 3 );

		/***********************************************************************
		* Menu d'accueil
		*/
		switch(entry){
			/*******************************************************************
			* Jouer :: Type de partie
			*/
			case 1:{
				do{
					entry = menu( "Type de partie", menu_jeu, 3 );
					switch(entry){
						case 1:{// Offline

						}
						case 2:{// Offline
							break;
						}
						/*******************************************************
						* Retour au menu précédant
						*/
						case 3:{// Retour
							break;
						}
					}
				}while(entry != 3);
				entry = 0;
				break;
			}
			/*******************************************************************
			* Menu des options
			*/
			case 2:{// Options
				do{
					entry = menu( "Options", menu_options, 3 );
					switch(entry){
						/*******************************************************
						* Menu des options
						*/
						case 1:{// Port
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
								entry = menu( "Options des Claviers", menu_options_claviers, 5 );
								switch(entry){
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
							}while(entry != 5);
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
				}while(entry != 3);
				entry=0;
				break;
			}

			case 3:{// Quiter
				break;
			}
		}
	}while(entry != 3);
	//entry = menu( menu_options, 3 );
}


/*******************************************************************************
* Système d'affichage de menu
*/
unsigned int moteur_ncurses::menu( char titre[], char *choix[], unsigned int nb_choix )
{
	int	key=0, highLight=1, i;// i : parcours des choix
	WINDOW *win_menu = 0;
	unsigned int titre_taille = strlen(titre);

	// On affiche le super cadre :D
	cadre();

	win_menu = newwin( 5+nb_choix, WINDOW_WIDTH, WINDOW_HEIGHT/2, WINDOW_WIDTH/2 );
	keypad( win_menu, TRUE ); /* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	// Création d'une boite
	box(win_menu, 0, 0);

	/***************************************************************************
	* Boucle EVENT
	*/
	do{
		// Affichage du titre
		mvwaddstr(win_menu, 1, (getmaxx(win_menu)-titre_taille)/2, titre);
		/************************************************************************
		* Affichage du menu
		*/
		for(i=0; i<(int)nb_choix; i++)
		{
			if(highLight == i+1){ /* High light the present choice */
				wattron(win_menu, A_REVERSE);
				//mvwaddstr(WIN, Y, X, choices[i]);
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
		switch(key)
		{
			case KEY_UP:
				if(highLight == 1)
					highLight = nb_choix;
				else
					--highLight;
				break;

			case KEY_DOWN:
				if(highLight == (int)nb_choix)
					highLight = 1;
				else
					++highLight;
				break;

			default:
				break;
		}
	}while(key != KEY_ENTER_bis && key != KEY_ESCAP);

	if(key == KEY_ESCAP)
		highLight = nb_choix;

	delwin(win_menu);
	endwin();

	return highLight;
}


/*******************************************************************************
* Page permettant d'afficher et de modifier la configuration du clavier
* d'un joueur
*/
void moteur_ncurses::afficherConfigurationClavier( unsigned char joueur )
{
	int	key=0, highLight=1, i;// i : parcours des choix
	char titre[] = "Configuration du clavier du joueur x";
	unsigned int titre_taille = strlen(titre);

	WINDOW *win_menu = 0;

	// On récupère la config du joueur
	clavier* cl = G_OPTIONS.clavierJoueur( joueur-1 );

	char *texte[] = {						// ATTENTION ! Si ajout / suppresion d'un menu,
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
	if(!cl){
		perror("Erreur lors de la tentative d'affichage de la configuration du clavier du joueur:: afficherConfigurationClavier( ");
		perror(titre+titre_taille-2);// On envoie la fin du titre soit x => 1
		perror(" )");
		return ;
	}

	// On affiche le super cadre :D
	cadre();

	win_menu = newwin( 12, WINDOW_WIDTH+20, WINDOW_HEIGHT/2, WINDOW_WIDTH/2 );
	keypad( win_menu, TRUE ); /* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	// Création d'une boite
	box(win_menu, 0, 0);

	/***************************************************************************
	* Boucle EVENT
	*/
	do{
		// Affichage du titre
		mvwaddstr(win_menu, 1, (getmaxx(win_menu)-titre_taille)/2, titre);
		/************************************************************************
		* Affichage du menu
		*/
		for(i=0; i<nbLiens; i++)
		{
			if(highLight == i+1){ /* High light the present choice */
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
		if(highLight == i+1){ /* High light the present choice */
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
		switch(key)
		{
			case KEY_UP:
				if(highLight == 1)
					highLight = nbLiens+1;
				else
					--highLight;
				break;

			case KEY_DOWN:
				if(highLight == nbLiens+1)
					highLight = 1;
				else
					++highLight;
				break;

			// Touche Entrer pressée
			case KEY_ENTER_bis:
				if(highLight <= nbLiens){// On a selectionné un lien de modification de touche
					wattron(win_menu, COLOR_PAIR(MU_RED_BLACK));
					mvwaddstr(win_menu, 2, (getmaxx(win_menu)-34)/2, "Donnez la touche pour cette action");
					wattroff(win_menu, COLOR_PAIR(MU_RED_BLACK));
					wrefresh(win_menu);

					key = wgetch(win_menu);// On récup la nouvelle touche
					// Nétoyage de l'écran
					wclear(win_menu);
					// ReCréation d'une boite
					box(win_menu, 0, 0);

					if( key != KEY_ESCAP ){// Toutes les touches sont accèpté SAUF ECHAP !
						cl->defTouche((clavier::t_touche)(clavier::haut+highLight-1), key);
						G_OPTIONS.enregistrerConfig();
					}else{// Touche ECHAP pressée
						key = 0;// pas envie d'exit completement
					}
				}else{
					// On a selectionné "Retour"
					// Donc on quit.
					key = KEY_ESCAP;
				}
				break;

			default:
				break;
		}
	}while( key != KEY_ESCAP );

	delwin(win_menu);
	endwin();
}


/*******************************************************************************
* Système d'affichage des touches
*/
void moteur_ncurses::affichageTouche( WINDOW *win, int y, int x, int key )
{
	int touches[] = {
		KEY_ENTER_bis,
		KEY_DC,
		KEY_END
	};
	unsigned char nbTouches = 3;
	char *affichage[] = {
		"{ENTRER}",
		"{Suppr}",
		"{Fin}"
	};

	// Affichage spécial pour les flèches directionnelles
	switch(key){
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
	for( int i=0; i<nbTouches; i++ ){
		if( touches[i] == key ){
			mvwaddstr(win, y, x, affichage[i]);
			return ;
		}
	}

	if('!' <= key && key <= '~')// Caracètres affichables en ASCII ( non étendu )
	{
		mvwaddch(win, y, x, key);
		return ;
	}

	// On a rien trouvé dans la base de donnée
	mvwaddstr(win, y, x, "{? INCONNU ?}");
}


int moteur_ncurses::obtenirNombre( char titre[], int valeurParDefaut, int ValeurMin, int ValeurMax )
{


}

