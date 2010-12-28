#ifndef PARTIE_h
#define PARTIE_h

#include "options.h"
#include "perso.h"
#include "map.h"
#include "ClientServer/Client/class_client.h"
#include "ClientServer/Server/class_server.h"
#include "metaProg.inl"


class partie
{
	private:
		/*!
		* @struct s_Event
		* @brief Permet de gérer les event graphiques.
		*
		* Exemple: La duréer d'une animation
		*/
		typedef struct {
			std::vector<s_Coordonnees>	listBlockDetruit;//!< Contient la liste des block qui ont été détruit => Bonus a la clef ^^
			std::vector<s_Coordonnees>	deflagration;//!< Contient la position de tous block qui ont été touchés par la déflagration
			s_Coordonnees				pos;//!< Position Originel de l'event
			bonus::t_Bonus				type;//!< Type de bonus
			clock_t						repetionSuivante;//!< Time de la prochaine répétion
			unsigned char				joueur;//!< Le joueur qui est la cause de l'event
			unsigned char				Nb_Repetition;//!< Nombre de répétition actuel pour l'event
			unsigned char				Nb_Repetition_MAX;//!< Nombre de répétition MAX pour l'event
			bool						continue_X,
										continue_negativeX,
										continue_Y,
										continue_negativeY;
		} s_Event;


	public:
		enum t_MODE {
			Combat_Libre,		// Free For All 	( F4A )
			Attrape_drapeau		// Capture the Flag	( CTF )
		};
		typedef enum {
			CNX_Host,
			CNX_Client,
			CNX_None			//!< Pas de connection
		} t_Connection;
		// Réseau ( Variable constante )
		enum {
			PACK_bufferSize = getSizeOfNumber<-1>::value*3/*3 uint32*/+getSizeOfNumber<map::bombe_poser_AVEC_plusieurs_joueurs>::value/*1 type*/+3/*virgule*/+1/*0*/
		};
		/*!
		* @typedef libAff
		* @brief Signature de la fonction a utiliser pour l'affichage de la map
		*
		* La fonction en question doit prendre en paramètre la partie actuel.<br />
		* Elle doit afficher la map.<br />
		* Puis elle doit renvoyer la touche actuellement appuyée.
		*
		* @see partie::main(libAff * afficherMapEtEvent);
		*/
		//typedef SYS_CLAVIER(CLASS_TO_USE::*fctAff)(const partie*);
		typedef SYS_CLAVIER(libAff)(const partie*);


	private:
		// struct {
			std::vector<s_Event>			c_listEvent;
			map*							c_map;//!< SIMPLE POINTEUR !
			union{
				perso*						c_joueurs;//!< Tableau de joueur (utilisé si offline ou si host)
				perso::t_Orientation*		c_joueursOrientation;//!< Tableau d'orientation (Utilisé si online + client)
			};
			SOCKET*							c_listClient;
			t_Connection					c_connection;//!< Partie en Host, Client
			t_MODE							c_mode;
			unsigned char					c_nb_joueurs;//!< Le nombre de joueur dans la partie
			unsigned char					c_nb_MAX_joueurs;
			union {// Rien ne sert de prendre 2 fois la taille d'un pointeur, si on en a besoin que d'un seul.
				client*						c_client;
				server*						c_server;
			};
			char							c_buffer[PACK_bufferSize];//!< Buffer pour communiquer sur le réseau
		// }

		void deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur );
		void checkInternalEvent();
		char actionSurLesElements( s_Event* e, unsigned int x, unsigned int y, char direction );
		char killPlayers( s_Event* e, unsigned int x, unsigned int y );

		// Réseau
		const char* packIt( uint32_t X, uint32_t Y, map::t_type type, uint32_t nb_MetaDonnee );
		void unPackIt( uint32_t* X, uint32_t* Y, map::t_type* type, uint32_t* nb_MetaDonnee );
		void ajouterNouvelleConnection( SOCKET s );


	public:
		partie();
		~partie();
		// Modificateurs
		void genMap();
		void def_nbJoueurs( unsigned char nb );
		inline void def_nbMAX_joueurs( unsigned char nb );
		inline void def_modeJeu( t_MODE m );
		void def_connection( t_Connection cnx );


		// Accesseurs
		inline unsigned char nbJoueurs() const;
		inline unsigned char nbMAX_joueurs() const;
		inline t_MODE modeJeu() const;
		inline t_Connection connection() const;
		perso* joueur( unsigned int joueur_numero ) const;
		inline map* getMap() const;
		unsigned char nbJoueurVivant() const;
		server* getServeur() const;
		client* getClient() const;

		// Autres
		void main( libAff * afficherMapEtEvent );
};


#include "partie.inl"

#endif
