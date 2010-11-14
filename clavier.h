#ifndef CLAVIER_h
#define CLAVIER_h

#include "config.h"
#include "debug.h"
#include <stdio.h> // Pour fread, fwrite


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
	private:
		// struct {
		SYS_CLAVIER *c_touches;//!< Tableau des touches qui sont affecté à une action
		unsigned int c_nb_touches;//!< Nombre de touche dans notre tableau
		// }

	public:
		/*!
		* @enum t_touche
		* @brief Les actions possibles avec les touches
		*/
		enum t_touche {
			NUL = -1,
			haut = 0,
			bas,
			droite,
			gauche,
			lancerBombe,
			declancheur
		};

		clavier();
		inline clavier( unsigned int nb_touches ){ initClavier( nb_touches ); }//!< Alias de initClavier( nb_touches );
		~clavier();

		// Modificateurs
		void initClavier( unsigned int nb_touches );
		void defTouche( t_touche t, SYS_CLAVIER tsys );
		bool chargerConfig( FILE* fp, unsigned int nb_touches );

		// Accesseurs
		bool estDansClavier( SYS_CLAVIER tsys ) const;
		unsigned int nb_touches() const;
		SYS_CLAVIER touche( t_touche t ) const;
		t_touche obtenirTouche( SYS_CLAVIER tsys ) const;

		// Autre
		bool enregistrerConfig( FILE* fp );
};

#endif
