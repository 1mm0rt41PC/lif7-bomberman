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

	SDL_EnableUNICODE(1);// On veux pas un clavier QWERTY

	/***************************************************************************
	* On charge ici le décor
	*/
	c_nb_Decor = 30;
	c_Decor = new SDL_Surface*[c_nb_Decor];
	for( unsigned int i=0; i<c_nb_Decor; i++ )// POUR EVVITER LES BUG EN ATTANDANT LES AUTRES IMAGES
	{
		c_Decor[i]=0;
	}
	//murs
	c_Decor[vide] = chargerImage("images/vide.png");
	c_Decor[mur_destructible]= chargerImage("images/mur_destructible.png");
	c_Decor[mur_indestructible]= chargerImage("images/mur_indestructible.png");
	//joueur1
	c_Decor[joueur1_haut]= chargerImage("images/bomberman1_haut.png");
	c_Decor[joueur1_bas]= chargerImage("images/bomberman1_bas.png");
	c_Decor[joueur1_gauche]= chargerImage("images/bomberman1_gauche.png");
	c_Decor[joueur1_droite]= chargerImage("images/bomberman1_droite.png");
//	//joueur2
//	c_Decor[joueur2_haut]= chargerImage("images/bomberman2_haut.png");
//	c_Decor[joueur2_bas]= chargerImage("images/bomberman2_bas.png");
//	c_Decor[joueur2_gauche]= chargerImage("images/bomberman2_gauche.png");
//	c_Decor[joueur2_droite]= chargerImage("images/bomberman2_droite.png");
//	//joueur3
//	c_Decor[joueur3_haut]= chargerImage("images/bomberman3_haut.png");
//	c_Decor[joueur3_bas]= chargerImage("images/bomberman3_bas.png");
//	c_Decor[joueur3_gauche]= chargerImage("images/bomberman3_gauche.png");
//	c_Decor[joueur3_droite]= chargerImage("images/bomberman3_droite.png");
//	//joueur4
//	c_Decor[joueur4_haut]= chargerImage("images/bomberman4_haut.png");
//	c_Decor[joueur4_bas]= chargerImage("images/bomberman4_bas.png");
//	c_Decor[joueur4_gauche]= chargerImage("images/bomberman4_gauche.png");
//	c_Decor[joueur4_droite]= chargerImage("images/bomberman4_droite.png");
	//gain
//	c_Decor[gain_flamme]= chargerImage("images/gain_flamme.png");
//	c_Decor[gain_puissance_flamme]= chargerImage("images/gain_puissance_flamme.png");
//	c_Decor[gain_bombe]= chargerImage("images/gain_bombe.png");
	//armes
	c_Decor[flamme_origine]= chargerImage("images/flamme_origine.png");
	c_Decor[flamme_vertical]= chargerImage("images/milieu_flamme_verticale.png");
	c_Decor[flamme_horizontal]= chargerImage("images/milieu_flamme_horizontale.png");
	c_Decor[bout_flamme_haut]= chargerImage("images/flamme_haut.png");
	c_Decor[bout_flamme_bas]= chargerImage("images/flamme_bas.png");
	c_Decor[bout_flamme_gauche]= chargerImage("images/flamme_gauche.png");
	c_Decor[bout_flamme_droite]= chargerImage("images/flamme_droite.png");
	c_Decor[bombe]= chargerImage("images/bombe.gif");
