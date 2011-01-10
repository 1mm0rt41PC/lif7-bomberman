#ifndef MOTEUR_SDL_h
#define MOTEUR_SDL_h

#include "config.h"
#include "options.h"
#include "partie.h"
#include "outils.h"
#include "math.h"
#include <string.h>// Pour strlen()
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

/*!
* @class moteur_sdl
* @brief Gère l'affichage via le moteur graphique SDL
*/

class moteur_sdl
{
	private:
		enum {
			vide=0,
			mur_destructible,
			mur_indestructible,
			joueur1_haut,
			joueur1_bas,
			joueur1_gauche,
			joueur1_droite,
			joueur2_haut,
			joueur2_bas,
			joueur2_gauche,
			joueur2_droite,
			joueur3_haut,
			joueur3_bas,
			joueur3_gauche,
			joueur3_droite,
			joueur4_haut,
			joueur4_bas,
			joueur4_gauche,
			joueur4_droite,
			flamme_origine,
			flamme_horizontal,
			flamme_vertical,
			bout_flamme_haut,
			bout_flamme_bas,
			bout_flamme_gauche,
			bout_flamme_droite,
			bombe,
			bombe_explosion,
			// Bonus
			gain_bombe,
			gain_declancheur,
			gain_puissance_flamme,
			gain_vitesse_vitesse,
			gain_vie,

			__nombre_de_decors__
		};


	private:
		static moteur_sdl* c_Instance;
		SDL_Surface* c_ecranGeneral;
		SDL_Surface* c_background;
		TTF_Font* c_policeGeneral;
		SDL_Surface** c_Decor;
		bool c_premierAffichage;

	private:
		SDL_Surface* chargerImage( const char image[] );
		SDL_Surface* ecritTexte( const char texte[] );
		SDL_Surface* ecritTexte( const char texte[], const SDL_Color& couleur );
		SDL_Surface* ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b );
		SDL_Surface* ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille );
		void joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos ) const;

		static char* completerMot( char texte[], unsigned int taille );

		moteur_sdl();

	public:
		static moteur_sdl& getInstance();
		~moteur_sdl();

		// Fonction d'affichage de menu
		unsigned int menu( const char titre[], const char *choix[], unsigned int nb_choix );
		void afficherConfigurationClavier( unsigned char joueur );
		int getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour );
		int getTexte( const char titre[], char texteRetour[21] );

		// Modificateur
		inline void forcerRafraichissement() { c_premierAffichage = 1; }

		// Autre :: Affichage
		static SYS_CLAVIER afficherMapEtEvent( partie* p );

		// Clavier
		static SDLKey traductionClavier( const SDL_KeyboardEvent* touche );
};

#endif
