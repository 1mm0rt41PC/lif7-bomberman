#ifndef PARTIE_h
#define PARTIE_h

#include "options.h"
#include "perso.h"
#include "map.h"

extern options G_OPTIONS;// Variable globale

class partie
{
	public:
		enum MODE {
			Combat_Libre,		// Free For All 	( F4A )
			Attrape_drapeau		// Capture the Flag	( CTF )
		};

		typedef SYS_CLAVIER(libAff)(const partie*);


	private:
		// struct {
			map* 			c_map;// SIMPLE POINTEUR !
			unsigned char	c_nb_joueurs;
			unsigned char	c_nb_MAX_joueurs;
			perso*			c_joueurs;// Tableau
			MODE			c_mode;
		// }

		void deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur );


	public:
		partie();
		~partie();
		// Modificateurs
		void genMap();
		void def_nbJoueurs( unsigned char nb );
		void def_nbMAX_joueurs( unsigned char nb );
		void def_modeJeu( MODE m );


		// Accesseurs
		unsigned char nbJoueurs() const;
		unsigned char nbMAX_joueurs() const;
		MODE modeJeu() const;
		perso* joueur( unsigned int joueur_numero ) const;
		map* getMap() const;
		unsigned char nbJoueurVivant() const;

		// Autres
		void main(libAff * afficherMapEtEvent);
};

#endif
