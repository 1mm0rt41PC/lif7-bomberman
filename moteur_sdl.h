#ifndef MOTEUR_SDL_h
#define MOTEUR_SDL_h

#include "config.h"
#include "options.h"
#include "partie.h"
#include "outils.h"
#include "math.h"
#include <string.h>// Pour strlen()


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
			gain_super_bombe,
			gain_declancheur,
			gain_puissance_flamme,
			gain_super_puissance_flamme,
			gain_pousse_bombe,
			gain_vie,
			bonus_teleporteur,
			bonus_inversseur_touche,
			bonus_force_explosion,

			__nombre_de_decors__
		} t_obj;


		// Liste des vitesses des sprites
		enum {
			VitesseSpriteBombe	= 200*1000,
			VitessePerso		= 500*1000
		};
		// Divers
		enum {
			Marge_menu = 300
		};

		/*!
		* @struct Sprite
		* @brief Contient les données pour faire un Sprite.
		*/
		typedef struct {
			s_Coordonnees	pos;
			t_obj			objet;
			unsigned char	frame;
			s_timeval		refresh;
		} Sprite;


	private:
		//struct {
			static moteur_sdl* c_Instance;
			std::vector<Sprite>			c_ListSprite;
			QList<Sprite>				c_ListSpriteJoueur;
			SDL_Surface*				c_ecranGeneral;
			SDL_Surface*				c_background;
			SDL_Surface*				c_backgroundLayer;
			SDL_Surface*				c_clock[12];
			TTF_Font*					c_policeTitre;
			TTF_Font*					c_policeGeneral;
			SDL_Surface*				c_Decor[__nombre_de_decors__];
			bool						c_premierAffichage;
			bool						c_fenetreOuverte;
			static SDL_Color			c_mainColor;
			static const SDL_Color			couleurBlanche;
			static const SDL_Color			couleurNoir;
			static const SDL_Color			couleurRouge;
			static const SDL_Color			couleurOrange;
		// }

	private:
		SDL_Surface* chargerImage( const char image[] ) const;
		inline SDL_Surface* ecritTexte( const char texte[] ) const;
		SDL_Surface* ecritTexte( const char texte[], const SDL_Color& couleur ) const;
		SDL_Surface* ecritTexte( const char texte[], Uint8 r, Uint8 g, Uint8 b ) const;
		SDL_Surface* ecritTexte( const char texte[], const SDL_Color& couleur, unsigned int taille ) const;
		inline SDL_Surface* ecritTexte( const char texte[], TTF_Font* police ) const;
		SDL_Surface* ecritTexte( const char texte[], TTF_Font* police, const SDL_Color& couleur ) const;
		void joueur_orientation( perso::t_Orientation ori, unsigned char joueur, SDL_Rect* pos, unsigned char frame ) const;

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
		int afficherGagnant( const partie* p, int retourPartie );
		inline bool isWindowOpen(){ return c_fenetreOuverte; }

		// Modificateur
		inline void forcerRafraichissement() { c_premierAffichage = 1; c_ListSprite.clear(); }

		// Autre :: Affichage
		static SYS_CLAVIER afficherMapEtEvent( partie* p );

		// Clavier
		static SDLKey traductionClavier( const SDL_KeyboardEvent* touche );
};

#endif
