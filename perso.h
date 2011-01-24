#ifndef PERSO_h
#define PERSO_h

#include <string>
#include "bonus.h"
#include "ClientServer/class_BaseClientServer.h"// Pour avoir le type SOCKET

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
			s_Coordonnees c_OldPos;
			bonus* c_armements;
			t_Orientation c_orientation;
			SOCKET c_MySocket;//!< Socket a utiliser pour le perso. Valeur par defaut: INVALID_SOCKET
			bool c_isLocal;
			bool c_need_refresh;//<! Permet de dire si le perso a besoin que son interface de présentation soit raffraichit
			//clavier c_Touches; <- CLAVIER géré en global ( options général )
		// }

	public:
		// Init & UnInit
		perso();
		perso( std::string nom, unsigned int Xpos, unsigned int Ypos );
		~perso();

		// Modificateurs
		inline void defNom( std::string nom );
		void defPos( unsigned int Xpos, unsigned int Ypos );
		void defPos( const s_Coordonnees& pos );
		inline void defOldPos( unsigned int Xpos, unsigned int Ypos );
		inline void defOldPos( const s_Coordonnees& OldPos );
		void defX( unsigned int Xpos );
		void defY( unsigned int Ypos );
		inline void defNeed_Refresh( bool refresh );
		void defArmements( bonus* a );
		inline void defOrientation( t_Orientation ori );
		inline void defLocal( bool isLocal );
		inline void defSocket( SOCKET MySocket );

		// Accesseurs
		const std::string* nom() const;
		inline unsigned int X() const;
		inline unsigned int Y() const;
		inline unsigned int old_X() const;
		inline unsigned int old_Y() const;
		inline const s_Coordonnees* old_Pos() const;
		bool estVivant() const;
		inline bonus* armements() const;
		inline t_Orientation orientation() const;
		inline bool isLocal() const;
		inline SOCKET socket() const;
		inline bool need_refresh();
};

#include "perso.inl"

#endif
