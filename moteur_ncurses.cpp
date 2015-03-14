#include "moteur_ncurses.h"
#include "debug.h"

bool moteur_ncurses::c_premierAffichage = 1;

/***************************************************************************//*!
* @fn void moteur_ncurses::forcerRafraichissement()
* @brief Permet de forcer un affichage complet
*/


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

	// Permet de changer la taille du terminal
	//resize_term(50, 100);

	clear();		/* Efface l'écran */
	noecho();		/* Lorsqu'une touche est préssée au clavier, elle n'apparait pas à  l'écran */
	cbreak();		/* Un caractère est directement affiché, il ne passe pas dans un buffer */
					/* In the cbreak() mode these control characters are interpreted as any other character by the terminal driver. */
	/*raw();*/		/* In the raw() mode these characters are directly passed to the program without generating a signal. */
	keypad(stdscr, TRUE);		/* Pour que les flèches, F1, ... soient traitées (il faut le faire après création de la fenêtre) */
	nodelay( stdscr, true );// Pas de délait d'attente

	if( !has_colors() ){
		this->~moteur_ncurses();
		stdErrorE("Votre terminal ne supporte pas les couleurs ! STOP ALL...");
	}
	start_color();

	// On éclaircit les couleurs si on peut
	if( can_change_color() ){
		/* int init_color(COLOR, r, g, b);
		 * param 1			: color name
		 * param 2, 3, 4	: rgb content min = 0, max = 1000
		 */
		init_color(COLOR_RED, 950, 0, 0);
		init_color(COLOR_YELLOW, 950, 950, 0);
		init_color(COLOR_BLUE, 0, 0, 950);
	}

	init_pair(MU_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
	init_pair(MU_RED_BLACK, COLOR_RED, COLOR_BLACK);
	init_pair(MU_BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
	init_pair(MU_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
	init_pair(MU_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(MU_BLACK_YELLOW, COLOR_BLACK, COLOR_YELLOW);
	init_pair(MU_BLACK_MAGENTA, COLOR_BLACK, COLOR_MAGENTA);

	curs_set(0);// Cache le cursor tout moche

	#if defined(WIN32) || defined(WIN64)
		PDC_set_title("Bomberman");
	#endif
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
	}while( key != KEY_ENTER_bis && key != PADENTER && key != KEY_ESCAP );

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
		stdErrorE("Erreur lors de la tentative d'affichage de la configuration du clavier du joueur:: afficherConfigurationClavier(%u)", (unsigned int)joueur);

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
			case PADENTER:
			case KEY_ENTER_bis: {
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

	if( valeurMin > valeurParDefaut || valeurParDefaut > valeurMax ){
		stdError("Valeur incorrect ! valeurMin(%d) <= valeurParDefaut(%d) && valeurParDefaut(%d) <= valeurMax(%d)", valeurMin, valeurParDefaut, valeurParDefaut, valeurMax);
		valeurParDefaut = (valeurMax - valeurMin)/2+valeurMin;
	}

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
			case PADENTER:
			case KEY_ENTER_bis: {
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
							case KEY_UP: {
								if( *returnValue+1 > valeurMax )
									*returnValue = valeurMin;
								else
									(*returnValue)++;
								break;
							}
							case KEY_LEFT:
							case KEY_DOWN: {
								if( *returnValue-1 < valeurMin )
									*returnValue = valeurMax;
								else
									(*returnValue)--;
								break;
							}
							case KEY_BACKSPACE:
							case KEY_BACKSPACE_bis: {
								*returnValue /= 10;
								break;
							}
							case KEY_ESCAP: {
								*returnValue = valeurParDefaut;
								break;
							}
							case PADENTER:
							case KEY_ENTER_bis: {
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
							case '-': {
								if( valeurMin <= *returnValue*(-1) && *returnValue*(-1) <= valeurMax )
									*returnValue *= -1;
								break;
							}
							default: {
								// Ajout de chiffre à la main
								if( '0' <= key && key <= '9' && *returnValue*10+(key-'0') <= valeurMax )
									*returnValue = *returnValue*10+(key-'0');
								break;
							}
						}
					}while( key != KEY_ESCAP && key != KEY_ENTER_bis && key != PADENTER );
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
	}while( key != KEY_ESCAP && !((key == KEY_ENTER_bis || key == PADENTER) && highLight == 2) );

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
			case PADENTER:
			case KEY_ENTER_bis: {
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
							case KEY_BACKSPACE_bis: {
								if( strlen(texteRetour) > 0 )
									texteRetour[strlen(texteRetour)-1] = 0;
								break;
							}
							case PADENTER:
							case KEY_ENTER_bis: {
								trimString(texteRetour);
								erreurTexteVide = 0;
								break;
							}
							default: {
								// Ajout d'une lettre à la main
								if( ' ' <= key && key <= '~' && strlen(texteRetour)+1 < 20 ){// NOTE: 21 = '\0'
									texteRetour[strlen(texteRetour)+1] = 0;
									texteRetour[strlen(texteRetour)] = key;
								}
								break;
							}
						}
					}while( key != KEY_ESCAP && key != KEY_ENTER_bis && key != PADENTER );
					cleanline(win_menu, 2);
					if( strlen(trimString(texteRetour)) ){// IL FAUT AU MOINS 1 CARACTRERES
						highLight = 2;// On déplace sur le ok le cursor
					}
					key = 0;// Pas d'exit du menu accidentel
				}else{
					if( highLight == 2 ){
						if( strlen(trimString(texteRetour)) < 1 ){
							// IL FAUT AU MOINS 1 CARACTRERES
							erreurTexteVide = 1;
							key = 0;
						}
					}else{// highLight == 3 )// On a selectionné "Retour"
						key = KEY_ESCAP;// Donc on quit.
					}
				}
				break;
			}

			case KEY_ESCAP:
				highLight = 3;
				break;

			default:
				break;
		}
	}while( key != KEY_ESCAP && !((key == KEY_ENTER_bis || key == PADENTER) && highLight == 2) );

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
		stdErrorE("Taille incorrect x_begin(=%d) > x_end(=%d)", x_begin, x_end);

	for( int x=x_begin; x<=x_end; x++ )
		mvwaddch(win, y, x, ' ');
}


