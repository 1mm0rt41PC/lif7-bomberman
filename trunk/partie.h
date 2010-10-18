#ifndef PARTIE_h
#define PARTIE_h
#include "perso.h"
#include "map.h"
#include "moteur_ncurses.h"

class partie
{
	public:
		enum MODE {
			Combat_Libre,		// Free For All 	( F4A )
			Attrape_drapeau		// Capture the Flag	( CTF )
		};


	private:
		// struct {
			map* 			c_map;// SIMPLE POINTEUR !
			unsigned char	c_nb_joueurs;
			unsigned char	c_nb_MAX_joueurs;
			perso*			c_joueurs;// Tableau
			MODE			c_mode;
		// }


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
};

#endif
