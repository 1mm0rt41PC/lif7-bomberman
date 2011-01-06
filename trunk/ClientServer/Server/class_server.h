#ifndef CLASS_SERVER_h
#define CLASS_SERVER_h

#include "../class_BaseClientServer.h"


/*!
* @class server
* @brief Cette class permet de gérer facilement un serveur.
*
* Exemple d'utilisation
* @code
*	SOCKET s;
*	char buffer[20];
*
*	server serv;
*	serv.setPort( 1234 );
*	serv.setNbClientMax( 5 );
*	serv.setWait( 0 , 100000);// 0.1secs
*	serv.Listening();
*
*	do {
*		serv.lookupNewConnection();// A faire en 1er !
*		serv.lookupConnectionClient();
*
*		while( (s = serv.lookupConnectionClient()) != INVALID_SOCKET )
*		{
*			serv.readClient( s, buffer, 20 );
*			printf("%s", buffer);
*			serv.send_message(s, "Message reçut", 14);
*		}
*
*	}while( serv.nb_clientConnecter() != 3 );// S'il y a 3 client alors, on quitte la boucle
*
*	serv.endListen();
* @endcode
*/
class server : public baseClientServer
{
	public:
		/*!
		* @struct sClient
		* @brief Contient les information d'un client
		*/
		typedef struct {
			SOCKET		listenSocket;
			SOCKADDR_IN	addr;
		} sClient;

	private:
		sClient* c_listClient;//!< Liste des client connecté
		unsigned int c_nb_Max_Client;//!< Nombre maximum de client. Si = à 0 => pas de limite
		unsigned int c_nb_clientConnecter;//!< Nombre de client actuellement connecté
		socklen_t c_clientDataSize;//!< Taille de la structure sClient ( sizeof(sClient) ) (N'EST JAMAIS MODIFIE)
		int c_max_fd;//!< Le fd (socket) le plus grand (DOIT être maintenu à jour !)

	public:
		server();
		~server();

		// Accésseurs
		inline unsigned int nb_Max_Client() const;
		inline unsigned int nb_clientConnecter() const;
		inline const sClient* listClient() const;
		SOCKET getClientSOCK( unsigned int id ) const;
		unsigned int getClientID( SOCKET ) const;

		// Modificateurs
		inline void setNbClientMax( unsigned int nbClient );
		inline void rmClient( SOCKET s );
		void rmClientID( unsigned int client );

		// Récéption de message
		int readClient( SOCKET sock, char* buffer, unsigned int bufferSize );

		// Autres
		void Listening();
		void Listening( unsigned int port );
		void endListen();
		// Récéption des activités ( Nouvelle connnection, Déconnection, message en attante )
		SOCKET lookupNewConnection();
		inline bool lookupConnectionClient( SOCKET client );
		SOCKET lookupConnectionClient();
};

#include "class_server.inl"

#endif
