#ifndef PARTIE_h
#define PARTIE_h
#include "perso.h"
#include "map.h"
#include "moteur_ncurses.h"

class partie
{
	public:
		enum MODE {
			local,
			en_ligne
		};


	private:
		// struct {
			unsigned char	c_nb_joueurs;
			perso*			c_joueurs;// Tableau
			map* 			c_map;
			MODE			c_mode;// <- AF
		// }


	public:
		partie();
		~partie();


	private:


};

#endif
