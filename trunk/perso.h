#ifndef PERSO_h
#define PERSO_h

#include <string>
#include "bonus.h"
#include "ClientServer/class_BaseClientServer.h"// Pour avoir le type SOCKET

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
		bool c_isLocal;
		SOCKET c_MySocket;//!< Socket a utiliser pour le perso. Valeur par defaut: INVALID_SOCKET
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
		void defOrientation( t_Orientation ori );
		void defLocal( bool isLocal );
		void defSocket( bool MySocket );

		// Accesseurs
		const std::string* nom() const;
		unsigned int X() const;
		unsigned int Y() const;
		bool estVivant() const;
		bonus* armements() const;
		t_Orientation orientation() const;
		bool isLocal() const;
		SOCKET socket() const;
};

#endif
