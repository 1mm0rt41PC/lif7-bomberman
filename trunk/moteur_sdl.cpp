#include "moteur_sdl.h"

/***************************************************************************//*!
* @fn moteur_sdl::moteur_sdl()
* @brief Initialise la class moteur_sdl
*
* Initialise l'écran sdl et prépare le "terrain" pour les traitements futurs
*/
moteur_sdl::moteur_sdl()
{
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 ) // Démarrage de la SDL. Si erreur alors...
		stdErrorVarE("Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Ecriture de l'erreur

	if(TTF_Init() == -1)
		stdErrorVarE("Erreur d'initialisation de TTF_Init : %s", TTF_GetError());

	c_ecranGeneral = SDL_SetVideoMode(1024,768, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_FillRect(c_ecranGeneral, NULL, SDL_MapRGB(c_ecranGeneral->format, 255, 255, 255));/* Efface l'écran */

	//SDL_ShowCursor(SDL_DISABLE);// Masque le curseur de la souris

	SDL_WM_SetCaption("Bomberman", NULL);// Titre de la fenêtre

	if( !(c_policeGeneral = TTF_OpenFont("Chicken Butt.ttf", 75)) )
		stdErrorVarE("Erreur lors du chargement de la police : %s", TTF_GetError());

	if( !(c_background = IMG_Load("images/background.png")) )
		stdErrorVarE("Erreur lors du chargement du background : %s", SDL_GetError());

	SDL_EnableUNICODE(1);// On veux pas un clavier QWERTY
}


/***************************************************************************//*!
* @fn moteur_sdl::~moteur_sdl()
* @brief Désinitialise la class moteur_sdl
*/
moteur_sdl::~moteur_sdl()
{
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
	SDL_Color couleurNoire = {0, 0, 0};
	SDL_Color couleurOrange = {255, 184, 0};
	SDL_Rect position;
	SDL_Surface** textes;
	unsigned int i;
	bool dessiner = 1;

	textes = new SDL_Surface*[nb_choix*2];

	for( i=0; i<nb_choix; i++ )/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
		textes[i] = TTF_RenderText_Blended(c_policeGeneral, choix[i], couleurNoire);

	// Création du texte HighLight
	for( i=nb_choix; i<nb_choix*2; i++ )/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
		textes[i] = TTF_RenderText_Blended(c_policeGeneral, choix[i-nb_choix], couleurOrange);

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

			position.y = 180;// Point de départ
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
	SDL_Color couleurNoire = {0, 0, 0};
	SDL_Color couleurOrange = {255, 184, 0};
	SDL_Color couleurRouge = {255, 0, 0};
	SDL_Rect position;
	SDL_Surface* sfr_titre;
	SDL_Surface* sfr_msg;
	SDL_Surface** textes;
	unsigned int i;
	bool dessiner = 1;
	char valeurRetourTexte[20];// <- Valable 32bit ONLY ! Calcule :: lenght(2^(sizeof(int)*8))+4*2

	*valeurRetour = valeurParDefaut;

	sprintf(valeurRetourTexte, "<-- %d -->", *valeurRetour);

	sfr_titre = TTF_RenderText_Blended(c_policeGeneral, titre, couleurNoire);

	textes = new SDL_Surface*[3*2];

	/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
	textes[0] = TTF_RenderText_Blended(c_policeGeneral, valeurRetourTexte, couleurNoire);
	textes[1] = TTF_RenderText_Blended(c_policeGeneral, "Ok", couleurNoire);
	textes[2] = TTF_RenderText_Blended(c_policeGeneral, "Retour", couleurNoire);

	// Création du texte HighLight
	textes[3] = TTF_RenderText_Blended(c_policeGeneral, valeurRetourTexte, couleurOrange);
	textes[4] = TTF_RenderText_Blended(c_policeGeneral, "Ok", couleurOrange);
	textes[5] = TTF_RenderText_Blended(c_policeGeneral, "Retour", couleurOrange);

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
							sfr_msg = TTF_RenderText_Blended(c_policeGeneral, "Entrez le nombre manuellement", couleurRouge);
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
									textes[0] = TTF_RenderText_Blended(c_policeGeneral, valeurRetourTexte, couleurNoire);
									textes[3] = TTF_RenderText_Blended(c_policeGeneral, valeurRetourTexte, couleurRouge);

									position.x = 0;
									position.y = 0;
									SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background

									position.y = 180;
									position.x = (c_ecranGeneral->w-sfr_msg->w)/2;// Centrage auto des blit ^^
									SDL_BlitSurface(sfr_msg, NULL, c_ecranGeneral, &position);// Blit background

									position.y = 180;// Point de départ
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
							textes[3] = TTF_RenderText_Blended(c_policeGeneral, valeurRetourTexte, couleurOrange);
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

			position.y = 180;// Point de départ
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

	return 0;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER moteur_sdl::afficherMapEtEvent( const partie* p )
* @brief Affiche une map
* @param[in] p	La partie en cours a afficher
* @return La touche actuellement appuyé
*/
SYS_CLAVIER moteur_sdl::afficherMapEtEvent( const partie* p )
{


	return SDLK_LAST;
}


/***************************************************************************//*!
* @fn SDLKey moteur_sdl::traductionClavier( SDL_KeyboardEvent* touche )
* @brief Permet l'uitlisation d'un clavier unicode en toute simplicité
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
