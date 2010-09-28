#ifndef MAP_h
#define MAP_h

#define TAILLE_BLOCK 8

class map
{
	private:
		// struct map {
		char *c_block;
		unsigned int c_tailleX;
		unsigned int c_tailleY;
		//}

	public:
		// A quoi correspond un block
		enum type {
			vide,
			// MUR
			Mur_destrucible,
			Mur_INdestrucible,
			// Armes
			flamme,
			bombe_poser,
			// Bonus
			gain_bombe,
			gain_puissance_flamme,
			gain_declancheur_manuel,
			// Player
			UN_joueur,
			plusieurs_joueurs,
			// Combinaison
			bombe_poser_AVEC_UN_joueur,
			bombe_poser_AVEC_plusieurs_joueurs
		};

		map();
		map(unsigned int tailleX, unsigned int tailleY);
		~map();
		char getBlock(unsigned int X, unsigned int Y);
};

#endif

