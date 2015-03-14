#ifndef CLASS_CLIENT_h
#define CLASS_CLIENT_h

#include "../class_BaseClientServer.h"

/*!
* @class client
* @brief Cette class permet de g�rer facilement un client.
*
* Exemple d'utilisation
* @code
*	SOCKET s;
*	char buffer[20];
*
*	client cli;
*	cli.setPort(1234);
*	cli.setServerAdress("127.0.0.1");
*
*	if( cli.connection() == 0 )// Connection et Si pas de connection => exit
*		exit(1);
*
*	do {
*		if( cli.lookupConnection() ){// A faire en 1er !
*			memset(buffer, 0, 20);// On vide le buffer
*			if( cli.readServer(buffer, 20) ){
*				printf("%s", buffer);
*			}else{
*				break;// On a �t� d�connect� !
*			}
*		}
*
*	}while( cli.is_connected() == 1 );// On reste tant que l'on est connect�
* @endcode
*/
class client : public baseClientServer
{
	private:
		IN_ADDR c_serverAdress;//!< L'adresse du serveur


	public:
		client();
		~client();

		// Acc�sseurs
		inline const IN_ADDR* serverAdress() const;

		// Modificateurs
		void setServerAdress( const char ip[] );
		void setServerAdress( const IN_ADDR* ip );
		const IN_ADDR* setServerHost( const char domain[] );

		// Autres
		bool connection();
		void disconnection();
		bool is_connected();

		// R�c�ption des activit�s ( Nouvelle connnection, D�connection, message en attante )
		bool lookupConnection();

		// R�c�ption de message
		int readServer( char* buffer, unsigned int bufferSize );

		// Envoie de message
		inline void send_message( const char* buffer, unsigned int bufferSize );
		using baseClientServer::send_message;
};

#include "class_client.inl"

#endif
