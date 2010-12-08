#ifndef PERSO_h
#define PERSO_h

#include <string>
#include "bonus.h"


/*!
* @class perso
* @brief Gère un personnage.
*
* Info :
* - Pour mettre un perso sur en état: mort -> defArmements(0)<br />
*   Pas d'arme OU plus de vie -> mort<br />
*   Pour definir une partie avec un nombre de vie illimité : quantite_MAX_Ramassable = 0 => SANS VIE<br />
*/
class perso
{
	public:
		typedef enum {
			ORI_haut,
			ORI_bas,
			ORI_droite,
			ORI_gauche
		} t_Orientation;

	private:
		// struct {
		std::string c_nom;
		s_Coordonnees c_pos;
		bonus* c_armements;
		t_Orientation c_orientation;
		//clavier c_Touches; <- CLAVIER géré en global ( options général )
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
		void defOrientation( t_Orientation ori );

		// Accesseurs
		std::string nom() const;
		unsigned int X() const;
		unsigned int Y() const;
		bool estVivant() const;
		bonus* armements() const;
		t_Orientation orientation() const;
};

#endif
