#include "moteur_sdl.h"
#include "debug.h"

moteur_sdl* moteur_sdl::c_Instance = 0;

/***************************************************************************//*!
* @fn moteur_sdl::moteur_sdl()
* @brief Initialise la class moteur_sdl
*
* Initialise l'écran sdl et prépare le "terrain" pour les traitements futurs
*/
moteur_sdl::moteur_sdl()
{
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 ) // Démarrage de la SDL. Si erreur alors...
		stdErrorE("Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Ecriture de l'erreur

	if(TTF_Init() == -1)
		stdErrorE("Erreur d'initialisation de TTF_Init : %s", TTF_GetError());

	c_ecranGeneral = SDL_SetVideoMode(1024,768, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_FillRect(c_ecranGeneral, NULL, SDL_MapRGB(c_ecranGeneral->format, 255, 255, 255));/* Efface l'écran */

	//SDL_ShowCursor(SDL_DISABLE);// Masque le curseur de la souris

	SDL_WM_SetCaption("Bomberman", NULL);// Titre de la fenêtre

	if( !(c_policeGeneral = TTF_OpenFont("Chicken Butt.ttf", 75)) )
		stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

	c_background = chargerImage("images/background.png");

	SDL_EnableUNICODE(1);// On ne veut pas un clavier QWERTY

	SDL_EnableKeyRepeat(0, 0);// Pas de répétition des touches

	/***************************************************************************
	* On charge ici le décor
	*/
	c_Decor = new SDL_Surface*[__nombre_de_decors__];
	// Initialisation du décor
	for( unsigned int i=0; i<__nombre_de_decors__; i++ )
		c_Decor[i] = 0;

	//murs
	c_Decor[vide] = chargerImage("images/vide.png");
	c_Decor[mur_destructible] = chargerImage("images/mur_destructible.png");
	c_Decor[mur_indestructible] = chargerImage("images/mur_indestructible.png");
	//joueur1
	c_Decor[joueur1_haut] = chargerImage("images/bomberman1_haut.png");
	c_Decor[joueur1_bas] = chargerImage("images/bomberman1_bas.png");
	c_Decor[joueur1_gauche] = chargerImage("images/bomberman1_gauche.png");
	c_Decor[joueur1_droite] = chargerImage("images/bomberman1_droite.png");
	//joueur2
	c_Decor[joueur2_haut] = chargerImage("images/bomberman2_haut.png");
	c_Decor[joueur2_bas] = chargerImage("images/bomberman2_bas.png");
	c_Decor[joueur2_gauche] = chargerImage("images/bomberman2_gauche.png");
	c_Decor[joueur2_droite] = chargerImage("images/bomberman2_droite.png");
	//joueur3
	c_Decor[joueur3_haut] = chargerImage("images/bomberman3_haut.png");
	c_Decor[joueur3_bas] = chargerImage("images/bomberman3_bas.png");
	c_Decor[joueur3_gauche] = chargerImage("images/bomberman3_gauche.png");
	c_Decor[joueur3_droite] = chargerImage("images/bomberman3_droite.png");
	//joueur4
	c_Decor[joueur4_haut] = chargerImage("images/bomberman4_haut.png");
	c_Decor[joueur4_bas] = chargerImage("images/bomberman4_bas.png");
	c_Decor[joueur4_gauche] = chargerImage("images/bomberman4_gauche.png");
	c_Decor[joueur4_droite] = chargerImage("images/bomberman4_droite.png");
	//armes
	c_Decor[flamme_origine] = chargerImage("images/flamme_origine.png");
	c_Decor[flamme_vertical] = chargerImage("images/milieu_flamme_verticale.png");
	c_Decor[flamme_horizontal] = chargerImage("images/milieu_flamme_horizontale.png");
	c_Decor[flamme_pointe_haut] = chargerImage("images/flamme_haut.png");
	c_Decor[bout_flamme_bas] = chargerImage("images/flamme_bas.png");
	c_Decor[bout_flamme_gauche] = chargerImage("images/flamme_gauche.png");
	c_Decor[bout_flamme_droite] = chargerImage("images/flamme_droite.png");
	c_Decor[bombe] = chargerImage("images/bombeSprite.png");
	//c_Decor[bombe] = chargerImage("images/bombe.gif");
	// Bonus
	c_Decor[gain_bombe] = chargerImage("images/gain_bombe.png");
	c_Decor[gain_declancheur] = chargerImage("images/detonateur.png");
	c_Decor[gain_puissance_flamme] = chargerImage("images/gain_puissance_flamme.png");
	c_Decor[gain_pousse_bombe] = chargerImage("images/kick.png");
	c_Decor[gain_vie] = chargerImage("images/gain_vie.gif");

	c_fenetreOuverte = true;
}


/***************************************************************************//*!
* @fn moteur_sdl& moteur_sdl::getInstance()
* @brief Renvoie l'instance de la class. Si pas d'instance, alors création d'une instance
*/
moteur_sdl& moteur_sdl::getInstance()
{
	if( !c_Instance )
		c_Instance = new moteur_sdl;

	return *c_Instance;
}


/***************************************************************************//*!
* @fn moteur_sdl::~moteur_sdl()
* @brief Désinitialise la class moteur_sdl
*/
moteur_sdl::~moteur_sdl()
{
	for( unsigned int i=0; i<__nombre_de_decors__; i++ )
	{
		if(c_Decor[i])
			SDL_FreeSurface(c_Decor[i]);
	}
	delete[] c_Decor;

	if( c_background )
		SDL_FreeSurface(c_background);// On libère le background

	if( c_policeGeneral )
		TTF_CloseFont(c_policeGeneral);/* Fermeture de la police avant TTF_Quit */

	TTF_Quit();/* Arrêt de SDL_ttf (peut être avant ou après SDL_Quit, peu importe) */
	SDL_Quit();
}


/***************************************************************************//*!
* @fn unsigned int moteur_sdl::menu( const char titre[], const char *choix[], unsigned int nb_choix )
* @brief Afficher l'image correspondante suivant le menu où on est
* @param[in] titre		Titre de la sous fenêtre
* @param[in] choix		Liste des choix a mettre dans la sous fenêtre. ( Tableau de chaine de caractères )
* @param[in] nb_choix	Nombre d'élement dans la liste des choix
* @return Le choix selectionné (sous forme d'un nombre: le ième element de la liste a été selectionné)
*
* La sous fenêtre propose de naviguer parmis une liste ( @a choix )<br />
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
unsigned int moteur_sdl::menu( const char titre[], const char *choix[], unsigned int nb_choix )
{
	unsigned int highLight = 1;
	bool continuer = 1;
	SDL_Event event;
	SDL_Color couleurOrange = {255, 184, 0, 0/*unused*/};
	SDL_Rect position;
	SDL_Surface* sfr_titre=0;
	SDL_Surface** textes={0};
	unsigned int i;
	bool dessiner = 1;

	textes = new SDL_Surface*[nb_choix*2];

	for( i=0; i<nb_choix; i++ )/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
		textes[i] = ecritTexte(choix[i]);

	// Création du texte HighLight
	for( i=nb_choix; i<nb_choix*2; i++ )/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
		textes[i] = ecritTexte(choix[i-nb_choix], couleurOrange);

	// Création du titre
	if( titre && strlen(titre) ){
		TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_UNDERLINE);
		sfr_titre = ecritTexte(titre);
		TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_NORMAL);
	}

	while( continuer )
	{
		SDL_WaitEvent(&event);

		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = nb_choix;// On veux quitter !
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: {
				switch( event.key.keysym.sym )
				{
					case SDLK_UP: { /* Appui sur la touche Echap, on arrête le programme */
						if( highLight == 1 ){// On bouge notre highLight vers le haut
							highLight = nb_choix;
						}else{
							highLight--;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_DOWN: {// On bouge notre highLight vers le bas
						if( highLight == nb_choix ){
							highLight = 1;
						}else{
							highLight++;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_RETURN: {// On a valider notre choix par entrer
						continuer = 0;
						break;
					}
					case SDLK_ESCAPE: { /* Appui sur la touche Echap, on arrête le programme */
						highLight = nb_choix;// On veux quitter !
						continuer = 0;
						break;
					}
					default:
						break;
				}
				break;
			}
			/*******************************************************************
			* Prise en charge de la souris
			*/
			case SDL_MOUSEBUTTONUP: {
				position.y = 200;
				for( i=0; i<nb_choix; i++ )
				{
					position.y += 100;// Espacement entre les textes ( blit )
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					if( position.x <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
						if( highLight == i+1 ){
							continuer = 0;
						}else{
							highLight = i+1;
							dessiner = 1;
						}
					}
				}
				break;
			}
			default:
				break;
		}

		/***********************************************************************
		* Blitage général
		* > On blit uniquement s'il y a eu une modification
		*/
		if( dessiner ){
			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

			position.y = 200;// Point de départ
			if( sfr_titre ){
				position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
				SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background
			}
			// On blit le texte
			for( i=0; i<nb_choix; i++ )
			{
				position.y += 100;// Espacement entre les textes ( blit )
				position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
				if( i == highLight-1 ){
					SDL_BlitSurface(textes[i+nb_choix], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}else{
					SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}
	}

	for( i=0; i<nb_choix*2; i++ )// On libère les textes
		SDL_FreeSurface(textes[i]);
	delete[] textes;// On libère les textes

	if( sfr_titre )
		SDL_FreeSurface(sfr_titre);
	return highLight;
}


/***************************************************************************//*!
* @fn void moteur_sdl::afficherConfigurationClavier( unsigned char joueur )
* @brief Menu permettant d'afficher et de modifier la configuration du clavier d'un joueur
* @param[in] joueur		Le numéro du joueur ( de 1 à ... )
*/
void moteur_sdl::afficherConfigurationClavier( unsigned char joueur )
{
	bool continuer = 1;
	SDL_Event event;
	unsigned int highLight = 1;
	bool dessiner = 1;
	int i;
	SDL_Rect position, position_droite; /* position de blittage du texte */
	// On récupère la config du joueur
	clavier* cl = options::getInstance()->clavierJoueur( joueur-1 );

	// POUR def une touche :
	// cl->defTouche((clavier::t_touche)(clavier::haut+highLight-1), key);
	// POUR récup une touche dans la configuration
	// cl->touche(  clavier::haut );

	SDL_Color couleurOrange = {255, 184, 0, 0/*unused*/};
	SDL_Color couleurRouge = {255, 0, 0, 0/*unused*/};

	// Création titre
	SDL_Surface* sfr_titre;
	char tempTexte[] = "Configuration clavier du joueur 0";// Ne pas oublier le '\0' !
	tempTexte[strlen(tempTexte)-1] += joueur;
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_UNDERLINE);
	sfr_titre = ecritTexte(tempTexte);
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_NORMAL);

	// Création du texte de retour
	SDL_Surface* sfr_retour[2];
	sfr_retour[0] = ecritTexte("Retour");
	sfr_retour[1] = ecritTexte("Retour", couleurOrange);

	// Ecriture des textes
	SDL_Surface* sfr_msg; /* pour dialoguer avec l'utilisateur */
	SDL_Surface* sfr_HAUT[2];
	SDL_Surface* sfr_HAUT_touche[2];
	SDL_Surface* sfr_BAS[2];
	SDL_Surface* sfr_BAS_touche[2];
	SDL_Surface* sfr_DROITE[2];
	SDL_Surface* sfr_DROITE_touche[2];
	SDL_Surface* sfr_GAUCHE[2];
	SDL_Surface* sfr_GAUCHE_touche[2];
	SDL_Surface* sfr_LANCER_BOMBE[2];
	SDL_Surface* sfr_LANCER_BOMBE_touche[2];
	SDL_Surface* sfr_DETONATEUR[2];
	SDL_Surface* sfr_DETONATEUR_touche[2];

	sfr_HAUT[0] = ecritTexte("Haut");
	sfr_HAUT[1] = ecritTexte("Haut", couleurOrange);
	sfr_HAUT_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
	sfr_HAUT_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
	sfr_BAS[0] = ecritTexte("Bas");
	sfr_BAS[1] = ecritTexte("Bas", couleurOrange);
	sfr_BAS_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ));
	sfr_BAS_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ), couleurOrange);
	sfr_DROITE[0] = ecritTexte("Droite");
	sfr_DROITE[1] = ecritTexte("Droite", couleurOrange);
	sfr_DROITE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ));
	sfr_DROITE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ), couleurOrange);
	sfr_GAUCHE[0] = ecritTexte("Gauche");
	sfr_GAUCHE[1] = ecritTexte("Gauche", couleurOrange);
	sfr_GAUCHE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ));
	sfr_GAUCHE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ), couleurOrange);
	sfr_LANCER_BOMBE[0] = ecritTexte("Lancer bombe");
	sfr_LANCER_BOMBE[1] = ecritTexte("Lancer bombe", couleurOrange);
	sfr_LANCER_BOMBE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ));
	sfr_LANCER_BOMBE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ), couleurOrange);
	sfr_DETONATEUR[0] = ecritTexte("Détonateur");
	sfr_DETONATEUR[1] = ecritTexte("Détonateur", couleurOrange);
	sfr_DETONATEUR_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ));
	sfr_DETONATEUR_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ), couleurOrange);

	do{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: { /*	si on clique sur la croix rouge de la fenêtre -> on sort et highLight */
				highLight = 7; /*	passe à 7 (valeur de sortie) */
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: { /* si touche appuyée */
				switch( traductionClavier(&event.key) ) /* on traduit la touche */
				{
					case SDLK_ESCAPE: { /*	si on appuit sur la touche echap */
						highLight = 7; /*	passe à 7 (valeur de sortie) */
						continuer = 0;
						break;
					}
					case SDLK_UP: { /* flèche du haut */
						if( highLight == 1 ){ /* si on est tout en haut, alors on va tout en bas */
							highLight = 7;
						}else{ /* sinon highLight --*/
							highLight--;
						}
						dessiner = 1; /* mouvement donc on redessine */
						break;
					}
					case SDLK_DOWN: { /* flèche bas */
						if( highLight == 7 ){ /* si on est tout en bas, alors on va tout en haut */
							highLight = 1;
						}else{ /* sinon highLight ++ */
							highLight++;
						}
						dessiner = 1; /* mouvement donc on redessine */
						break;
					}
					case SDLK_RETURN: { /* touche entrer appuyée */
						if( highLight == 7 ){ /* entrer + dernière ligne -> sortie */
							highLight = 7;
							continuer = 0;
							break;
						}

						switch( highLight )
						{
							/* liens modifiables : de 1 à 6 */
							case 1:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour HAUT", couleurRouge, 50);
								break;
							case 2:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour BAS", couleurRouge, 50);
								break;
							case 3:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour DROITE", couleurRouge, 50);
								break;
							case 4:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour GAUCHE", couleurRouge, 50);
								break;
							case 5:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour la BOMBE", couleurRouge, 50);
								break;
							case 6:
								sfr_msg = ecritTexte("Appuyer sur la nouvelle touche pour le DETONATEUR", couleurRouge, 50);
								break;
						}

						position.x = 0;
						position.y = 0;
						SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);/* blittage du background */
						position.x = (c_ecranGeneral->w-sfr_msg->w)/2;
						position.y = (c_ecranGeneral->h-sfr_msg->h)/2;
						SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position); /* blittage du texte */
						SDL_Flip(c_ecranGeneral); /* raffraichissement */
						do{
							SDL_WaitEvent(&event); /* en attente d'un évènement */
							if( event.type == SDL_QUIT ){ /* si croix rouge -> on sort */
								continuer = 0;
								highLight = 7;
								c_fenetreOuverte = false;
								break;
							}
							/* sinon lire la touche et l'enregistrer */
							if( event.type == SDL_KEYDOWN && traductionClavier(&event.key) != SDLK_ESCAPE ){
								/* redéfinition de la touche : */
								cl->defTouche((clavier::t_touche)(clavier::haut+highLight-1), traductionClavier(&event.key));
								switch( highLight )
								{
									/* liens modifiables : de 1 à 6 */
									case 1:
										SDL_FreeSurface(sfr_HAUT_touche[0]);
										SDL_FreeSurface(sfr_HAUT_touche[1]);
										sfr_HAUT_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ));
										sfr_HAUT_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::haut) ), couleurOrange);
										break;
									case 2:
										SDL_FreeSurface(sfr_BAS_touche[0]);
										SDL_FreeSurface(sfr_BAS_touche[1]);
										sfr_BAS_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ));
										sfr_BAS_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::bas) ), couleurOrange);
										break;
									case 3:
										SDL_FreeSurface(sfr_DROITE_touche[0]);
										SDL_FreeSurface(sfr_DROITE_touche[1]);
										sfr_DROITE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ));
										sfr_DROITE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::droite) ), couleurOrange);
										break;
									case 4:
										SDL_FreeSurface(sfr_GAUCHE_touche[0]);
										SDL_FreeSurface(sfr_GAUCHE_touche[1]);
										sfr_GAUCHE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ));
										sfr_GAUCHE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::gauche) ), couleurOrange);
										break;
									case 5:
										SDL_FreeSurface(sfr_LANCER_BOMBE_touche[0]);
										SDL_FreeSurface(sfr_LANCER_BOMBE_touche[1]);
										sfr_LANCER_BOMBE_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ));
										sfr_LANCER_BOMBE_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::lancerBombe) ), couleurOrange);
										break;
									case 6:
										SDL_FreeSurface(sfr_DETONATEUR_touche[0]);
										SDL_FreeSurface(sfr_DETONATEUR_touche[1]);
										sfr_DETONATEUR_touche[0] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ));
										sfr_DETONATEUR_touche[1] = ecritTexte(SDL_GetKeyName( cl->touche(clavier::declancheur) ), couleurOrange);
										break;
								}
							}
						}while( event.type != SDL_KEYDOWN );
						SDL_FreeSurface(sfr_msg); /* libération de sfr_msg */
						dessiner = 1;
						break;
					}

					default:// On eject les autres cas
						break;
				} /* fin switch */
			} /* fin case return */
		} /* fin switch */

		if( dessiner )
		{
			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position); /* blittage du background */

			position.y = 200;
			position.x = (c_ecranGeneral->w-sfr_titre->w)/2; /* centrage */
			SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position); /* blittage du titre */


			position.x = 100;
			position_droite.x = c_ecranGeneral->w-250;

			position.y = 300;//+60
			position_droite.y = position.y;
			if( highLight == 1 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> égaux */
				SDL_BlitSurface(sfr_HAUT[1], NULL, c_ecranGeneral, &position);
				SDL_BlitSurface(sfr_HAUT_touche[1], NULL, c_ecranGeneral, &position_droite); /* Blit du texte par-dessus */
			}else{
				SDL_BlitSurface(sfr_HAUT[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_HAUT_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 2 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> égaux */
				SDL_BlitSurface(sfr_BAS[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_BAS_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_BAS[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_BAS_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 3 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> égaux */
				SDL_BlitSurface(sfr_DROITE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DROITE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_DROITE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DROITE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 4 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> égaux */
				SDL_BlitSurface(sfr_GAUCHE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_GAUCHE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_GAUCHE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_GAUCHE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 5 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> égaux */
				SDL_BlitSurface(sfr_LANCER_BOMBE[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_LANCER_BOMBE_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_LANCER_BOMBE[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_LANCER_BOMBE_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.y += 60;
			position_droite.y = position.y;
			if( highLight == 6 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> égaux */
				SDL_BlitSurface(sfr_DETONATEUR[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DETONATEUR_touche[1], NULL, c_ecranGeneral, &position_droite);
			}else{
				SDL_BlitSurface(sfr_DETONATEUR[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				SDL_BlitSurface(sfr_DETONATEUR_touche[0], NULL, c_ecranGeneral, &position_droite);
			}
			position.x = (c_ecranGeneral->w-sfr_retour[0]->w)/2;
			position.y += 60;
			if( highLight == 7 ){ /* if i = highLight (exemple i = 0 implique higLight = 1 -> égaux */
				SDL_BlitSurface(sfr_retour[1], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
			}else{
				SDL_BlitSurface(sfr_retour[0], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}

	}while( continuer );/* fin while */

	// Free
	for( i=0; i<2; i++ )
	{
		SDL_FreeSurface(sfr_retour[i]);
		SDL_FreeSurface(sfr_HAUT[i]);
		SDL_FreeSurface(sfr_HAUT_touche[i]);
		SDL_FreeSurface(sfr_BAS[i]);
		SDL_FreeSurface(sfr_BAS_touche[i]);
		SDL_FreeSurface(sfr_DROITE[i]);
		SDL_FreeSurface(sfr_DROITE_touche[i]);
		SDL_FreeSurface(sfr_GAUCHE[i]);
		SDL_FreeSurface(sfr_GAUCHE_touche[i]);
		SDL_FreeSurface(sfr_LANCER_BOMBE[i]);
		SDL_FreeSurface(sfr_LANCER_BOMBE_touche[i]);
		SDL_FreeSurface(sfr_DETONATEUR[i]);
		SDL_FreeSurface(sfr_DETONATEUR_touche[i]);
	}

	SDL_FreeSurface(sfr_titre);
}


/***************************************************************************//*!
* @fn int moteur_sdl::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour )
* @brief Créer un menu pour récupérer un nombre entré par un utilisateur
* @param[in] titre			 Le titre du menu
* @param[in] valeurParDefaut La valeur par défaut
* @param[in] valeurMin		 La valeur minimum
* @param[in] valeurMax		 La valeur maximum
* @param[out] valeurRetour	 Dans cette variable sera stocké, le nombre obtenu a la fin de la fonction
* @return
*	- 2 : Nombre validé et accèpté
*	- 3 : Action annulée
*/
int moteur_sdl::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour )
{
	unsigned int highLight = 2;
	bool continuer = 1;
	SDL_Event event;
	SDL_Color couleurOrange = {255, 184, 0, 0/*unused*/};
	SDL_Color couleurRouge = {255, 0, 0, 0/*unused*/};
	SDL_Rect position;
	SDL_Surface* sfr_titre = 0;
	SDL_Surface* sfr_msg = 0;
	SDL_Surface** textes;
	unsigned int i;
	bool dessiner = 1;
	bool dessinerNombre = 0;

	char valeurRetourTexte[/*Taille max d'un unsigned int*/getSizeOfNumber<-1>::value+1/*Pour le signe*/+4*2/*Pour les <--*/+1/*Le \0*/];// <- Calcule :: lenght(2^(sizeof(int)*8))+4*2

	if( valeurMin > valeurParDefaut || valeurParDefaut > valeurMax ){
		stdError("Valeur incorrect ! valeurMin(%d) <= valeurParDefaut(%d) && valeurParDefaut(%d) <= valeurMax(%d)", valeurMin, valeurParDefaut, valeurParDefaut, valeurMax);
		valeurParDefaut = (valeurMax - valeurMin)/2+valeurMin;
	}

	*valeurRetour = valeurParDefaut;
	sprintf(valeurRetourTexte, "<-- %d -->", *valeurRetour);

	textes = new SDL_Surface*[3*2];

	/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
	textes[0] = ecritTexte(valeurRetourTexte);
	textes[1] = ecritTexte("Ok");
	textes[2] = ecritTexte("Retour");

	// Création du texte HighLight
	textes[3] = ecritTexte(valeurRetourTexte, couleurOrange);
	textes[4] = ecritTexte("Ok", couleurOrange);
	textes[5] = ecritTexte("Retour", couleurOrange);

	// Création du titre
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_UNDERLINE);
	sfr_titre = ecritTexte(titre);
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_NORMAL);


	while( continuer && c_fenetreOuverte )
	{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = 3;// On veut quitter !
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: {
				switch( event.key.keysym.sym )
				{
					case SDLK_UP: { /* Appui sur la touche Echap, on arrête le programme */
						if( highLight == 1 ){// On bouge notre highLight vers le haut
							highLight = 3;
						}else{
							highLight--;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_DOWN: {// On bouge notre highLight vers le bas
						if( highLight == 3 ){
							highLight = 1;
						}else{
							highLight++;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_RIGHT: {
						if( highLight != 1 )// Pour être certain que l'on ne change les nombre que si l'on est sur le 1er element
							break;

						if( *valeurRetour+1 > valeurMax )
							*valeurRetour = valeurMin;
						else
							(*valeurRetour)++;

						// On redessine le nombre
						dessinerNombre = 1;
						dessiner = 1;
						break;
					}
					case SDLK_LEFT: {
						if( highLight != 1 )// Pour être certain que l'on ne change les nombre que si l'on est sur le 1er element
							break;

						if( *valeurRetour-1 < valeurMin )
							*valeurRetour = valeurMax;
						else
							(*valeurRetour)--;

						// On redessine le nombre
						dessinerNombre = 1;
						dessiner = 1;
						break;
					}
					case SDLK_RETURN: {// On a validé notre choix par entrer
						if( highLight == 1 ){// On a selectionné un lien de modification de texte
							dessiner = 1;
							sfr_msg = ecritTexte("Entrez le nombre manuellement", couleurRouge);
							do{
								SDL_WaitEvent(&event);
								if( event.type == SDL_QUIT ){
									continuer = 0;
									highLight = 3;
									c_fenetreOuverte = false;
								}

								if( event.type == SDL_KEYDOWN )
								switch( traductionClavier(&event.key) )
								{
									case SDLK_RIGHT:
									case SDLK_UP: {
										if( *valeurRetour+1 > valeurMax )
											*valeurRetour = valeurMin;
										else
											(*valeurRetour)++;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_LEFT:
									case SDLK_DOWN: {
										if( *valeurRetour-1 < valeurMin )
											*valeurRetour = valeurMax;
										else
											(*valeurRetour)--;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_BACKSPACE: {
										*valeurRetour /= 10;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_ESCAPE: {
										*valeurRetour = valeurParDefaut;
										continuer = 0;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_RETURN: {
										if( !(valeurMin <= *valeurRetour && *valeurRetour <= valeurMax) ){
											*valeurRetour = valeurMin;
											// On redessine le nombre
											dessinerNombre = 1;
										}else{
											highLight = 2;
										}
										continuer = 0;
										break;
									}
									case SDLK_PLUS:
									case SDLK_MINUS:
									case SDLK_KP_PLUS:
									case SDLK_KP_MINUS: {
										if( valeurMin <= *valeurRetour*(-1) && *valeurRetour*(-1) <= valeurMax )
											*valeurRetour *= -1;
										dessiner = 1;// On redessine
										break;
									}
									default: {
										// Ajout de chiffre à la main
										if( SDLK_0 <= event.key.keysym.unicode && event.key.keysym.unicode <= SDLK_9 && *valeurRetour*10+(int)(event.key.keysym.unicode-SDLK_0) <= valeurMax )
											*valeurRetour = *valeurRetour*10+(event.key.keysym.unicode-SDLK_0);
										if( SDLK_KP0 <= event.key.keysym.unicode && event.key.keysym.unicode <= SDLK_KP0 && *valeurRetour*10+(int)(event.key.keysym.unicode-SDLK_KP0) <= valeurMax )
											*valeurRetour = *valeurRetour*10+(event.key.keysym.unicode-SDLK_KP0);
										dessiner = 1;// On redessine
										break;
									}
								}

								/***********************************************************************
								* Blitage général
								* > On blit uniquement s'il y a eu une modification
								*/
								if( dessiner ){
									sprintf(valeurRetourTexte, "<-- %d -->", *valeurRetour);
									SDL_FreeSurface(textes[0]);
									SDL_FreeSurface(textes[3]);
									textes[0] = ecritTexte(valeurRetourTexte);
									textes[3] = ecritTexte(valeurRetourTexte, couleurRouge);

									position.x = 0;
									position.y = 0;
									SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

									position.y = 200;
									position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
									SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background

									// On blit le texte
									for( i=0; i<3; i++ )
									{
										position.y += 100;// Espacement entre les textes ( blit )
										position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
										if( i == highLight-1 ){
											SDL_BlitSurface(textes[i+3], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
										}else{
											SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
										}
									}

									SDL_Flip(c_ecranGeneral);
									dessiner = 0;
								}
							}while( continuer );
							SDL_FreeSurface(sfr_msg);
							SDL_FreeSurface(textes[3]);
							textes[3] = ecritTexte(valeurRetourTexte, couleurOrange);
							dessiner = 1;
							continuer = 1;
						}else
							continuer = 0;// On sort de la boucle
						break;
					}
					case SDLK_ESCAPE: { /* Appui sur la touche Echap, on arrête le programme */
						highLight = 3;// On veut quitter !
						continuer = 0;
						break;
					}
					default:
						break;
				}
				break;
			}
			/*******************************************************************
			* Prise en charge de la souris
			*/
			case SDL_MOUSEBUTTONUP: {
				position.y = 200;
				for( i=0; i<3; i++ )
				{
					position.y += 100;// Espacement entre les textes ( blit )
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					if( position.x <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
						if( highLight == i+1 ){
							if( highLight == 1 ){
								if( position.x <= event.button.x && event.button.x <= position.x+35 && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
									if( *valeurRetour-1 < valeurMin )
										*valeurRetour = valeurMax;
									else
										(*valeurRetour)--;

									// On redessine le nombre
									dessinerNombre = 1;
									dessiner = 1;
								}
								if( position.x+textes[i]->w-35 <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
									if( *valeurRetour+1 > valeurMax )
										*valeurRetour = valeurMin;
									else
										(*valeurRetour)++;

									// On redessine le nombre
									dessinerNombre = 1;
									dessiner = 1;
								}
							}else{
								continuer = 0;
							}
						}else{
							highLight = i+1;
							dessiner = 1;
						}
					}
				}
				break;
			}
			default:
				break;
		}

		/***********************************************************************
		* Blitage général
		* > On blit uniquement s'il y a eu une modification
		*/
		if( dessiner ){
			if( dessinerNombre ){
				// On redessine le nombre
				sprintf(valeurRetourTexte, "<-- %d -->", *valeurRetour);
				SDL_FreeSurface(textes[0]);
				SDL_FreeSurface(textes[3]);
				textes[0] = ecritTexte(valeurRetourTexte);
				textes[3] = ecritTexte(valeurRetourTexte, couleurOrange);
				dessinerNombre = 0;
			}

			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

			position.y = 200;// Point de départ
			position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
			SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background

			// On blit le texte
			for( i=0; i<3; i++ )
			{
				position.y += 100;// Espacement entre les textes ( blit )
				position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
				if( i == highLight-1 ){
					SDL_BlitSurface(textes[i+3], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}else{
					SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}
	}

	for( i=0; i<3*2; i++ )// On libère les textes
		SDL_FreeSurface(textes[i]);
	delete[] textes;// On libère les textes

	SDL_FreeSurface(sfr_titre);

	return highLight;
}


/***************************************************************************//*!
* @fn int moteur_sdl::getTexte( const char titre[], char texteRetour[21] )
* @brief Permet d'obtenir du texte
* @param[in] titre				Le titre du menu
* @param[out] texteRetour		Dans cette variable sera stocké, le texte obtenu a la fin de la fonction
* @return
*	- 2 : Texte validé et accèpté
*	- 3 : Action annulée
*/
int moteur_sdl::getTexte( const char titre[], char texteRetour[21] )
{
	unsigned int highLight = 1;
	bool continuer = 1;
	SDL_Event event;
	SDL_Color couleurOrange = {255, 184, 0, 0/*unused*/};
	SDL_Color couleurRouge = {255, 0, 0, 0/*unused*/};
	SDL_Rect position;
	SDL_Surface* sfr_titre=0;
	SDL_Surface* sfr_msg = 0;
	SDL_Surface** textes;
	unsigned int tailleTexteRetour;
	unsigned int i;
	bool dessiner = 1;
	bool dessinerMot = 0;

	tailleTexteRetour = strlen(texteRetour);

	textes = new SDL_Surface*[3*2];

	// Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal)
	if( tailleTexteRetour )
		textes[0] = ecritTexte(texteRetour);
	else
		textes[0] = ecritTexte(completerMot(texteRetour, tailleTexteRetour));
	textes[1] = ecritTexte("Ok");
	textes[2] = ecritTexte("Retour");

	// Création du texte HighLight
	textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
	textes[4] = ecritTexte("Ok", couleurOrange);
	textes[5] = ecritTexte("Retour", couleurOrange);

	// Création du titre
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_UNDERLINE);
	sfr_titre = ecritTexte(titre);
	TTF_SetFontStyle(c_policeGeneral, TTF_STYLE_NORMAL);
	int taille = 70;
	while( sfr_titre->w > c_ecranGeneral->w ){
		SDL_FreeSurface(sfr_titre);

		TTF_Font* police;
		if( !(police = TTF_OpenFont("Chicken Butt.ttf", taille)) )
			stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

		TTF_SetFontStyle(police, TTF_STYLE_UNDERLINE);
		sfr_titre = ecritTexte(titre, police);
		TTF_SetFontStyle(police, TTF_STYLE_NORMAL);
		taille-=5;

		TTF_CloseFont(police);
	}


	while( continuer )
	{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = 3;// On veut quitter !
				continuer = 0;
				c_fenetreOuverte = false;
				break;
			}
			case SDL_KEYDOWN: {
				switch( traductionClavier(&event.key) )
				{
					case SDLK_UP: { /* Appui sur la touche Echap, on arrête le programme */
						if( highLight == 1 ){// On bouge notre highLight vers le haut
							highLight = 3;
						}else{
							highLight--;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_DOWN: {// On bouge notre highLight vers le bas
						if( highLight == 3 ){
							highLight = 1;
						}else{
							highLight++;
						}
						dessiner = 1;// On redessine
						break;
					}
					case SDLK_BACKSPACE: {
						if( highLight != 1 )// Si on est pas sur le texte => on fait rien
							break;

						if( tailleTexteRetour == 0 )
							break;
						tailleTexteRetour--;
						texteRetour[tailleTexteRetour] = '_';
						dessiner = 1;// On redessine
						dessinerMot = 1;
						break;
					}
					case SDLK_RETURN: {// On a validé notre choix par entrer
						if( highLight == 1 ){// On a selectionné un lien de modification de texte
							trimString(texteRetour);
							if( tailleTexteRetour ){
								highLight = 2;
							}else{
								if( sfr_msg )
									SDL_FreeSurface(sfr_msg);
								sfr_msg = ecritTexte("Le texte dois avoir 1 caractere min", couleurRouge);
							}
							dessiner = 1;
						}else{
							if( highLight == 2 ){
								if( tailleTexteRetour ){
									continuer = 0;// On sort de la boucle
								}else{
									if( sfr_msg )
										SDL_FreeSurface(sfr_msg);
									sfr_msg = ecritTexte("Le texte dois avoir 1 caractere min", couleurRouge);
									dessiner = 1;
								}
							}else{
								continuer = 0;// On sort de la boucle
							}
						}
						break;
					}
					case SDLK_ESCAPE: { /* Appui sur la touche Echap, on arrête le programme */
						highLight = 3;// On veut quitter !
						continuer = 0;
						break;
					}
					default: {
						if( highLight != 1 )// Si on est pas sur le texte => on fait rien
							break;

						//' ' <= key && key <= '~'
						// Ajout de chiffre à la main
						if( SDLK_SPACE <= event.key.keysym.unicode && event.key.keysym.unicode <= 126 && tailleTexteRetour+1 < 21  ){
							texteRetour[tailleTexteRetour] = event.key.keysym.unicode;
							tailleTexteRetour++;
							completerMot(texteRetour, tailleTexteRetour);
						}
					//	if( SDLK_KP0 <= event.key.keysym.unicode && event.key.keysym.unicode <= SDLK_KP0 && *valeurRetour*10+(int)(event.key.keysym.unicode-SDLK_KP0) <= valeurMax )
					//		*valeurRetour = *valeurRetour*10+(event.key.keysym.unicode-SDLK_KP0);
						dessiner = 1;// On redessine
						dessinerMot = 1;
						break;
					}
				}
				break;
			}
			/*******************************************************************
			* Prise en charge de la souris
			*/
			case SDL_MOUSEBUTTONUP: {
				position.y = 200;
				for( i=0; i<3; i++ )
				{
					position.y += 100;// Espacement entre les textes ( blit )
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					if( position.x <= event.button.x && event.button.x <= position.x+textes[i]->w && position.y <= event.button.y && event.button.y <= position.y+textes[i]->h ){
						if( highLight == i+1 ){
							if( highLight != 1 )
								continuer = 0;
						}else{
							highLight = i+1;
							dessiner = 1;
						}
					}
				}
				break;
			}
			default:
				break;
		}

		/***********************************************************************
		* Blitage général
		* > On blit uniquement s'il y a eu une modification
		*/
		if( dessiner ){
			if( dessinerMot ){
				SDL_FreeSurface(textes[0]);
				SDL_FreeSurface(textes[3]);

				texteRetour[tailleTexteRetour] = 0;
				trimString(texteRetour);
				tailleTexteRetour = strlen(texteRetour);

				if( tailleTexteRetour )
					textes[0] = ecritTexte(texteRetour);
				else
					textes[0] = ecritTexte(completerMot(texteRetour, tailleTexteRetour));
				textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
				dessinerMot = 0;
			}

			position.x = 0;
			position.y = 0;
			SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

			position.y = 200;
			if( sfr_msg ){
				position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
				SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background
			}else{
				position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
				SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background
			}
			// On blit le texte
			for( i=0; i<3; i++ )
			{
				position.y += 100;// Espacement entre les textes ( blit )
				if( i == highLight-1 ){
					position.x = (c_ecranGeneral->w-textes[i+3]->w)/2;// Centrage auto des blit ^^
					SDL_BlitSurface(textes[i+3], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}else{
					position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
					SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); /* Blit du texte par-dessus */
				}
			}

			SDL_Flip(c_ecranGeneral);
			dessiner = 0;
		}
	}

	for( i=0; i<3*2; i++ )// On libère les textes
		SDL_FreeSurface(textes[i]);
	delete[] textes;// On libère les textes

	SDL_FreeSurface(sfr_titre);

	if( sfr_msg )
		SDL_FreeSurface(sfr_msg);

	texteRetour[tailleTexteRetour] = 0;
	trimString(texteRetour);
	tailleTexteRetour = strlen(texteRetour);

	return highLight;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER moteur_sdl::afficherMapEtEvent( partie* p )
* @brief Affiche une map
* @param[in] p	La partie en cours a afficher
* @return La touche actuellement appuyé
*/
SYS_CLAVIER moteur_sdl::afficherMapEtEvent( partie* p )
{
	if( !c_Instance )
		stdErrorE("Veuillez instancier la class moteur_sdl !");

	bool dessiner = 0;
	bool refresh = 0;
	SDL_Surface* sfr_tmp;
	map* l_map = p->getMap();// l_map pour local variable map
	// Décalage
	unsigned int	xpos=(c_Instance->c_ecranGeneral->w-l_map->X()*32)/2,
					ypos=(c_Instance->c_ecranGeneral->h-l_map->Y()*32)/2;
	ypos = ypos+ypos/2;

	SDL_Rect pos;
	SDL_Rect imgRect;

	// Tester ici si la taille de la map n'est pas trop grande pour la fenetre.
	if ( l_map->X() > (unsigned int)c_Instance->c_ecranGeneral->w/32 || l_map->Y() > (unsigned int)c_Instance->c_ecranGeneral->h/32 )
		stdErrorE("%u > %d || %u > %d", l_map->X(), c_Instance->c_ecranGeneral->w/32, l_map->Y(), c_Instance->c_ecranGeneral->h/32);

	if( c_Instance->c_premierAffichage ){
		// Traitement a faire sur les cadres ICI
		// A FAIRE
		pos.x = 0;
		pos.y = 0;
		SDL_FillRect(c_Instance->c_ecranGeneral, NULL, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

		for( unsigned int x, y=0; y<l_map->Y(); y++ )
		{
			for( x=0; x<l_map->X(); x++ )
			{
				pos.x=x*32+xpos;
				pos.y=y*32+ypos;
				switch( l_map->getBlock(x, y)->element )
				{
					case map::vide: {
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::Mur_destructible: {
						SDL_BlitSurface(c_Instance->c_Decor[mur_destructible], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::Mur_INdestructible: {
						SDL_BlitSurface(c_Instance->c_Decor[mur_indestructible], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::UN_joueur: {
						if( !l_map->getBlock(x, y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y)->joueur=0", x, y);
						if( !l_map->getBlock(x, y)->joueur->size() )
							stdErrorE("l_map->getBlock(%u, %u)->joueur->size()=0", x, y);

						unsigned char joueur = l_map->getBlock(x, y)->joueur->at(0);
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
						break;
					}
					case map::plusieurs_joueurs: {
						if( !l_map->getBlock(x, y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y)->joueur=0", x, y);

						unsigned char joueur;
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						for( int i=l_map->getBlock(x, y)->joueur->size()-1; i>=0; i-- ){
							joueur = l_map->getBlock(x, y)->joueur->at(i);
							c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
						}
						break;
					}
					case map::bombe_poser: {
						if( c_Instance->isInSpriteList(coordonneeConvert(x, y)) == -1 ){
							Sprite b;
							b.pos = coordonneeConvert(x, y);
							b.objet = bombe;
							b.frame = 0;
							b.refresh = 0;
							c_Instance->c_ListSprite.push_back(b);
						}

						imgRect.x = 0;
						imgRect.y = 0;
						imgRect.w = 32;
						imgRect.h = 32;
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::bombe_poser_AVEC_UN_joueur:
					case map::bombe_poser_AVEC_plusieurs_joueurs: {
						if( !l_map->getBlock(x, y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y)->joueur=0", x, y);

						if( l_map->getBlock(x, y)->joueur->size() <= 1 )
							stdErrorE("Nombre d'info joueur (%u) incorrect !", l_map->getBlock(x, y)->joueur->size());

						unsigned char joueur = l_map->getBlock(x, y)->joueur->at(1);

						if( c_Instance->isInSpriteList(coordonneeConvert(x, y)) == -1 ){
							Sprite b;
							b.pos = coordonneeConvert(x, y);
							b.objet = bombe;
							b.frame = 0;
							b.refresh = 0;
							c_Instance->c_ListSprite.push_back(b);
						}

						imgRect.x = 0;
						imgRect.y = 0;
						imgRect.w = 32;
						imgRect.h = 32;
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
						break;
					}
					case map::flamme_origine: {
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[flamme_origine], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_horizontal: {
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[flamme_horizontal], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_vertical: {
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[flamme_vertical], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_haut: {
						SDL_BlitSurface(c_Instance->c_Decor[flamme_pointe_haut], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_bas: {
						SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_bas], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_droite: {
						SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_droite], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_gauche: {
						SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_gauche], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::bonus: {
						if( !l_map->getBlock(x, y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y).joueur=0", x, y);

						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						switch( (bonus::t_Bonus)l_map->getBlock(x, y)->joueur->at(0) )
						{
							case bonus::bombe: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::puissance_flamme: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::declancheur: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::pousse_bombe: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case bonus::vie: {
								SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							default: {
								stdError("Erreur lors de la lecture de la map, BONUS inconu <%d>", (int)l_map->getBlock(x, y)->joueur->at(0));
								break;
							}
						}
						break;
					}
					default: {
						stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(x, y)->element);
						break;
					}
				}
			}
		}

		SDL_Flip(c_Instance->c_ecranGeneral);// afficher la map
	}else{
		s_Coordonnees v_pos;
		while( l_map->getModifications(v_pos) )
		{
			dessiner = 1;
			pos.x=v_pos.x*32+xpos;
			pos.y=v_pos.y*32+ypos;
			switch( l_map->getBlock(v_pos)->element )
			{
				case map::vide: {
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::Mur_destructible: {
					SDL_BlitSurface(c_Instance->c_Decor[mur_destructible], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::Mur_INdestructible: {
					SDL_BlitSurface(c_Instance->c_Decor[mur_indestructible], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::UN_joueur: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					if( !l_map->getBlock(v_pos)->joueur->size() )
						stdErrorE("l_map->getBlock(%u, %u)->joueur->size()=0", v_pos.x, v_pos.y);


					unsigned char joueur = l_map->getBlock(v_pos)->joueur->at(0);
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
					break;
				}
				case map::plusieurs_joueurs: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					unsigned char joueur;
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					for( int i=l_map->getBlock(v_pos)->joueur->size()-1; i>=0; i-- ){
						joueur = l_map->getBlock(v_pos)->joueur->at(i);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
					}
					break;
				}
				case map::bombe_poser: {
					if( c_Instance->isInSpriteList(v_pos) == -1 ){
						Sprite b;
						b.pos = v_pos;
						b.objet = bombe;
						b.frame = 0;
						b.refresh = 0;
						c_Instance->c_ListSprite.push_back(b);
					}

					imgRect.x = 0;
					imgRect.y = 0;
					imgRect.w = 32;
					imgRect.h = 32;
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::bombe_poser_AVEC_UN_joueur: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					if( l_map->getBlock(v_pos)->joueur->size() <= 1 )
						stdErrorE("Nombre d'info joueur (%u) incorrect !", l_map->getBlock(v_pos)->joueur->size());

					unsigned char joueur = l_map->getBlock(v_pos)->joueur->at(1);

					if( c_Instance->isInSpriteList(v_pos) == -1 ){
						Sprite b;
						b.pos = v_pos;
						b.objet = bombe;
						b.frame = 0;
						b.refresh = 0;
						c_Instance->c_ListSprite.push_back(b);
					}

					imgRect.x = 0;
					imgRect.y = 0;
					imgRect.w = 32;
					imgRect.h = 32;
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
					c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
					break;
				}
				case map::bombe_poser_AVEC_plusieurs_joueurs: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					if( l_map->getBlock(v_pos)->joueur->size() <= 1 )
						stdErrorE("Nombre d'info joueur (%u) incorrect !", l_map->getBlock(v_pos)->joueur->size());

					unsigned char joueur;

					if( c_Instance->isInSpriteList(v_pos) == -1 ){
						Sprite b;
						b.pos = v_pos;
						b.objet = bombe;
						b.frame = 0;
						b.refresh = 0;
						c_Instance->c_ListSprite.push_back(b);
					}

					imgRect.x = 0;
					imgRect.y = 0;
					imgRect.w = 32;
					imgRect.h = 32;
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
					for( int i=l_map->getBlock(v_pos)->joueur->size()-1; i>=1; i-- ){
						joueur = l_map->getBlock(v_pos)->joueur->at(i);
						c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
					}
					break;
				}
				case map::flamme_origine: {
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[flamme_origine], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_horizontal: {

					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[flamme_horizontal], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_vertical: {
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[flamme_vertical], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_haut: {
					SDL_BlitSurface(c_Instance->c_Decor[flamme_pointe_haut], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_bas: {
					SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_bas], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_droite: {
					SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_droite], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_gauche: {
					SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_gauche], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::bonus: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x, y).joueur=0", v_pos.x, v_pos.y);

					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					switch( (bonus::t_Bonus)l_map->getBlock(v_pos)->joueur->at(0) )
					{
						case bonus::bombe: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::puissance_flamme: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::declancheur: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::pousse_bombe: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case bonus::vie: {
							SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						default: {
							stdError("Erreur lors de la lecture de la map, BONUS inconu <%d>", (int)l_map->getBlock(v_pos)->joueur->at(0));
							break;
						}
					}
					break;
				}
				default: {
					stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(v_pos)->element);
					break;
				}
			}
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, 32, 32);
		}
	}

	/***************************************************************************
	* Blitage des animations
	*/
	for( unsigned int i=0; i<c_Instance->c_ListSprite.size(); i++ )
	{
		switch( c_Instance->c_ListSprite.at(i).objet )
		{
			case bombe: {
				switch( l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->element )
				{
					case map::bombe_poser: {
						if( c_Instance->c_ListSprite.at(i).refresh < clock() ){
							pos.x=c_Instance->c_ListSprite.at(i).pos.x*32+xpos;
							pos.y=c_Instance->c_ListSprite.at(i).pos.y*32+ypos;
							imgRect.x = (c_Instance->c_ListSprite.at(i).frame%(c_Instance->c_Decor[bombe]->w/32))*32;
							imgRect.y = 0;
							imgRect.w = 32;
							imgRect.h = 32;
							SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
							SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
							c_Instance->c_ListSprite.at(i).frame++;
							c_Instance->c_ListSprite.at(i).refresh = clock()+VitesseSpriteBombe;

							SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, 32, 32);
						}
						break;
					}
					case map::bombe_poser_AVEC_UN_joueur: {
						if( c_Instance->c_ListSprite.at(i).refresh < clock() ){
							if( !l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur )
								stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(pos).joueur=0", c_Instance->c_ListSprite.at(i).pos.x, c_Instance->c_ListSprite.at(i).pos.y);

							unsigned char joueur = l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur->at(1);

							pos.x=c_Instance->c_ListSprite.at(i).pos.x*32+xpos;
							pos.y=c_Instance->c_ListSprite.at(i).pos.y*32+ypos;
							imgRect.x = (c_Instance->c_ListSprite.at(i).frame%(c_Instance->c_Decor[bombe]->w/32))*32;
							imgRect.y = 0;
							imgRect.w = 32;
							imgRect.h = 32;
							SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
							SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
							c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
							c_Instance->c_ListSprite.at(i).frame++;
							c_Instance->c_ListSprite.at(i).refresh = clock()+VitesseSpriteBombe;

							SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, 32, 32);
						}
						break;
					}
					case map::bombe_poser_AVEC_plusieurs_joueurs: {
						if( c_Instance->c_ListSprite.at(i).refresh < clock() ){
							if( !l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur )
								stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(pos).joueur=0", c_Instance->c_ListSprite.at(i).pos.x, c_Instance->c_ListSprite.at(i).pos.y);

							unsigned char joueur;

							pos.x=c_Instance->c_ListSprite.at(i).pos.x*32+xpos;
							pos.y=c_Instance->c_ListSprite.at(i).pos.y*32+ypos;
							imgRect.x = (c_Instance->c_ListSprite.at(i).frame%(c_Instance->c_Decor[bombe]->w/32))*32;
							imgRect.y = 0;
							imgRect.w = 32;
							imgRect.h = 32;
							SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
							SDL_BlitSurface(c_Instance->c_Decor[bombe], &imgRect, c_Instance->c_ecranGeneral, &pos);
							for( int k=l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur->size()-1; k>=1; k-- ){
								joueur = l_map->getBlock(c_Instance->c_ListSprite.at(i).pos)->joueur->at(k);
								c_Instance->joueur_orientation( p->joueur(joueur-1)->orientation(), joueur, &pos );
							}
							c_Instance->c_ListSprite.at(i).frame++;
							c_Instance->c_ListSprite.at(i).refresh = clock()+VitesseSpriteBombe;

							SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, 32, 32);
						}
						break;
					}
					default: {
						c_Instance->c_ListSprite.erase(c_Instance->c_ListSprite.begin()+i);
						i--;
						break;
					}
				}
				break;
			}
			default: {
				stdError("Attention objet animé inconnu ! X=%u, Y=%u, objet=%d", c_Instance->c_ListSprite.at(i).pos.x, c_Instance->c_ListSprite.at(i).pos.y, c_Instance->c_ListSprite.at(i).objet);
				c_Instance->c_ListSprite.erase(c_Instance->c_ListSprite.begin()+i);
				i--;
				break;
			}
		}
	}


	static SDL_Color couleurBlanche = {255,255,255,0/*unused*/};
	static SDL_Color couleurNoire = {0,0,0,0/*unused*/};

	/***************************************************************************
	* Affichage du temps de jeu restant
	*/
	pos.x = 0;
	pos.y = 0;
	char tempsAff[30];
	if( p->timeOut() > clock() ){
		sprintf(tempsAff, "%02ldmin:%02ldsecs", (p->timeOut()-clock())/(CLOCKS_PER_SEC*60), ((p->timeOut()-clock())%(CLOCKS_PER_SEC*60))/CLOCKS_PER_SEC);
		sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurNoire, 40);
	}else {
		sfr_tmp = c_Instance->ecritTexte("00min:00secs", couleurNoire, 40);
	}
	imgRect.x = 0;
	imgRect.y = 0;
	imgRect.w = sfr_tmp->w+15;
	imgRect.h = sfr_tmp->h;
	SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
	SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
	SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, sfr_tmp->h);
	SDL_FreeSurface(sfr_tmp);


	/***************************************************************************
	* Affichage du joueur 1
	*/
	// Affichage du nom du joueur 1
	pos.x = 10;
	pos.y = ypos;
	refresh = p->playerNeedRefresh(0);
	if( refresh ){
		// Affichage du nom du joueur 1
		sfr_tmp = c_Instance->ecritTexte(p->joueur(0)->nom()->c_str(), couleurNoire, 55);
		// On remplis le fond
		imgRect.x = pos.x;
		imgRect.y = pos.y;
		imgRect.w = xpos-pos.x;
		imgRect.h = sfr_tmp->h;
		SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
		if( (unsigned int)pos.x+(unsigned int)sfr_tmp->w > xpos ){
			imgRect.x = 0;
			imgRect.y = 0;
			SDL_BlitSurface(sfr_tmp, &imgRect, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		}else{
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
		}
		SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
		SDL_FreeSurface(sfr_tmp);
	}
	pos.y += 25;// Espacement
	if( p->joueur(0)->armements() ){
		// Background
		imgRect.w = xpos-40;// 40 (espacement bonus)
		imgRect.h = 32;

		// Bombe
		static unsigned char nbBombe = 0;
		pos.x = 40;
		pos.y += 35;
		if( nbBombe != p->joueur(0)->armements()->quantiteMAX(bonus::bombe) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::bombe) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

			SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			nbBombe = p->joueur(0)->armements()->quantiteMAX(bonus::bombe);
		}

		// Puissance de flamme
		static unsigned char PuissFlamme = 0;
		pos.x = 40;
		pos.y += 35;
		if( PuissFlamme != p->joueur(0)->armements()->quantiteMAX(bonus::puissance_flamme) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::puissance_flamme) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

			SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			PuissFlamme = p->joueur(0)->armements()->quantiteMAX(bonus::puissance_flamme);
		}

		// Déclancheur
		static unsigned char Declancheur = 0;
		pos.x = 40;
		pos.y += 35;
		if( Declancheur != p->joueur(0)->armements()->quantiteMAX(bonus::declancheur) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::declancheur) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

			SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			Declancheur = p->joueur(0)->armements()->quantiteMAX(bonus::declancheur);
		}

		// Vie
		static unsigned char nbVie = 0;
		pos.x = 40;
		pos.y += 35;
		if( nbVie != p->joueur(0)->armements()->quantiteMAX(bonus::vie) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::vie) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

			SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			nbVie = p->joueur(0)->armements()->quantiteMAX(bonus::vie);
		}

		// Pousse
		static unsigned char PousseBombe = 0;
		pos.x = 40;
		pos.y += 35;
		if( PousseBombe != p->joueur(0)->armements()->quantiteMAX(bonus::pousse_bombe) || c_Instance->c_premierAffichage || refresh ){
			sprintf(tempsAff, "%d", p->joueur(0)->armements()->quantiteMAX(bonus::pousse_bombe) );
			sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

			// Blit background
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

			SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
			pos.x += 40;
			pos.y -= 6;
			SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			pos.y += 6;

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
			SDL_FreeSurface(sfr_tmp);
			PousseBombe = p->joueur(0)->armements()->quantiteMAX(bonus::pousse_bombe);
		}
	}

	/***************************************************************************
	* Affichage du joueur 4
	*/
	static bool cleanPlayer4=0;
	if( p->nbJoueurs() == 4 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(3)->isLocal() || ( !p->joueur(3)->isLocal() && p->joueur(3)->socket() != INVALID_SOCKET )))) ){
		refresh = p->playerNeedRefresh(3);
		// Affichage du nom du joueur 4
		pos.x = 10;
		pos.y = l_map->Y()*32-(25+35*4);
		if( refresh ){
			// Affichage du nom du joueur 4
			sfr_tmp = c_Instance->ecritTexte(p->joueur(3)->nom()->c_str(), couleurNoire, 55);
			// On remplis le fond
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			imgRect.w = xpos-pos.x;
			imgRect.h = sfr_tmp->h;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
			if( (unsigned int)pos.x+(unsigned int)sfr_tmp->w > xpos ){
				imgRect.x = 0;
				imgRect.y = 0;
				SDL_BlitSurface(sfr_tmp, &imgRect, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}else{
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			SDL_FreeSurface(sfr_tmp);
		}
		pos.y += 25;// Espacement
		if( p->joueur(3)->armements() ){
			// Background
			imgRect.w = xpos-40;// 40 (espacement bonus)
			imgRect.h = 32;

			// Bombe
			static unsigned char nbBombe = 0;
			pos.x = 40;
			pos.y += 35;
			if( nbBombe != p->joueur(3)->armements()->quantiteMAX(bonus::bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbBombe = p->joueur(3)->armements()->quantiteMAX(bonus::bombe);
			}

			// Puissance de flamme
			static unsigned char PuissFlamme = 0;
			pos.x = 40;
			pos.y += 35;
			if( PuissFlamme != p->joueur(3)->armements()->quantiteMAX(bonus::puissance_flamme) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::puissance_flamme) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PuissFlamme = p->joueur(3)->armements()->quantiteMAX(bonus::puissance_flamme);
			}

			// Déclancheur
			static unsigned char Declancheur = 0;
			pos.x = 40;
			pos.y += 35;
			if( Declancheur != p->joueur(3)->armements()->quantiteMAX(bonus::declancheur) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::declancheur) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				Declancheur = p->joueur(3)->armements()->quantiteMAX(bonus::declancheur);
			}

			// Vie
			static unsigned char nbVie = 0;
			pos.x = 40;
			pos.y += 35;
			if( nbVie != p->joueur(3)->armements()->quantiteMAX(bonus::vie) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::vie) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbVie = p->joueur(3)->armements()->quantiteMAX(bonus::vie);
			}

			// Pousse Bombe
			static unsigned char PousseBombe = 0;
			pos.x = 40;
			pos.y += 35;
			if( PousseBombe != p->joueur(3)->armements()->quantiteMAX(bonus::pousse_bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(3)->armements()->quantiteMAX(bonus::pousse_bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PousseBombe = p->joueur(3)->armements()->quantiteMAX(bonus::pousse_bombe);
			}
		}
		cleanPlayer4 = 1;
	}else{
		if( cleanPlayer4 ){
			// Blit background
			imgRect.x = 10;
			imgRect.y = l_map->Y()*32-(25+35*4);
			imgRect.w = xpos;
			imgRect.h = 25+35*6;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
			pos.x = 10;
			pos.y = l_map->Y()*32;
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			cleanPlayer4 = 0;
		}
	}

	/***************************************************************************
	* Affichage du joueur 2
	*/
	static bool cleanPlayer2=0;
	if( p->nbJoueurs() > 1 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(1)->isLocal() || ( !p->joueur(1)->isLocal() && p->joueur(1)->socket() != INVALID_SOCKET )))) ){
		refresh = p->playerNeedRefresh(1);
		// Affichage du nom du joueur 2
		pos.x = l_map->X()*32+xpos+10;
		pos.y = l_map->Y()*32-(25+35*4);

		if( refresh ){
			// Affichage du nom du joueur 2
			sfr_tmp = c_Instance->ecritTexte(p->joueur(1)->nom()->c_str(), couleurNoire, 55);

			// On remplis le fond
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			imgRect.w = xpos-10;
			imgRect.h = sfr_tmp->h;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
			if( pos.x+sfr_tmp->w > c_Instance->c_ecranGeneral->w ){
				imgRect.x = 0;
				imgRect.y = 0;
				SDL_BlitSurface(sfr_tmp, &imgRect, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}else{
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}

			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			SDL_FreeSurface(sfr_tmp);
		}
		pos.y += 25;// Espacement
		if( p->joueur(1)->armements() ){
			// Background
			imgRect.w = xpos-40;// 40 (espacement bonus)
			imgRect.h = 32;

			// Bombe
			static unsigned char nbBombe = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( nbBombe != p->joueur(1)->armements()->quantiteMAX(bonus::bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbBombe = p->joueur(1)->armements()->quantiteMAX(bonus::bombe);
			}

			// Puissance de flamme
			static unsigned char PuissFlamme = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( PuissFlamme != p->joueur(1)->armements()->quantiteMAX(bonus::puissance_flamme) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::puissance_flamme) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PuissFlamme = p->joueur(1)->armements()->quantiteMAX(bonus::puissance_flamme);
			}

			// Déclancheur
			static unsigned char Declancheur = 255;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( Declancheur != p->joueur(1)->armements()->quantiteMAX(bonus::declancheur) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::declancheur) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				Declancheur = p->joueur(1)->armements()->quantiteMAX(bonus::declancheur);
			}

			// Vie
			static unsigned char nbVie = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( nbVie != p->joueur(1)->armements()->quantiteMAX(bonus::vie) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::vie) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbVie = p->joueur(1)->armements()->quantiteMAX(bonus::vie);
			}

			// Pousse Bombe
			static unsigned char PousseBombe = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( PousseBombe != p->joueur(1)->armements()->quantiteMAX(bonus::pousse_bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(1)->armements()->quantiteMAX(bonus::pousse_bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PousseBombe = p->joueur(1)->armements()->quantiteMAX(bonus::pousse_bombe);
			}
		}
		cleanPlayer2 = 1;
	}else{
		if( cleanPlayer2 ){
			// Blit background
			imgRect.x = l_map->X()*32+xpos+10;
			imgRect.y = l_map->Y()*32-(25+35*4);
			imgRect.w = c_Instance->c_ecranGeneral->w-(l_map->X()*32+xpos);
			imgRect.h = 25+35*6;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
			pos.x = l_map->X()*32+xpos+10;
			pos.y = l_map->Y()*32-(25+35*4);
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			cleanPlayer2 = 0;
		}
	}

	/***************************************************************************
	* Affichage du joueur 3
	*/
	static bool cleanPlayer3=0;
	if( p->nbJoueurs() > 2 && (p->connection() == partie::CNX_None || ( p->connection() != partie::CNX_None && (p->joueur(2)->isLocal() || ( !p->joueur(2)->isLocal() && p->joueur(2)->socket() != INVALID_SOCKET )))) ){
		refresh = p->playerNeedRefresh(2);
		// Affichage du nom du joueur 3
		pos.x = l_map->X()*32+xpos+10;
		pos.y = ypos;
		if( refresh ){
			// Affichage du nom du joueur 3
			sfr_tmp = c_Instance->ecritTexte(p->joueur(2)->nom()->c_str(), couleurNoire, 55);

			// On remplis le fond
			imgRect.x = pos.x;
			imgRect.y = pos.y;
			imgRect.w = c_Instance->c_ecranGeneral->w-(xpos+l_map->X()*32);
			imgRect.h = sfr_tmp->h;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
			if( (unsigned int)pos.x+(unsigned int)sfr_tmp->w > imgRect.w ){
				imgRect.x = 0;
				imgRect.y = 0;
				SDL_BlitSurface(sfr_tmp, &imgRect, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}else{
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
			}
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			SDL_FreeSurface(sfr_tmp);
		}
		pos.y += 25;// Espacement
		if( p->joueur(2)->armements() ){
			// Background
			imgRect.w = xpos-40;// 40 (espacement bonus)
			imgRect.h = 32;

			// Bombe
			static unsigned char nbBombe = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( nbBombe != p->joueur(2)->armements()->quantiteMAX(bonus::bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbBombe = p->joueur(2)->armements()->quantiteMAX(bonus::bombe);
			}

			// Puissance de flamme
			static unsigned char PuissFlamme = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( PuissFlamme != p->joueur(2)->armements()->quantiteMAX(bonus::puissance_flamme) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::puissance_flamme) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_puissance_flamme], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PuissFlamme = p->joueur(2)->armements()->quantiteMAX(bonus::puissance_flamme);
			}

			// Déclancheur
			static unsigned char Declancheur = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( Declancheur != p->joueur(2)->armements()->quantiteMAX(bonus::declancheur) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::declancheur) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_declancheur], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				Declancheur = p->joueur(2)->armements()->quantiteMAX(bonus::declancheur);
			}

			// Vie
			static unsigned char nbVie = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( nbVie != p->joueur(2)->armements()->quantiteMAX(bonus::vie) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::vie) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_vie], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				nbVie = p->joueur(2)->armements()->quantiteMAX(bonus::vie);
			}

			// Pousse Bombe
			static unsigned char PousseBombe = 0;
			pos.x = l_map->X()*32+xpos+40;
			pos.y += 35;
			if( PousseBombe != p->joueur(2)->armements()->quantiteMAX(bonus::pousse_bombe) || c_Instance->c_premierAffichage || refresh ){
				sprintf(tempsAff, "%d", p->joueur(2)->armements()->quantiteMAX(bonus::pousse_bombe) );
				sfr_tmp = c_Instance->ecritTexte(tempsAff, couleurBlanche, 40);

				// Blit background
				imgRect.x = pos.x;
				imgRect.y = pos.y;
				SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));

				SDL_BlitSurface(c_Instance->c_Decor[gain_pousse_bombe], NULL, c_Instance->c_ecranGeneral, &pos);
				pos.x += 40;
				pos.y -= 6;
				SDL_BlitSurface(sfr_tmp, NULL, c_Instance->c_ecranGeneral, &pos);// Blitage du Texte
				pos.y += 6;

				SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-40, pos.y, imgRect.w, 32);
				SDL_FreeSurface(sfr_tmp);
				PousseBombe = p->joueur(2)->armements()->quantiteMAX(bonus::pousse_bombe);
			}
		}
		cleanPlayer3 = 1;
	}else{
		if( cleanPlayer3 ){
			// Blit background
			imgRect.x = l_map->X()*32+xpos+10;
			imgRect.y = ypos+15;
			imgRect.w = c_Instance->c_ecranGeneral->w-(l_map->X()*32+xpos);
			imgRect.h = 25+35*5;
			SDL_FillRect(c_Instance->c_ecranGeneral, &imgRect, SDL_MapRGB(c_Instance->c_ecranGeneral->format, 131, 202, 255));
			pos.x = l_map->X()*32+xpos+10;
			pos.y = ypos+15;
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x, pos.y, imgRect.w, imgRect.h);
			cleanPlayer3 = 0;
		}
	}


	if( c_Instance->c_premierAffichage )
		c_Instance->c_premierAffichage = 0;

	SDL_Event event;
	SDL_PollEvent(&event);
	if( event.type == SDL_KEYDOWN )
		return traductionClavier(&event.key);
	else
		return SDLK_UNKNOWN;
}


/***************************************************************************//*!
* @fn SDLKey moteur_sdl::traductionClavier( const SDL_KeyboardEvent* touche )
* @brief Permet l'utilisation d'un clavier unicode en toute simplicité
* @param[in] touche L'event: event.key
* @return La touche actuellement appuyé
*
* Exemple:
* @code
* SDL_Event event;
* SDL_WaitEvent(&event);
* switch( traductionClavier(&event.key) )
* {
*	case SDLK_LEFT:
*		break;
*	case SDLK_a:
*		break;
* }
* @endcode
*/
SDLKey moteur_sdl::traductionClavier( const SDL_KeyboardEvent* touche )
{
	if( touche->keysym.unicode != SDLK_UNKNOWN ){
		return (SDLKey)touche->keysym.unicode;
	}else{
		return (SDLKey)touche->keysym.sym;
	}
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::chargerImage( const char image[] ) const
* @brief Permet de charger une image.
* @param[in] image L'image a charger
* @return Une surface (contenant l'image) a libérer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::chargerImage( const char image[] ) const
{
	SDL_Surface* tmp = IMG_Load(image);
	if( !tmp )
		stdErrorE("Erreur lors du chargement de l'image <%s> : %s", image, SDL_GetError());
	return tmp;
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[] ) const
* @brief Permet d'écrire du texte en <b>NOIR</b>
* @param[in] texte Le texte a écrire en noir
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*
* Cette fonction est un alias de moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
* @see moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[] ) const
{
	static SDL_Color couleurNoire = {0,0,0,0/*unused*/};
	return ecritTexte( texte, couleurNoire );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b ) const
* @brief Permet d'écrire du texte dans une couleur RBG
* @param[in] texte Le texte a écrire
* @param[in] r,g,b Le taux d'utilisation de chaque couleurs ( de 0 à 255 )
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*
* Cette fonction est un alias de moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
* @see moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b ) const
{
	SDL_Color couleur = {r, g, b, 0/*unused*/};
	return ecritTexte( texte, couleur );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur ) const
* @brief Permet d'écrire du texte dans une couleur
* @param[in] texte Le texte a écrire
* @param[in] couleur La couleur du texte
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur ) const
{
	SDL_Surface* tmp = TTF_RenderText_Blended(c_policeGeneral, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la création du texte <%s>, Couleur: {%u, %u, %u}, Erreur renvoyée: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

	return tmp;
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille ) const
* @brief Permet d'écrire du texte dans une couleur avec une certaine taille de police
* @param[in] texte Le texte a écrire
* @param[in] couleur La couleur a utiliser
* @param[in] taille La taille du texte
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille ) const
{
	TTF_Font* police = TTF_OpenFont("Chicken Butt.ttf", taille);
	if( !police )
		stdErrorE("Erreur lors du chargement de la police : %s", TTF_GetError());

	SDL_Surface* tmp = TTF_RenderText_Blended(police, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la création du texte <%s>, Couleur: {%u, %u, %u}, Erreur renvoyée: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

	TTF_CloseFont(police);/* Fermeture de la police */

	return tmp;
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], TTF_Font* police ) const
* @brief Permet d'écrire du texte en <b>NOIR</b>
* @param[in] texte		Le texte a écrire en <b>NOIR</b>
* @param[in] police		La police a utiliser
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*
* @note Cette fonction est un alias de moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur, TTF_Font* police )
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], TTF_Font* police ) const
{
	static SDL_Color couleurNoire = {0,0,0,0/*unused*/};
	return ecritTexte( texte, police, couleurNoire );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur, TTF_Font* police ) const
* @brief Permet d'écrire du texte
* @param[in] texte		Le texte a écrire
* @param[in] police		La police a utiliser
* @param[in] couleur	La couleur a utiliser
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], TTF_Font* police, const SDL_Color& couleur ) const
{
	SDL_Surface* tmp = TTF_RenderText_Blended(police, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la création du texte <%s>, Couleur: {%u, %u, %u}, Erreur renvoyée: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

	return tmp;
}


/***************************************************************************//*!
* @fn char* moteur_sdl::completerMot( char texte[], unsigned int taille )
* @brief Complete le texte avec le caractère _ jusqu'a avoir un texte de longueur 20
* @param[in,out] texte Le texte a combler
* @param[in] taille Taille réel du texte ( sans les _ )
* @return La variable : texte
*/
char* moteur_sdl::completerMot( char texte[], unsigned int taille )
{
	if( taille == 20 )
		return texte;

	for( unsigned int i=taille; i<20; i++ )
	{
		texte[i] = '_';
	}
	return texte;
}



/***************************************************************************//*!
* @fn int moteur_sdl::isInSpriteList( s_Coordonnees pos ) const
* @brief Complete le texte avec le caractère _ jusqu'a avoir un texte de longueur 20
* @return Si trouvé, alors renvoie la position dans la liste. Sinon -1
*/
int moteur_sdl::isInSpriteList( s_Coordonnees pos ) const
{
	unsigned int i;
	for( i=0; i<c_ListSprite.size(); i++ )
	{
		if( coordonneeEgal(c_ListSprite.at(i).pos, pos) )
			return i;
	}
	return -1;
}


/***************************************************************************//*!
* @fn void moteur_sdl::joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos ) const
* @brief Blit le joueur {joueur} a la position {pos} avec l'orientation {ori}
* @param[in] ori L'orientation du perso
* @param[in] joueur L'e noméro du joueur a blitter [1-...]
* @param[in] pos La position où blitter le perso
*/
void moteur_sdl::joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos ) const
{
	switch( ori )
	{
		case perso::ORI_haut: {
			switch( joueur )
			{
				case 1:
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_haut], NULL, c_Instance->c_ecranGeneral, pos);
					return ;
				case 2:
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_haut], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 3:
					SDL_BlitSurface(c_Instance->c_Decor[joueur3_haut], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 4:
					SDL_BlitSurface(c_Instance->c_Decor[joueur4_haut], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				default:
					stdError("Cas(%d) NON gere !", (unsigned int)joueur);
			}
			return ;
		}

		case perso::ORI_bas: {
			switch( joueur )
			{
				case 1:
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_bas], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_bas], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 3:
					SDL_BlitSurface(c_Instance->c_Decor[joueur3_bas], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 4:
					SDL_BlitSurface(c_Instance->c_Decor[joueur4_bas], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				default:
					stdError("Cas(%d) NON gere !", (unsigned int)joueur);
			}
			return ;
		}

		case perso::ORI_droite: {
			switch( joueur )
			{
				case 1:
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_droite], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_droite], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 3:
					SDL_BlitSurface(c_Instance->c_Decor[joueur3_droite], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 4:
					SDL_BlitSurface(c_Instance->c_Decor[joueur4_droite], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				default:
					stdError("Cas(%d) NON gere !", (unsigned int)joueur);
			}
			return ;
		}

		case perso::ORI_gauche: {
			switch( joueur )
			{
				case 1:
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_gauche], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 2:
					SDL_BlitSurface(c_Instance->c_Decor[joueur2_gauche], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 3:
					SDL_BlitSurface(c_Instance->c_Decor[joueur3_gauche], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				case 4:
					SDL_BlitSurface(c_Instance->c_Decor[joueur4_gauche], NULL, c_Instance->c_ecranGeneral, pos);
					return;
				default:
					stdError("Cas(%d) NON gere !", (unsigned int)joueur);
			}
			return ;
		}

		default:
			stdError("Cas NON gere !");
			return ;
	}
}
