#ifndef OPTIONS_h
#define OPTIONS_h

#include <stdio.h>// Pour remove(char *)
#include "clavier.h"
#include "config.h"

#ifdef __LIB_ncurses__
	#define CONFIG_FILE "config_ncurses.conf"
#elif __LIB_SDL__
	#define CONFIG_FILE "config_sdl.conf"
#elif __LIB_SFML__
	#define CONFIG_FILE "config_sfml.conf"
#endif


/*!
* @class options
* @brief Gère les options.
*
* La class manipule le port de jeu et les claviers ( 4 min et 4 max => 4 claviers tout le temps ^^ )
* <b>Il ne peut y avoir qu'une seule et unique instance de cette class !</b><br />
* <span style="text-decoration:underline;">Utilisation:</span><br />
*	- Pour créer l'instance: option* optGeneral = options::getInstance();
*	- Pour récup l'instance: option* optGeneral = options::getInstance();
*
* Exemple:
* @code
* // Va créer l'instance si elle n'existe pas
* // puis va charger la config
* // puis attribut 5 comme port
* options::getInstance()->defPort( 5 );
*
* clavier* j1 = options::getInstance()->clavierJoueur( 1 );// <- Retourne le clavier du joueur 2 ( nombre de [0 à 3] )
* // NOTE: Dans la ligne juste audessus, le port est bien 5
* @endcode
*
* @warning Utiliser options::getInstance() pour récupérer l'instance général ( ou la créer si elle n'existe pas )
* @todo Prendre en compte le clavier pour SDL, SFML
*/
class options
{
	private:
		// struct {
			clavier c_ClavierJoueur[4];//!< Les claviers dispo en mémoire ( 4 dans tous les cas ! )
			unsigned int c_port;//!< Le port utilisé ( par défaut 947 )
			static options* c_Instance;//!< L'instance de notre class
		// }

		void supprimerFichierConfiguation() const;

		// ON NE TOUCHE PAS AU CONSTRUCTEUR / DESTRUCTEUR
		options();
		~options(){}

	public:
		// Constructeur
		static options* getInstance();
		static void uInit();

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
