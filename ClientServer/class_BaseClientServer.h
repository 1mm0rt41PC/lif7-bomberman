#ifndef CLASS_BASECLIENTSERVER_h
#define CLASS_BASECLIENTSERVER_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

// R�seau
#define __TCP_NODELAY__ true

#if defined(WIN32) || defined(WIN64)
	#include <winsock2.h>
	//#pragma comment(lib, "wsock32.lib")
	typedef int socklen_t;
	#define IF_WINDOWS( ... ) __VA_ARGS__
	#define IF_LINUX( ... )
	#define SOCKET_REPPORT_ERROR WSAGetLastError()
	#define SHUT_RDWR SD_BOTH

#elif defined (linux)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>		// Pour TCP_NODELAY et SOL_TCP.
	#include <arpa/inet.h>
	#include <unistd.h>				// close
	#include <netdb.h>				// gethostbyname
	#include <errno.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define closesocket(s) close(s)
	typedef int SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct sockaddr SOCKADDR;
	typedef struct in_addr IN_ADDR;
	typedef struct hostent HOSTENT;
	#define IF_WINDOWS( ... )
	#define IF_LINUX( ... ) __VA_ARGS__
	#define SOCKET_REPPORT_ERROR errno

#else
	#error Pas de support pour votre platforme
#endif


#ifndef stdError
	// Syst�me d'erreur SANS EXIT
	#define stdError( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, ##__VA_ARGS__); fflush(stderr);}

	// Syst�me d'erreur AVEC EXIT
	#define stdErrorE( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, ##__VA_ARGS__); exit(__LINE__);}
#endif


/*!
* @class baseClientServer
* @brief Support pour la cr�ation d'une class client/serveur
* @warning Cette class ne contient pas les outils n�c�ssaire pour �tre consid�r�e comme un serveur ou un client
*/
class baseClientServer
{
	private:
		IF_WINDOWS(
			static WSADATA c_wsaData;
		)


	protected:
		timeval c_wait;//!< Temps d'attente au niveau du select
		unsigned int c_port;//!< Port a utiliser
		SOCKET c_listenSocket;//!< Le socket principal. ( c'est clui l� quit sert pour l'�coute sur le port c_port )
		SOCKADDR_IN c_addr;
		fd_set c_rdfs;//!< Le fd qui est utiliser pour check les signaux en lecture


	public:
		baseClientServer();
		~baseClientServer();

		// Acc�sseurs
		inline unsigned int port() const;
		inline bool TCP_NoDelay();
		bool TCP_NoDelay( SOCKET s );

		// Modificateurs
		void setPort( unsigned int port );
		void setWait( long tv_sec, long tv_usec );
		inline void setWait( const timeval* wait );
		inline void setTCP_NoDelay( bool activer );
		void setTCP_NoDelay( SOCKET s, bool activer );

		// R�c�ption de message
		int readClient( SOCKET sock, char* buffer, unsigned int bufferSize );

		// Envoie de message
		void send_message( SOCKET sock, const char* buffer, unsigned int bufferSize );

		// Compactiblit�
		IF_WINDOWS(
			inline const char* strerror( int ) const { return "?"; }
		)

		static void printAllBuffer( const char buffer[], unsigned int bufferSize, const char file[], unsigned int line );

		static bool isLittleEndian();// isIntel
		// Conversion Little Endian <-> Big Endian
		template<class Type> static Type reverseByte( Type var );// Inverse les octets
		//template<class Type> Type reverseBit( Type var );// Inverse les bits
		char* getLocalIPv4() const;
};

#include "class_BaseClientServer.inl"

#endif
