#ifndef CLAVIER_h
#define CLAVIER_h

#include "config.h"
#include <stdio.h> // Pour fread, fwrite et perror ( pour les erreurs )


class clavier
{
	private:
		// struct {
		SYS_CLAVIER *c_touches;
		unsigned int c_nb_touches;
		// }

	public:
		typedef enum t_touche {
			haut = 0,
			bas,
			droite,
			gauche,
			lancerBombe,
			declancheur
		};

		clavier();
		inline clavier( unsigned int nb_touches ){ initClavier( nb_touches ); }
		~clavier();

		// Modificateurs
		void initClavier( unsigned int nb_touches );
		void defTouche( t_touche t, SYS_CLAVIER tsys );
		bool chargerConfig( FILE* fp, unsigned int nb_touches );

		// Accesseurs
		bool estAppuyer( SYS_CLAVIER tsys ) const;
		unsigned int nb_touches() const;
		SYS_CLAVIER touche( t_touche t ) const;

		// Autre
		bool enregistrerConfig( FILE* fp );
};

#endif
