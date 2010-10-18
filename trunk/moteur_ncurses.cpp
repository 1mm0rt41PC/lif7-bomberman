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

	// init_color(COLOR_RED, 700, 0, 0);
	/* param 1     : color name
	 * param 2, 3, 4 : rgb content min = 0, max = 1000 */

	curs_set(0);// Cache le cursor tout moche
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
	endwin();/* End curses mode */
}


/*******************************************************************************
*
*/
void moteur_ncurses::cadre()
{
	clear();// Efface l'écran

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

	attron(COLOR_PAIR(MU_YELLOW_BLACK));
	mvprintw(WINDOW_HEIGHT/2 -2, WINDOW_WIDTH-4, "Bomberman");
	attroff(COLOR_PAIR(MU_YELLOW_BLACK));

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
		"Clavier Joueur 1",							// NE PAS OUBLIER de modifier le for !
		"Clavier Joueur 2",
		"Clavier Joueur 3",
		"Clavier Joueur 4",
		"Retour"
	};
	unsigned int entry=0;
	do{
		entry = menu( "", menu_main, 3 );

		/***********************************************************************
		* Menu d'accueil
		*/
		switch(entry){
			case 1:{// Jouer

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
										getKey( "Configuration du clavier du J1", "Haut" );
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

	//int choice_size = sizeof(choix) / sizeof(char *);// <- Permet de savoir le nombre d'element
	WINDOW *win_menu = 0;

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
		mvwprintw(win_menu, 1, WINDOW_WIDTH/2-strlen(titre)/2, "%s", titre);
		/************************************************************************
		* Affichage du menu
		*/
		for(i=0; i<(int)nb_choix; i++)
		{
			if(highLight == i+1){ /* High light the present choice */
				wattron(win_menu, A_REVERSE);
				//mvwprintw(WIN, Y, X, "%s", choices[i]);
				mvwprintw(win_menu, i+3, 2, "%s", choix[i]);
				wattroff(win_menu, A_REVERSE);
			}else{
				mvwprintw(win_menu, i+3, 2, "%s", choix[i]);
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
* Système de définition d'une touche de clavier
*/
int moteur_ncurses::getKey( char titre[], char touche[] )
{
	int	key=0;
	WINDOW *win_menu=0;

	// On affiche le super cadre :D
	cadre();

	win_menu = newwin( WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT/2, WINDOW_WIDTH/2 );
	keypad( win_menu, TRUE ); /* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */

	// Création d'une boite
	box(win_menu, 0, 0);

	/***************************************************************************
	* Boucle EVENT
	*/
	do{
		// Affichage du titre
		mvwprintw(win_menu, 1, WINDOW_WIDTH/2-strlen(titre)/2, "%s", titre);
		// Affichage du titre touche
		mvwprintw(win_menu, 3, WINDOW_WIDTH/2-strlen(touche)/2, "%s", touche);
		// Efface la touche précédante
		mvwprintw(win_menu, 5, WINDOW_WIDTH/2-5, "                        ");
		// Affichage de la touche
		switch(key){
			case KEY_UP:{
					mvwaddstr(win_menu, 5, (WINDOW_WIDTH+10)/2-strlen("{FLECHE HAUT}"), "{FLECHE HAUT}");
				break;
			}
			case KEY_DOWN:{
					mvwaddstr(win_menu, 5, WINDOW_WIDTH/2-strlen("{FLECHE BAS}"), "{FLECHE BAS}");
				break;
			}
			case KEY_LEFT:{
					mvwaddstr(win_menu, 5, WINDOW_WIDTH/2-strlen("{FLECHE GAUCHE}"), "{FLECHE GAUCHE}");
				break;
			}
			case KEY_RIGHT:{
					mvwaddstr(win_menu, 5, WINDOW_WIDTH/2-strlen("{FLECHE DROITE}"), "{FLECHE DROITE}");
				break;
			}
			default:{
				mvwprintw(win_menu, 5, WINDOW_WIDTH/2-1, "%c", key);
				break;
			}
		}
		wrefresh(win_menu);

		/************************************************************************
		* EVENT
		*/
		key = wgetch(win_menu);
	}while(key != KEY_ESCAP);

	delwin(win_menu);
	endwin();

	return key;
}



/*******************************************************************************
*
*/
void moteur_ncurses::ncursBcl()
{
	int key=0;

	do
	{
		//ncursAff( win, pJeu);

		key = getch();
		/* jeuActionClavier( jeu, 'd'); */
		switch(key)
		{
			case KEY_LEFT:
				//jeuActionClavier( pJeu, 'g');
				break;
			case KEY_RIGHT:
				//jeuActionClavier( pJeu, 'd');
				break;
			case KEY_UP:
				//jeuActionClavier( pJeu, 'h');
				break;
			case KEY_DOWN:
				//jeuActionClavier( pJeu, 'b');
				break;
		}
	} while (key != 27);/* ECHAP */

	endwin();
}
