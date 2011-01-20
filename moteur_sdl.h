#ifndef MOTEUR_SDL_h
#define MOTEUR_SDL_h

#include "config.h"
#include "options.h"
#include "partie.h"
#include "outils.h"
#include "math.h"
#include <string.h>// Pour strlen()
#include <time.h>// Pour clock() et clock_t


/*!
* @class moteur_sdl
* @brief Gère l'affichage via le moteur graphique SDL
*/

class moteur_sdl
{
	private:
		/*!
		* @enum t_obj
		* @brief Liste des images (Utilisé pour le tableau c_Decor)
		*/
		typedef enum {
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
			flamme_pointe_haut,
			bout_flamme_bas,
			bout_flamme_gauche,
			bout_flamme_droite,
			bombe,
			// Bonus
			gain_bombe,
			gain_declancheur,
			gain_puissance_flamme,
			gain_pousse_bombe,
			gain_vie,

			__nombre_de_decors__
		} t_obj;


		// Liste des vitesses des sprites
		enum {
			VitesseSpriteBombe = CLOCKS_PER_SEC/6
		};

		/*!
		* @struct Sprite
		* @brief Contient les données pour faire un Sprite.
		*/
		typedef struct {
			s_Coordonnees	pos;
			t_obj			objet;
			unsigned char	frame;
			clock_t			refresh;
		} Sprite;


		typedef struct {
			unsigned int nb_bombe_ramasser;
			unsigned int nb_puissance_de_flamme;

			unsigned int nb_bombe_ramasser_TOTAL;
			unsigned int nb_puissance_de_flamme_TOTAL;

			unsigned int nb_de_block_detruit;
			unsigned int nb_de_fois_tuer_par_J1;
			unsigned int nb_de_fois_tuer_par_J2;
			unsigned int nb_de_fois_tuer_par_J3;
			unsigned int nb_de_fois_tuer_par_J4;
			unsigned int nb_de_fois_que_vous_avez_tuer_J1;
			unsigned int nb_de_fois_que_vous_avez_tuer_J2;
			unsigned int nb_de_fois_que_vous_avez_tuer_J3;
			unsigned int nb_de_fois_que_vous_avez_tuer_J4;
		} sStatistique;


	private:
		static moteur_sdl* c_Instance;
		std::vector<sStatistique>	c_ListStatistique;
		std::vector<Sprite>			c_ListSprite;
		SDL_Surface*				c_ecranGeneral;
		SDL_Surface*				c_background;
		TTF_Font*					c_policeGeneral;
		SDL_Surface**				c_Decor;
		bool						c_premierAffichage;
		bool						c_fenetreOuverte;

	private:
		SDL_Surface* chargerImage( const char image[] ) const;
		SDL_Surface* ecritTexte( const char texte[] ) const;
		SDL_Surface* ecritTexte( const char texte[], const SDL_Color& couleur ) const;
		SDL_Surface* ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b ) const;
		SDL_Surface* ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille ) const;
		SDL_Surface* ecritTexte( const char texte[], TTF_Font* police ) const;
		SDL_Surface* ecritTexte( const char texte[], TTF_Font* police, const SDL_Color& couleur ) const;
		void joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos ) const;

		static char* completerMot( char texte[], unsigned int taille );

		int isInSpriteList( s_Coordonnees pos ) const;

		moteur_sdl();

	public:
		static moteur_sdl& getInstance();
		~moteur_sdl();

		// Fonction d'affichage de menu
		unsigned int menu( const char titre[], const char *choix[], unsigned int nb_choix );
		void afficherConfigurationClavier( unsigned char joueur );
		int getNombre( const char titre[], int valeurParDefaut, int valeurMin, int valeurMax, int* valeurRetour );
		int getTexte( const char titre[], char texteRetour[21] );
		int afficherGagnant( const partie* p );
		inline bool isWindowOpen(){ return c_fenetreOuverte; }

		// Modificateur
		inline void forcerRafraichissement() { c_premierAffichage = 1; c_ListSprite.clear(); }

		// Autre :: Affichage
		static SYS_CLAVIER afficherMapEtEvent( partie* p );

		// Clavier
		static SDLKey traductionClavier( const SDL_KeyboardEvent* touche );
};

#endif
