#ifndef OPTIONS_h
#define OPTIONS_h

#include <stdio.h>// Pour remove(char *), perror(char *)
#include "clavier.h"

#define DEFAULT_PORT 947 // 947 = 'b'+'o'+'m'+'b'+'e'+'r'+'m'+'a'+'n' °(^_^)°
#define CONFIG_FILE "config.conf"

class options
{
	private:
		// struct {
			clavier c_ClavierJoueur[4];
			unsigned int c_port;
		// }
		bool fichierExiste( const char fichier[] ) const;

	public:
		options();

		// Modificateurs
		void configParDefaut();
		void remise_a_zero();
		bool defPort( unsigned int port );

		// Accesseurs
		unsigned int port() const;
		clavier* clavierJoueur( unsigned char clavier_numero );
		clavier* operator[]( unsigned char clavier_numero );

		// Autres
		void enregistrerConfig();
};

#endif
