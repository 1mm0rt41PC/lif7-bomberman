#ifndef CLAVIER_h
#define CLAVIER_h

#include <stdio.h> // Pour fread, fwrite
#include "config.h"

/*!
* @class clavier
* @brief Gère les claviers.
*
* Cette class a été consu pour éviter les problèmes de portablilité vers des systèmes<br />
* / des lib qui pourraient gérer les claviers d'une autre façon<br />
* La class permet :<br />
*	- de définir des touches
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
			SYS_CLAVIER c_touches[NB_ELEMENT_t_touche];//!< Tableau des touches qui sont affecté à une action
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
