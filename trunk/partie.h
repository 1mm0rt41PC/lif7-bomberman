#ifndef PARTIE_h
#define PARTIE_h

#include "options.h"
#include "perso.h"
#include "map.h"
#include "ClientServer/Client/class_client.h"
#include "ClientServer/Server/class_server.h"
#include "metaProg.inl"
#include "QList.h"


/*!
* @class partie
* @brief Permet de gérer une partie offline ou online ( host et client )
*/
class partie
{
	private:
		/*!
		* @struct s_EventBombe
		* @brief Permet de gérer les event des bombes.
		*/
		typedef struct {
			std::vector<s_Coordonnees>	listBlockDetruit;//!< Contient la liste des block qui ont été détruit => Bonus a la clef ^^
			std::vector<s_Coordonnees>	deflagration;//!< Contient la position de tous block qui ont été touchés par la déflagration
			s_Coordonnees				pos;//!< Position Originel de l'event
			clock_t						repetionSuivante;//!< Time de la prochaine répétion
			unsigned char				joueur;//!< Le joueur qui est la cause de l'event
			unsigned char				Nb_Repetition;//!< Nombre de répétition actuel pour l'event
			unsigned char				Nb_Repetition_MAX;//!< Nombre de répétition MAX pour l'event
			bool						continue_X,
										continue_negativeX,
										continue_Y,
										continue_negativeY;
		} s_EventBombe;

		/*!
		* @struct s_EventPousseBombe
		* @brief Permet de gérer les event des pousses bombes.
		*/
		typedef struct {
			perso::t_Orientation		direction;//!< Direction de la bombe
			s_Coordonnees				pos;//!< Position de l'event
			clock_t						repetionSuivante;//!< Time de la prochaine répétion
			unsigned char				joueur;//!< Le joueur a qui appartient la bombe [0-...[
		} s_EventPousseBombe;


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
			PACK_bufferSize = getSizeOfNumber<-1>::value*4/*4 uint32*/+getSizeOfNumber<map::bombe_poser_AVEC_plusieurs_joueurs>::value/*1 type*/+4/*virgule*/+1/*²*/+1/*0*/
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
		//typedef SYS_CLAVIER(CLASS_TO_USE::*fctAff)(partie*);
		typedef SYS_CLAVIER(libAff)(partie*);


	private:
		// struct {
			std::vector<s_EventBombe>		c_listEventBombe;
			QList<s_EventPousseBombe>		c_listEventPouseBombe;
			std::vector<unsigned char>		c_listPlayerRefresh;
			map*							c_map;//!< SIMPLE POINTEUR !
			perso*							c_joueurs;//!< Tableau de joueur (utilisé si offline ou si host)
			t_Connection					c_connection;//!< Partie en Host, Client
			t_MODE							c_mode;
			unsigned char					c_nb_joueurs;//!< Le nombre de joueur dans la partie
			union {// Rien ne sert de prendre 2 fois la taille d'un pointeur, si on en a besoin que d'un seul.
				client*						c_client;
				server*						c_server;
			};
			char							c_buffer[PACK_bufferSize+1];//!< Buffer pour communiquer sur le réseau
			std::string						c_winnerName;
			clock_t							c_timeOut;
			unsigned int					c_timerAttak;
			unsigned char					c_uniqueJoueurID;//!< Si connection client -> ID du joueur (correspond au numéro dans le tableau c_joueur)
		// }

		void placer_perso_position_initial( unsigned char joueur );
		void deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur );
		void checkInternalEvent();
		void checkInternalEventPousseBombe();
		char actionSurLesElements( s_EventBombe* e, unsigned int x, unsigned int y, char direction );
		char killPlayers( s_EventBombe* e, unsigned int x, unsigned int y );

		// Réseau
		const char* packIt( uint32_t X, uint32_t Y, map::t_type type );
		const char* packIt( std::vector<unsigned char>* list );
		const char* packIt( clavier::t_touche t );
		const char* packIt( unsigned char joueur, bool includeWeapon );
		void unPackIt( uint32_t* X, uint32_t* Y, map::t_type* type );
		void unPackIt( uint32_t X, uint32_t Y );
		void unPackIt( clavier::t_touche* t );
		void unPackIt();
		void ajouterNouvelleConnection( SOCKET s );


	public:
		partie();
		~partie();
		// Modificateurs
		void genMap();
		void def_nbJoueurs( unsigned char nb );
		inline void def_modeJeu( t_MODE m );
		void def_connection( t_Connection cnx );


		// Accesseurs
		inline unsigned char nbJoueurs() const;
		unsigned char nbJoueursReel() const;
		inline t_MODE modeJeu() const;
		inline t_Connection connection() const;
		perso* joueur( unsigned int joueur_numero ) const;
		inline map* getMap() const;
		unsigned char nbJoueurVivant() const;
		server* getServeur() const;
		client* getClient() const;
		inline std::string getWinnerName() const;
		inline clock_t timeOut() const;
		inline unsigned char getUniqueJoueurID() const;
		bool playerNeedRefresh( unsigned char idJoueur );

		// Autres
		char main( libAff * afficherMapEtEvent );
};

#include "partie.inl"

#endif