/***************************************************************************//*!
* @fn chtype moteur_ncurses::getCouleurJoueur( unsigned char joueur )
* @brief Renvoie la couleur d'un joueur
* @param[in] joueur Numéro du joueur de 1 à ...
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
		default: {
			stdError("Problème d'id joueur ! id_joueur=%d", joueur);
			return COLOR_PAIR(MU_WHITE_BLACK);
		}
	}
	return COLOR_PAIR(MU_JOUEUR4);
}


/***************************************************************************//*!
* @fn SYS_CLAVIER moteur_ncurses::afficherMapEtEvent( partie* p )
* @brief Affiche une map
* @param[in] p	La partie en cours a afficher
* @return La touche actuellement appuyé
*/
SYS_CLAVIER moteur_ncurses::afficherMapEtEvent( partie* p )
{
	map* l_map = p->getMap();// l_map pour local variable map
	// Décalage
	unsigned int	xpos=(getmaxx(stdscr)-l_map->X())/2,
					ypos=(getmaxy(stdscr)-l_map->Y())/2;
	// Fenetre d'affichage
	WINDOW* win = stdscr;

	// Sert à afficher les couleurs pour les joueurs
	chtype couleur;

	if( (unsigned int)getmaxx(stdscr) < l_map->X() || (unsigned int)getmaxy(stdscr) < l_map->Y() )
		stdErrorE("La map est trop grande pour l'affichage ! SCREEN:(Xmax=%d, Ymax=%d), MAP:(Xmax=%u, Ymax=%u)", getmaxx(stdscr), getmaxy(stdscr), l_map->X(), l_map->Y());

	//halfdelay( 2 );
	//notimeout( c_win, true );
	//wtimeout( c_win, 500 );
	//nodelay( stdscr, true );

	if( c_premierAffichage ){
		attron(COLOR_PAIR(MU_YELLOW_BLACK));
		mvwaddstr(stdscr, 1, (getmaxx(stdscr)-9)/2, "Bomberman");// -9 pour la taille de "Bomberman"
		attroff(COLOR_PAIR(MU_YELLOW_BLACK));
		box(stdscr, 0, 0);
		refresh(); /* Print it on to the real screen */

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
					case map::Mur_destructible: {
						mvwaddch( win, ypos+y, xpos+x, '%');
						break;
					}
					case map::Mur_INdestructible: {
						mvwaddch( win, ypos+y, xpos+x, '#');
						break;
					}
					case map::UN_joueur:
					case map::plusieurs_joueurs: {
						if( !l_map->getBlock(x,y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y)->joueur=0", x, y);

						couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
						wattron(win, couleur);
						mvwaddch( win, ypos+y, xpos+x, '@');
						wattroff(win, couleur);

						break;
					}
					case map::bombe_poser: {
						couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
						wattron(win, couleur);
						mvwaddch( win, ypos+y, xpos+x, '!');
						wattroff(win, couleur);
						break;
					}
					case map::bombe_poser_AVEC_UN_joueur:
					case map::bombe_poser_AVEC_plusieurs_joueurs: {
						if( !l_map->getBlock(x,y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y).joueur=0", x, y);

						couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
						wattron(win, couleur);
						mvwaddch( win, ypos+y, xpos+x, '$');
						wattroff(win, couleur);

						break;
					}
					case map::flamme_origine: {
						wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
						mvwaddch( win, ypos+y, xpos+x, 'O');
						wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
						break;
					}
					case map::flamme_horizontal:
					case map::flamme_vertical: {
						wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
						mvwaddch( win, ypos+y, xpos+x, 'X');
						wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
						break;
					}
					case map::flamme_pointe_haut: {
						wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
						mvwaddch( win, ypos+y, xpos+x, '^');
						wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
						break;
					}
					case map::flamme_pointe_bas: {
						wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
						mvwaddch( win, ypos+y, xpos+x, 'v');
						wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
						break;
					}
					case map::flamme_pointe_droite: {
						wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
						mvwaddch( win, ypos+y, xpos+x, '>');
						wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
						break;
					}
					case map::flamme_pointe_gauche: {
						wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
						mvwaddch( win, ypos+y, xpos+x, '<');
						wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
						break;
					}
					case map::bonus: {
						wattron(win, COLOR_PAIR(MU_BLACK_MAGENTA));
						if( !l_map->getBlock(x,y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y).joueur=0", x, y);
						switch( (bonus::t_Bonus)l_map->getBlock(x,y)->joueur->at(0) )
						{
							case bonus::bombe: {
								mvwaddch( win, ypos+y, xpos+x, '!');
								break;
							}
							case bonus::puissance_flamme: {
								mvwaddch( win, ypos+y, xpos+x, 'O');
								break;
							}
							case bonus::declancheur: {
								mvwaddch( win, ypos+y, xpos+x, 'D');
								break;
							}
							case bonus::pousse_bombe: {
								mvwaddch( win, ypos+y, xpos+x, 'P');
								break;
							}
							case bonus::vie: {
								mvwaddch( win, ypos+y, xpos+x, '@');
								break;
							}
							default: {
								stdError("Erreur lors de la lecture de la map, BONUS inconu <%d>", (int)l_map->getBlock(x,y)->joueur->at(0));
								break;
							}
						}
						//mvwaddch( win, ypos+y, xpos+x, '?');
						wattroff(win, COLOR_PAIR(MU_BLACK_MAGENTA));
						break;
					}
					default: {
						stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(x,y)->element);
						break;
					}
				}
			}
		}
		c_premierAffichage = false;
	}else{
		s_Coordonnees pos;
		while( l_map->getModifications(pos) )
		{
			switch( l_map->getBlock(pos)->element )
			{
				case map::vide: {
					mvwaddch( win, ypos+pos.y, xpos+pos.x, ' ');
					break;
				}
				case map::Mur_destructible: {
					mvwaddch( win, ypos+pos.y, xpos+pos.x, '%');
					break;
				}
				case map::Mur_INdestructible: {
					mvwaddch( win, ypos+pos.y, xpos+pos.x, '#');
					break;
				}
				case map::UN_joueur:
				case map::plusieurs_joueurs: {
					if( !l_map->getBlock(pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(pos)->joueur=0", pos.x, pos.y);

					couleur = getCouleurJoueur( l_map->getBlock(pos)->joueur->at(0) );
					wattron(win, couleur);
					mvwaddch( win, ypos+pos.y, xpos+pos.x, '@');
					wattroff(win, couleur);

					break;
				}
				case map::bombe_poser: {
					couleur = getCouleurJoueur( l_map->getBlock(pos)->joueur->at(0) );
					wattron(win, couleur);
					mvwaddch( win, ypos+pos.y, xpos+pos.x, '!');
					wattroff(win, couleur);
					break;
				}
				case map::bombe_poser_AVEC_UN_joueur:
				case map::bombe_poser_AVEC_plusieurs_joueurs: {
					if( !l_map->getBlock(pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(pos).joueur=0", pos.x, pos.y);

					couleur = getCouleurJoueur( l_map->getBlock(pos)->joueur->at(0) );
					wattron(win, couleur);
					mvwaddch( win, ypos+pos.y, xpos+pos.x, '$');
					wattroff(win, couleur);

					break;
				}
				case map::flamme_origine: {
					wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
					mvwaddch( win, ypos+pos.y, xpos+pos.x, 'O');
					wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
					break;
				}
				case map::flamme_horizontal:
				case map::flamme_vertical: {
					wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
					mvwaddch( win, ypos+pos.y, xpos+pos.x, 'X');
					wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
					break;
				}
				case map::flamme_pointe_haut: {
					wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
					mvwaddch( win, ypos+pos.y, xpos+pos.x, '^');
					wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
					break;
				}
				case map::flamme_pointe_bas: {
					wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
					mvwaddch( win, ypos+pos.y, xpos+pos.x, 'v');
					wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
					break;
				}
				case map::flamme_pointe_droite: {
					wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
					mvwaddch( win, ypos+pos.y, xpos+pos.x, '>');
					wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
					break;
				}
				case map::flamme_pointe_gauche: {
					wattron(win, COLOR_PAIR(MU_BLACK_YELLOW));
					mvwaddch( win, ypos+pos.y, xpos+pos.x, '<');
					wattroff(win, COLOR_PAIR(MU_BLACK_YELLOW));
					break;
				}
				case map::bonus: {
					wattron(win, COLOR_PAIR(MU_BLACK_MAGENTA));
					if( !l_map->getBlock(pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(pos).joueur=0", pos.x, pos.y);
					switch( (bonus::t_Bonus)l_map->getBlock(pos)->joueur->at(0) )
					{
						case bonus::bombe: {
							mvwaddch( win, ypos+pos.y, xpos+pos.x, '!');
							break;
						}
						case bonus::puissance_flamme: {
							mvwaddch( win, ypos+pos.y, xpos+pos.x, 'O');
							break;
						}
						case bonus::declancheur: {
							mvwaddch( win, ypos+pos.y, xpos+pos.x, 'D');
							break;
						}
						case bonus::pousse_bombe: {
							mvwaddch( win, ypos+pos.y, xpos+pos.x, 'P');
							break;
						}
						case bonus::vie: {
							mvwaddch( win, ypos+pos.y, xpos+pos.x, '@');
							break;
						}
						default: {
							stdError("Erreur lors de la lecture de la map, BONUS inconu <%d>", (int)l_map->getBlock(pos)->joueur->at(0));
							break;
						}
					}
					//mvwaddch( win, ypos+pos.y, xpos+pos.x, '?');
					wattroff(win, COLOR_PAIR(MU_BLACK_MAGENTA));
					break;
				}
				default: {
					stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(pos)->element);
					break;
				}
			}

		}
	}

	// Affichage du timer
	if( p->timeOut() > clock() )
		mvwprintw(win, 3, 2, "Temps restant %02dmin:%02dsecs", (p->timeOut()-clock())/(CLOCKS_PER_SEC*60), ((p->timeOut()-clock())%(CLOCKS_PER_SEC*60))/CLOCKS_PER_SEC);
	else
		mvwaddstr(win, 3, 2, "Temps restant 0min:0secs");


	// Joueur 1
	couleur = getCouleurJoueur( 1 );
	wattron(win, couleur);
	if( (int)xpos-26 < 0 )
		stdErrorE("Fenêtre trop petite pour afficher toutes les informations nécéssaire !");

	mvwprintw(win, ypos, xpos-26, "%s", p->joueur(0)->nom()->c_str());
	wattroff(win, couleur);
	if( p->joueur(0)->armements() && p->connection() != partie::CNX_Client ){
		mvwprintw(win, ypos+1, xpos-24, "Nombre de bombe: %d", p->joueur(0)->armements()->quantiteMAX(bonus::bombe));
		mvwprintw(win, ypos+2, xpos-24, "Puissance de flamme: %d", p->joueur(0)->armements()->quantiteMAX(bonus::puissance_flamme));
		mvwprintw(win, ypos+3, xpos-24, "Declancheur manuel: %d", p->joueur(0)->armements()->quantiteMAX(bonus::declancheur));
		mvwprintw(win, ypos+4, xpos-24, "Pousse bombe: %d", p->joueur(0)->armements()->quantiteMAX(bonus::pousse_bombe));
		mvwprintw(win, ypos+5, xpos-24, "Nombre de vie: %d", p->joueur(0)->armements()->quantiteMAX(bonus::vie));
	}


	static bool cleanPlayer2=0;
	// Joueur 2
	if( p->nbJoueurs() > 1 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(1)->isLocal() || ( !p->joueur(1)->isLocal() && p->joueur(1)->socket() != INVALID_SOCKET )))) ){
		couleur = getCouleurJoueur( 2 );
		wattron(win, couleur);
		mvwprintw(win, ypos+l_map->Y()-6, xpos+l_map->X()+2, "%s", p->joueur(1)->nom()->c_str());
		wattroff(win, couleur);
		if( p->joueur(1)->armements() ){
			mvwprintw(win, ypos+l_map->Y()-5, xpos+l_map->X()+4, "Nombre de bombe: %d", p->joueur(1)->armements()->quantiteMAX(bonus::bombe));
			mvwprintw(win, ypos+l_map->Y()-4, xpos+l_map->X()+4, "Puissance de flamme: %d", p->joueur(1)->armements()->quantiteMAX(bonus::puissance_flamme));
			mvwprintw(win, ypos+l_map->Y()-3, xpos+l_map->X()+4, "Declancheur manuel: %d", p->joueur(1)->armements()->quantiteMAX(bonus::declancheur));
			mvwprintw(win, ypos+l_map->Y()-2, xpos+l_map->X()+4, "Pousse bombe: %d", p->joueur(1)->armements()->quantiteMAX(bonus::pousse_bombe));
			mvwprintw(win, ypos+l_map->Y()-1, xpos+l_map->X()+4, "Nombre de vie: %d", p->joueur(1)->armements()->quantiteMAX(bonus::vie));
		}
		cleanPlayer2 = 1;
	}else{
		if( cleanPlayer2 ){
			cleanline(win, ypos+l_map->Y()-6, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+l_map->Y()-5, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+l_map->Y()-4, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+l_map->Y()-3, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+l_map->Y()-2, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+l_map->Y()-1, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanPlayer2 = 0;
		}
	}

	static bool cleanPlayer3=0;
	// Joueur 3
	if( p->nbJoueurs() > 2 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(2)->isLocal() || ( !p->joueur(2)->isLocal() && p->joueur(2)->socket() != INVALID_SOCKET )))) ){
		couleur = getCouleurJoueur( 3 );
		wattron(win, couleur);
		mvwprintw(win, ypos, xpos+l_map->X()+2, "%s", p->joueur(2)->nom()->c_str());
		wattroff(win, couleur);
		if( p->joueur(2)->armements() ){
			mvwprintw(win, ypos+1, xpos+l_map->X()+4, "Nombre de bombe: %d", p->joueur(2)->armements()->quantiteMAX(bonus::bombe));
			mvwprintw(win, ypos+2, xpos+l_map->X()+4, "Puissance de flamme: %d", p->joueur(2)->armements()->quantiteMAX(bonus::puissance_flamme));
			mvwprintw(win, ypos+3, xpos+l_map->X()+4, "Declancheur manuel: %d", p->joueur(2)->armements()->quantiteMAX(bonus::declancheur));
			mvwprintw(win, ypos+4, xpos+l_map->X()+4, "Pousse bombe: %d", p->joueur(2)->armements()->quantiteMAX(bonus::pousse_bombe));
			mvwprintw(win, ypos+5, xpos+l_map->X()+4, "Nombre de vie: %d", p->joueur(2)->armements()->quantiteMAX(bonus::vie));
		}
		cleanPlayer3 = 1;
	}else{
		if( cleanPlayer3 ){
			cleanline(win, ypos, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+1, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+2, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+3, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+4, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanline(win, ypos+5, xpos+l_map->X()+2, getmaxx(win)-2 );
			cleanPlayer3 = 0;
		}
	}

	static bool cleanPlayer4=0;
	// Joueur 4
	if( p->nbJoueurs() == 4 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(3)->isLocal() || ( !p->joueur(3)->isLocal() && p->joueur(3)->socket() != INVALID_SOCKET )))) ){
		couleur = getCouleurJoueur( 4 );
		wattron(win, couleur);
		mvwprintw(win, ypos+l_map->Y()-6, xpos-26, "%s", p->joueur(3)->nom()->c_str());
		wattroff(win, couleur);
		if( p->joueur(3)->armements() ){
			mvwprintw(win, ypos+l_map->Y()-5, xpos-24, "Nombre de bombe: %d", p->joueur(3)->armements()->quantiteMAX(bonus::bombe));
			mvwprintw(win, ypos+l_map->Y()-4, xpos-24, "Puissance de flamme: %d", p->joueur(3)->armements()->quantiteMAX(bonus::puissance_flamme));
			mvwprintw(win, ypos+l_map->Y()-3, xpos-24, "Declancheur manuel: %d", p->joueur(3)->armements()->quantiteMAX(bonus::declancheur));
			mvwprintw(win, ypos+l_map->Y()-2, xpos-24, "Pousse bombe: %d", p->joueur(3)->armements()->quantiteMAX(bonus::pousse_bombe));
			mvwprintw(win, ypos+l_map->Y()-1, xpos-24, "Nombre de vie: %d", p->joueur(3)->armements()->quantiteMAX(bonus::vie));
		}
		cleanPlayer4 = 1;
	}else{
		if( cleanPlayer4 ){
			cleanline(win, ypos+l_map->Y()-6, xpos-26, 26 );
			cleanline(win, ypos+l_map->Y()-5, xpos-26, 26 );
			cleanline(win, ypos+l_map->Y()-4, xpos-26, 26 );
			cleanline(win, ypos+l_map->Y()-3, xpos-26, 26 );
			cleanline(win, ypos+l_map->Y()-2, xpos-26, 26 );
			cleanline(win, ypos+l_map->Y()-1, xpos-26, 26 );
			cleanPlayer4 = 0;
		}
	}



	//refresh(); /* Print it on to the real screen */
	wrefresh(win);

	return wgetch(win);
}
