#ifndef MOTEUR_SDL_h
#define MOTEUR_SDL_h

#include "config.h"
#include "options.h"
#include "partie.h"
#include <string.h>// Pour strlen()
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

/*!
* @class moteur_sdl
* @brief G�re l'affichage via le moteur graphique SDL
*/

class moteur_sdl
{
	private:
		SDL_Surface* c_ecranGeneral;
		SDL_Surface* c_background;
		TTF_Font *c_policeGeneral;

	public:
		moteur_sdl();
		~moteur_sdl();

		// Fonction d'affichage de menu
		unsigned int menu( const char titre[], const char *choix[], unsigned int nb_choix );
		void afficherConfigurationClavier( unsigned char joueur );
		int getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* returnValue );
		int getTexte( const char titre[], char texteRetour[21] );

		// Autre
		static SYS_CLAVIER afficherMapEtEvent( const partie* p );

		//static u