//	c_Decor[bombe_explosion]= chargerImage("images/bombe_explosion.png");
}


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
	// FONCTIONNEL PARTIELLEMENT -> EN ATTANTE DES IMAGES MANQUANTES
	for( unsigned int i=0; i<c_nb_Decor; i++ )
	{
		if(c_Decor[i]) SDL_FreeSurface(c_Decor[i]);
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
				position.y+=100;// Espacement entre les textes ( blit )
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
	char valeurRetourTexte[20];// <- Valable 32bit ONLY ! Calcule :: lenght(2^(sizeof(int)*8))+4*2

	if( !(valeurMin <= valeurParDefaut && valeurParDefaut <= valeurMax) )
		stdErrorE("Valeur incorrect ! valeurMin(%d) <= valeurParDefaut(%d) && valeurParDefaut(%d) <= valeurMax(%d)", valeurMin, valeurParDefaut, valeurParDefaut, valeurMax);

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


	while( continuer )
	{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = 3;// On veux quitter !
				continuer = 0;
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
					case SDLK_RETURN: {// On a valider notre choix par entrer
						if( highLight == 1 ){// On a selectionné un lien de modification de texte
							dessiner = 1;
							sfr_msg = ecritTexte("Entrez le nombre manuellement", couleurRouge);
							do{
								SDL_WaitEvent(&event);
								if( event.type == SDL_QUIT ){
									continuer = 0;
									highLight = 3;
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
									case SDLK_BACKSPACE:{
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
										if( !(valeurMin <= *valeurRetour && *valeurRetour <= valeurMax) )
											*valeurRetour = valeurMin;
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
										position.y+=100;// Espacement entre les textes ( blit )
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
						highLight = 3;// On veux quitter !
						continuer = 0;
						break;
					}
					default:
						break;
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
			position.x = (c_ecranGeneral->w-sfr_titre->w)/2;// Centrage auto des blit ^^
			SDL_BlitSurface(sfr_titre, NULL, c_ecranGeneral, &position);// Blit background

			// On blit le texte
			for( i=0; i<3; i++ )
			{
				position.y+=100;// Espacement entre les textes ( blit )
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

	tailleTexteRetour = strlen(texteRetour);

	textes = new SDL_Surface*[3*2];

	// Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal)
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

	while( continuer )
	{
		SDL_WaitEvent(&event);
		switch( event.type )
		{
			case SDL_QUIT: {
				highLight = 3;// On veux quitter !
				continuer = 0;
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
					case SDLK_RETURN: {// On a valider notre choix par entrer
						if( highLight == 1 ){// On a selectionné un lien de modification de texte
							dessiner = 1;
							sfr_msg = ecritTexte("Entrez le texte manuellement", couleurRouge);
							textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
							do{
								SDL_WaitEvent(&event);
								if( event.type == SDL_QUIT ){
									continuer = 0;
									highLight = 3;
								}

								if( event.type == SDL_KEYDOWN )
								switch( traductionClavier(&event.key) )
								{
									case SDLK_BACKSPACE:{
										if( tailleTexteRetour == 0 )
											break;
										tailleTexteRetour--;
										texteRetour[tailleTexteRetour] = '_';
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_ESCAPE: {
										continuer = 0;
										dessiner = 1;// On redessine
										break;
									}
									case SDLK_RETURN: {
										continuer = 0;
										break;
									}
									default: {
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
										break;
									}
								}

								/***********************************************************************
								* Blitage général
								* > On blit uniquement s'il y a eu une modification
								*/
								if( dessiner ){
									SDL_FreeSurface(textes[0]);
									SDL_FreeSurface(textes[3]);
									textes[0] = ecritTexte(texteRetour);
									textes[3] = ecritTexte(texteRetour, couleurRouge);

									position.x = 0;
									position.y = 0;
									SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

									position.y = 200;
									position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
									SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background

									// On blit le texte
									for( i=0; i<3; i++ )
									{
										position.y+=100;// Espacement entre les textes ( blit )
										position.x = (c_ecranGeneral->w-textes[i]->w)/2;// Centrage auto des blit ^^
										if( i == highLight-1 ){
											SDL_BlitSurface(textes[i+3], NULL, c_ecranGeneral, &position); // Blit du texte par-dessus
										}else{
											SDL_BlitSurface(textes[i], NULL, c_ecranGeneral, &position); // Blit du texte par-dessus
										}
									}

									SDL_Flip(c_ecranGeneral);
									dessiner = 0;
								}
							}while( continuer );
							SDL_FreeSurface(sfr_msg);
							sfr_msg = 0;
							SDL_FreeSurface(textes[0]);
							SDL_FreeSurface(textes[3]);
							// On vire les espaces
							if( tailleTexteRetour ){
								texteRetour[tailleTexteRetour] = 0;
								trimString(texteRetour);
								tailleTexteRetour = strlen(texteRetour);
							}
							if( tailleTexteRetour ){
								textes[0] = ecritTexte(texteRetour);
								textes[3] = ecritTexte(texteRetour, couleurOrange);
								highLight = 2;
							}else{
								textes[0] = ecritTexte(completerMot(texteRetour, tailleTexteRetour));
								textes[3] = ecritTexte(completerMot(texteRetour, tailleTexteRetour), couleurOrange);
							}
							dessiner = 1;
							continuer = 1;
						}else{
							if( highLight == 2 ){
								if( tailleTexteRetour ){
									continuer = 0;// On sort de la boucle
								}else{
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
						highLight = 3;// On veux quitter !
						continuer = 0;
						break;
					}
					default:
						break;
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
				position.y+=100;// Espacement entre les textes ( blit )
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

	if( sfr_msg )
		SDL_FreeSurface(sfr_msg);

	return highLight;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER moteur_sdl::afficherMapEtEvent( const partie* p )
* @brief Affiche une map
* @param[in] p	La partie en cours a afficher
* @return La touche actuellement appuyé
*/
SYS_CLAVIER moteur_sdl::afficherMapEtEvent( const partie* p )
{
	if( !c_Instance )
		stdErrorE("Veuillez instancier la class moteur_sdl !");

	bool dessiner = 0;
	map* l_map = p->getMap();// l_map pour local variable map
	// Décalage
	static unsigned int	xpos=(((c_Instance->c_ecranGeneral->w/32)-l_map->X())/2)*32,
						ypos=(((c_Instance->c_ecranGeneral->h/32)-l_map->Y())/2)*32;
	SDL_Rect pos;

	// Tester ici si la taille de la map n'est pas trop grande pour la fenetre.
	if ( l_map->X() > (unsigned int)c_Instance->c_ecranGeneral->w/32 || l_map->Y() > (unsigned int)c_Instance->c_ecranGeneral->h/32 )
		stdErrorE("%u > %d || %u > %d", l_map->X(), c_Instance->c_ecranGeneral->w/32, l_map->Y(), c_Instance->c_ecranGeneral->h/32);

	if( c_Instance->c_premierAffichage ){
		// Traitement a faire sur les cadres ICI
		// A FAIRE

		for( unsigned int x,y=0; y<l_map->Y(); y++ )
		{
			for( x=0; x<l_map->X(); x++ )
			{
				pos.x=x*32+xpos;
				pos.y=y*32+ypos;
				switch( l_map->getBlock(x,y)->element )
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
					case map::UN_joueur:
					case map::plusieurs_joueurs: {
						if( !l_map->getBlock(x,y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y)->joueur=0", x, y);

						//couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						switch( p->joueur(l_map->getBlock(x,y)->joueur->at(0)-1)->orientation() )
						{
							case perso::ORI_haut: {
								SDL_BlitSurface(c_Instance->c_Decor[joueur1_haut], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case perso::ORI_bas: {
								SDL_BlitSurface(c_Instance->c_Decor[joueur1_bas], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case perso::ORI_droite: {
								SDL_BlitSurface(c_Instance->c_Decor[joueur1_droite], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
							case perso::ORI_gauche: {
								SDL_BlitSurface(c_Instance->c_Decor[joueur1_gauche], NULL, c_Instance->c_ecranGeneral, &pos);
								break;
							}
						}
						break;
					}
					case map::bombe_poser: {
						//couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[bombe], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::bombe_poser_AVEC_UN_joueur: {
						if( !l_map->getBlock(x,y)->joueur )
							stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(x,y).joueur=0", x, y);

						//couleur = getCouleurJoueur( l_map->getBlock(x,y)->joueur->at(0) );
						SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[bombe], NULL, c_Instance->c_ecranGeneral, &pos);
						SDL_BlitSurface(c_Instance->c_Decor[joueur1_bas], NULL, c_Instance->c_ecranGeneral, &pos);

						break;
					}
					case map::flamme_origine: {
						SDL_BlitSurface(c_Instance->c_Decor[flamme_origine], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_horizontal:{
							SDL_BlitSurface(c_Instance->c_Decor[flamme_horizontal], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
					}
					case map::flamme_vertical: {
						SDL_BlitSurface(c_Instance->c_Decor[flamme_vertical], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
					}
					case map::flamme_pointe_haut: {
						SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_haut], NULL, c_Instance->c_ecranGeneral, &pos);
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
					default: {
						stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(x,y)->element);
						break;
					}
				}
			}
		}
		c_Instance->c_premierAffichage = false;
		SDL_Flip(c_Instance->c_ecranGeneral);// afficher la map
	}else{
		s_Coordonnees v_pos;
		while( l_map->getModification(v_pos) )
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
				case map::UN_joueur:
				case map::plusieurs_joueurs: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos)->joueur=0", v_pos.x, v_pos.y);

					//couleur = getCouleurJoueur( l_map->getBlock(v_pos)->joueur->at(0) );
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					switch( p->joueur(l_map->getBlock(v_pos)->joueur->at(0)-1)->orientation() )
					{
						case perso::ORI_haut: {
							SDL_BlitSurface(c_Instance->c_Decor[joueur1_haut], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case perso::ORI_bas: {
							SDL_BlitSurface(c_Instance->c_Decor[joueur1_bas], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case perso::ORI_droite: {
							SDL_BlitSurface(c_Instance->c_Decor[joueur1_droite], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
						case perso::ORI_gauche: {
							SDL_BlitSurface(c_Instance->c_Decor[joueur1_gauche], NULL, c_Instance->c_ecranGeneral, &pos);
							break;
						}
					}
					break;
				}
				case map::bombe_poser: {
					//couleur = getCouleurJoueur( l_map->getBlock(v_pos)->joueur->at(0) );
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::bombe_poser_AVEC_UN_joueur: {
					if( !l_map->getBlock(v_pos)->joueur )
						stdErrorE("POINTEUR NULL !X=%u, Y=%u, l_map->getBlock(v_pos).joueur=0", v_pos.x, v_pos.y);

					//couleur = getCouleurJoueur( l_map->getBlock(v_pos)->joueur->at(0) );
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[bombe], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[joueur1_bas], NULL, c_Instance->c_ecranGeneral, &pos);

					break;
				}
				case map::flamme_origine: {
					SDL_BlitSurface(c_Instance->c_Decor[vide], NULL, c_Instance->c_ecranGeneral, &pos);
					SDL_BlitSurface(c_Instance->c_Decor[flamme_origine], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_horizontal:{
						SDL_BlitSurface(c_Instance->c_Decor[flamme_horizontal], NULL, c_Instance->c_ecranGeneral, &pos);
						break;
				}
				case map::flamme_vertical: {
					SDL_BlitSurface(c_Instance->c_Decor[flamme_vertical], NULL, c_Instance->c_ecranGeneral, &pos);
					break;
				}
				case map::flamme_pointe_haut: {
					SDL_BlitSurface(c_Instance->c_Decor[bout_flamme_haut], NULL, c_Instance->c_ecranGeneral, &pos);
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
				default: {
					stdError("Erreur lors de la lecture de la map, Objet inconu <%d>", (int)l_map->getBlock(v_pos)->element);
					break;
				}
			}
			SDL_UpdateRect(c_Instance->c_ecranGeneral, pos.x-32, pos.y-32, 32*3, 32*3);
		}
	}

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
* @fn SDL_Surface* moteur_sdl::chargerImage( const char image[] )
* @brief Permet de charger une image.
* @param[in] image L'image a charger
* @return Une surface (contenant l'image) a libérer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::chargerImage( const char image[] )
{
	SDL_Surface* tmp = IMG_Load(image);
	if( !tmp )
		stdErrorE("Erreur lors du chargement de l'image <%s> : %s", image, SDL_GetError());
	return tmp;
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[] )
* @brief Permet d'écrire du texte en <b>NOIR</b>
* @param[in] texte Le texte a écrire en noir
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*
* Cette fonction est un alias de moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
* @see moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[] )
{
	static SDL_Color couleurNoire = {0,0,0,0/*unused*/};
	return ecritTexte( texte, couleurNoire );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b )
* @brief Permet d'écrire du texte dans une couleur RBG
* @param[in] texte Le texte a écrire
* @param[in] r,g,b Le taux d'utilisation de chaque couleurs ( de 0 à 255 )
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*
* Cette fonction est un alias de moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
* @see moteur_sdl::ecritTexte( const char texte[], const SDL_Color* couleur )
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b )
{
	SDL_Color couleur = {r, g, b, 0/*unused*/};
	return ecritTexte( texte, couleur );
}


/***************************************************************************//*!
* @fn SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur )
* @brief Permet d'écrire du texte dans une couleur
* @param[in] texte Le texte a écrire
* @param[in] couleur La couleur du texte
* @return Une surface (contenant le texte) a libérer avec SDL_FreeSurface()
*/
SDL_Surface* moteur_sdl::ecritTexte( const char texte[], const SDL_Color& couleur )
{
	SDL_Surface* tmp = TTF_RenderText_Blended(c_policeGeneral, texte, couleur);
	if( !tmp )
		stdErrorE("Erreur lors de la création du texte <%s>, Couleur:{%u, %u, %u}, Erreur renvoyée: %s", texte, (unsigned int)couleur.r, (unsigned int)couleur.g, (unsigned int)couleur.b, TTF_GetError());

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
