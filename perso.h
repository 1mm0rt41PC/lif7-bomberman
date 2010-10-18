#ifndef PERSO_h
#define PERSO_h
#include <string>
#include "clavier.h"

class perso
{
	private:
		// struct {
		std::string c_nom;
		unsigned int c_X_pos;
		unsigned int c_Y_pos;
		bool c_est_vivant;
		clavier c_Touches;
		// }

	public:
		// Init & UnInit
		perso();
		perso(std::string nom, unsigned int Xpos, unsigned int Ypos);
		~perso();
		
		// Modificateurs
		void defPos(unsigned int Xpos, unsigned int Ypos);
		void defVivant( bool v );
		
		// Accesseurs
		unsigned int X();
		unsigned int Y();
		bool estVivant();
		bool estAppuyer( SYS_CLAVIER t );
};

#endif
