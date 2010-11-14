#ifndef PERSO_h
#define PERSO_h

#include "bonus.h"
#include "debug.h"
#include <string>


/*!
* @class perso
* @brief G�re un personnage.
*
* Info :
* - Pour mettre un perso sur en �tat: mort -> defArmements(0)<br />
*   Pas d'arme OU plus de vie -> mort<br />
*   Pour definir une partie avec un nombre de vie illimit� : quantite_MAX_Ramassable = 0 => SANS VIE<br />
*/
class perso
{
	private:
		// struct {
		std::string c_nom;
		s_Coordonnees c_pos;
		bonus* c_armements;
		//clavier c_Touches; <- CLAVIER g�r� en global ( options g�n�ral )
		// }

	public:
		// Init & UnInit
		perso();
		perso( std::string nom, unsigned int Xpos, unsigned int Ypos );
		~perso();

		// Modificateurs
		void defNom( std::string nom );
		void defPos( unsigned int Xpos, unsigned int Ypos );
		void defPos( s_Coordonnees pos );
		void defX( unsigned int Xpos );
		void defY( unsigned int Ypos );
		void defArmements( bonus* a );

		// Accesseurs
		std::string nom() const;
		unsigned int X() const;
		unsigned int Y() const;
		bool estVivant() const;
		bonus* armements() const;
};

#endif
