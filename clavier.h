#ifndef CLAVIER_h
#define CLAVIER_h

#include <stdio.h> // Pour fread, fwrite
#include "config.h"

/*!
* @class clavier
* @brief G�re les claviers.
*
* Cette class a �t� consu pour �viter les probl�mes de portablilit� vers des syst�mes<br />
* / des lib qui pourraient g�rer les claviers d'une autre fa�on<br />
* La class permet :<br />
*	- de d�finir des touches
*	- d'enregistrer / de charger la configuration d'un clavier
*/
class clavier
{
	public:
		/*!
		* @enum t_touche
		* @brief Les actions possibles avec les touches
		*/
		typedef enum {
			haut = 0,
			bas,
			droite,
			gauche,
			lancerBombe,
			lancerSuperBombe,
			declancheur,
			NB_ELEMENT_t_touche,
			NUL//!< En cas de bug
		} t_touche;

	private:
		// struct {
			SYS_CLAVIER c_touches[NB_ELEMENT_t_touche];//!< Tableau des touches qui sont affect� � une action
		// }

	public:
		clavier();
		~clavier();

		// Modificateurs
		inline void defTouche( t_touche t, SYS_CLAVIER tsys );
		bool chargerConfig( FILE* fp, unsigned int nb_touches );

		// Accesseurs
		bool estDansClavier( SYS_CLAVIER tsys ) const;
		inline SYS_CLAVIER touche( t_touche t ) const;
		t_touche obtenirTouche( SYS_CLAVIER tsys ) const;

		// Autre
		bool enregistrerConfig( FILE* fp ) const;
};

#include "clavier.inl"

#endif
