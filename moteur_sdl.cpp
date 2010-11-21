#include "moteur_sdl.h"

/***************************************************************************//*!
* @fn moteur_sdl::moteur_sdl()
* @brief Initialise la class moteur_sdl
*
* Initialise l'�cran sdl et pr�pare le "terrain" pour les traitements futurs
*/
moteur_sdl::moteur_sdl()
{
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 ) // D�marrage de la SDL. Si erreur alors...
		stdErrorVarE("Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Ecriture de l'erreur

	if(TTF_Init() == -1)
		stdErrorVarE("Erreur d'initialisation de TTF_Init : %s", TTF_GetError());

	c_ecranGeneral = SDL_SetVideoMode(1024,768, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_FillRect(c_ecranGeneral, NULL, SDL_MapRGB(c_ecranGeneral->format, 255, 255, 255));/* Efface l'�cran */

	//SDL_ShowCursor(SDL_DISABLE);// Masque le curseur de la souris

	SDL_WM_SetCaption("Bomberman", NULL);// Titre de la fen�tre

	if( !(c_policeGeneral = TTF_OpenFont("Chicken Butt.ttf", 75)) )
		stdErrorVarE("Erreur lors du chargement de la police : %s", TTF_GetError());

	if( !(c_background = IMG_Load("images/background.png")) )
		stdErrorVarE("Erreur lors du chargement du background : %s", SDL_GetError());
}


/***************************************************************************//*!
* @fn moteur_sdl::~moteur_sdl()
* @brief D�sinitialise la class moteur_sdl
*/
moteur_sdl::~moteur_sdl()
{
	SDL_FreeSurface(c_background);// On lib�re le background

	TTF_CloseFont(c_policeGeneral);/* Fermeture de la police avant TTF_Quit */
	TTF_Quit();/* Arr�t de SDL_ttf (peut �tre avant ou apr�s SDL_Quit, peu importe) */
	SDL_Quit();
}


/***************************************************************************//*!
* @fn unsigned int moteur_sdl::menu( const char titre[], const char *choix[], unsigned int nb_choix )
* @brief Afficher l'image correspondante suivant le menu o� on est
* @param[in] titre		Titre de la sous fen�tre
* @param[in] choix		Liste des choix a mettre dans la sous fen�tre. ( Tableau de chaine de caract�res )
* @param[in] nb_choix	Nombre d'�lement dans la liste des choix
* @return Le choix selectionn� (sous forme d'un nombre: le i�me element de la liste a �t� selectionn�)
*
* La sous fen�tre propose de naviguer parmis une liste ( @a choix )<br />
* Puis renvoie le choix (sous forme d'un nombre: i�me element de la liste)<br />
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
	SDL_Color couleurRouge = {255, 184, 0};
	SDL_Rect position;
	SDL_Surface** textes;
	unsigned int i;

	textes = new SDL_Surface*[nb_choix*2];

	for( i=0; i<nb_choix; i++ )/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
		textes[i] = TTF_RenderText_Blended(c_policeGeneral, choix[i], couleurNoire);

	// Cr�ation du texte HighLight
	for( i=nb_choix; i<nb_choix*2; i++ )/* Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal) */
		textes[i] = TTF_RenderText_Blended(c_policeGeneral, choix[i-nb_choix], couleurRouge);

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
					case SDLK_UP: { /* Appui sur la touche Echap, on arr�te le programme */
						if( highLight == 1 ){// On bouge notre highLight vers le haut
							highLight = nb_choix;
						}else{
							highLight--;
						}
						break;
					}
					case SDLK_DOWN: {// On bouge notre highLight vers le bas
						if( highLight == nb_choix ){
							highLight = 1;
						}else{
							highLight++;
						}
						break;
					}
					case SDLK_RETURN: {// On a valider notre choix par entrer
						continuer = 0;
						break;
					}
					case SDLK_ESCAPE: { /* Appui sur la touche Echap, on arr�te le programme */
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

		SDL_FillRect(c_ecranGeneral, NULL, SDL_MapRGB(c_ecranGeneral->format, 255, 255, 255));

		position.x = 0;
		position.y = 0;
		SDL_BlitSurface(c_background, NULL, c_ecranGeneral, &position);// Blit background


		position.y = 180;// Point de d�part
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
	}

	for( i=0; i<nb_choix*2; i++ )// On lib�re les textes
		SDL_FreeSurface(textes[i]);
	delete[] textes;// On lib�re les textes

	return highLight;
}


/***************************************************************************//*!
* @fn void moteur_sdl::afficherConfigurationClavier( unsigned char joueur )
* @brief Menu permettant d'afficher et de modifier la configuration du clavier d'un joueur
* @param[in] joueur		Le num�ro du joueur ( de 1 � ... )
*/
void moteur_sdl::afficherConfigurationClavier( unsigned char joueur )
{

}


/***************************************************************************//*!
* @fn int moteur_sdl::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* returnValue )
* @brief Cr�er un menu pour r�cup�rer un nombre entr� par un utilisateur
* @param[in] titre			 Le titre du menu
* @param[in] valeurParDefaut La valeur par d�faut
* @param[in] valeurMin		 La valeur minimum
* @param[in] valeurMax		 La valeur maximum
* @param[out] returnValue	 Dans cette variable sera stock�, le nombre obtenu a la fin de la fonction
* @return
*	- 2 : Nombre valid� et acc�pt�
*	- 3 : Action annul�e
*/
int moteur_sdl::getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* returnValue )
{

	return 0;
}


/***************************************************************************//*!
* @fn int moteur_sdl::getTexte( const char titre[], char texteRetour[21] )
* @brief Permet d'obtenir du texte
* @param[in] titre				Le titre du menu
* @param[out] texteRetour		Dans cette variable sera stock�, le texte obtenu a la fin de la fonction
* @return
*	- 2 : Texte valid� et acc�pt�
*	- 3 : Action annul�e
*/
int moteur_sdl::getTexte( const char titre[], char texteRetour[21] )
{

	return 0;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER moteur_sdl::afficherMapEtEvent( const partie* p )
* @brief Affiche une map
* @param[in] p	La partie en cours a afficher
* @return La touche actuellement appuy�
*/
SYS_CLAVIER moteur_sdl::afficherMapEtEvent( const partie* p )
{


	return SDLK_LAST;
}
