#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32) || defined(WIN64)
	#include <winsock2.h>
	#pragma comment(lib, "wsock32.lib")
	typedef int socklen_t;
	#define IF_WINDOWS( ... ) __VA_ARGS__
	#define SOCKET_REPPORT_ERROR WSAGetLastError()

#elif defined (linux)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h> /* close */
	#include <netdb.h> /* gethostbyname */
	#include <errno.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define closesocket(s) close(s)
	typedef int SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct sockaddr SOCKADDR;
	typedef struct in_addr IN_ADDR;
	#warning LINUX

	#define IF_WINDOWS( ... )
	#define SOCKET_REPPORT_ERROR errno
#else
	#error Pas de support pour votre platforme
#endif

#define MAX_CLIENTS 10

// Système d'erreur SANS EXIT
#define stdError( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, ##__VA_ARGS__); fflush(stderr);}

// Système d'erreur AVEC EXIT
#define stdErrorE( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, ##__VA_ARGS__); exit(__LINE__);}

/*
typedef struct{
	SOCKET      sock;
	SOCKADDR_IN data;
} socket_t;
*/
/*
typedef struct WSAData {
	// The version of the Windows Sockets specification that the Ws2_32.dll expects the caller to use.
	WORD	wVersion;
	// The highest version of the Windows Sockets specification that the Ws2_32.dll can support
	// This is the same value as the wVersion member when the version requested in the wVersionRequested parameter passed to the WSAStartup function is the highest version of the Windows Sockets specification that the Ws2_32.dll can support.
	WORD	wHighVersion;
	// (ASCII) Description of the Windows Sockets implementation
	char	szDescription[WSADESCRIPTION_LEN+1];
	// (ASCII) Copies relevant status or configuration information. The Ws2_32.dll should use this parameter only if the information might be useful to the user or support staff. This member should not be considered as an extension of the szDescription parameter.
	char	szSystemStatus[WSASYS_STATUS_LEN+1];
	// The maximum number of sockets that may be opened. This member should be ignored for Windows Sockets version 2 and later.
	unsigned short	iMaxSockets;
	// The maximum datagram message size. This member is ignored for Windows Sockets version 2 and later.
	unsigned short	iMaxUdpDg;
	// A pointer to vendor-specific information. This member should be ignored for Windows Sockets version 2 and later.
	char * 	lpVendorInfo;
} WSADATA;
*/

typedef struct {
	SOCKET		listenSocket;
	SOCKADDR_IN	addr;
} sClient;


int main()// -lws2_32
{
	//freopen("bug.txt", "w", stderr);

	SOCKET listenSocket;
	sClient listClient[MAX_CLIENTS] = {0};
	SOCKADDR_IN addr;
	socklen_t clientDataSize = sizeof(SOCKADDR_IN);
	fd_set rdfs;
	unsigned int nb_clientConnected = 0;
	int max = 0;
	timeval wait;
	wait.tv_sec = 0;
	wait.tv_usec = 100000;  /* 0.1 secondes */

	#if defined(WIN32) || defined(WIN64)
		WSADATA wsaData;
		// MAKEWORD(2, 2) := La version que l'on veux avoir (2.2)
		// Démarrage winsock
		int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if( err )
			stdErrorE("Erreur(%d) lors du démmarage de WinSock", err);
		stdError("Info sur le serveur: wVersion=%u, wHighVersion=%u, szDescription=%s, szSystemStatus=%s", wsaData.wVersion, wsaData.wHighVersion, wsaData.szDescription, wsaData.szSystemStatus);

		if( LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 )
			stdError("La version WinSock 2.2 n'est pas disponnible sur cette ordinateur.");

	#endif
	// AF_INET, PF_INET := The Internet Protocol version 4 (IPv4) address family.
	// AF_INET6, PF_INET6 := The Internet Protocol version 6 (IPv6) address family.
	// AF_INET = Address Format, Internet = IP Addresses
	// PF_INET = Packet Format, Internet = IP, TCP/IP or UDP/IP
	// SOCK_STREAM := A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism. This socket type uses the Transmission Control Protocol (TCP) for the Internet address family (AF_INET or AF_INET6).
	// SOCK_DGRAM := A socket type that supports datagrams, which are connectionless, unreliable buffers of a fixed (typically small) maximum length. This socket type uses the User Datagram Protocol (UDP) for the Internet address family (AF_INET or AF_INET6).
	listenSocket = socket(PF_INET, SOCK_STREAM, 0/* Pas de protocole */);
	if( listenSocket == INVALID_SOCKET ){
		stdError("Erreur(%d) lors de l'ouverture du socket", SOCKET_REPPORT_ERROR);
		IF_WINDOWS(
			WSACleanup();
		)
		exit(__LINE__);
	}

	// Paramètrage du serveur
	addr.sin_family      = AF_INET;			// Address family
	addr.sin_port        = htons(1234);		// IP port ( The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).)
	// The in_addr structure is the IPv4 equivalent of the IPv6-based in6_addr structure.
	addr.sin_addr.s_addr = INADDR_ANY;		// IP address ( ex d'une autre utilisation: inet_aton // inet_addr("127.0.0.1") )
	// addr.sin_zero // padding ( packed )

	// Bind lie un socket avec une structure sockaddr.
	if( bind(listenSocket,(SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR ){
		closesocket(listenSocket);
		stdError("Erreur(%d) lors de l'association de l'adresse local avec le socket.", SOCKET_REPPORT_ERROR);
		IF_WINDOWS(
			WSACleanup();
		)
		exit(__LINE__);
	}

	// The listen function places a socket in a state in which it is listening for an incoming connection.
	// Nb de connection 0=oo
	if( listen(listenSocket, 0) == SOCKET_ERROR ){
		stdError("Erreur(%d) lors de l'écoute sur le socket.", SOCKET_REPPORT_ERROR);
		IF_WINDOWS(
			WSACleanup();
		)
		exit(__LINE__);
	}

	max = (int)listenSocket;// Pour le fd de select
	do{
		stdError("Server Running");
		/***********************************************************************
		* Récupération des clients
		*/
		FD_ZERO(&rdfs);

		// Ajout du socket principal dans la liste des écoute non bloquante
		FD_SET(listenSocket, &rdfs);

		// Ajout du socket principal dans la liste des écoute non bloquante
		for( unsigned int i=0; i<nb_clientConnected; i++ )
		{
			FD_SET(listClient[i].listenSocket, &rdfs);
		}

	/*
		// LINUX:
		//fcntl (sock, F_SETFL, O_NONBLOCK);
		// WINDOWS:
		if(ioctlsocket(sock, FIONBIO, (unsigned long*) &nonblocking)<0){
			perror("La socket ne peut être rendu non-bloquante car" );
			cerr<<"On continue quand même avec socket bloquante. Patience."<<endl;
		}
	*/


		// Lecture du socket principal
		if( select(max+1, &rdfs, NULL, NULL, &wait) == SOCKET_ERROR ){
			closesocket(listenSocket);
			stdError("Erreur(%d) lors de la lecture (select) du socket principal.", SOCKET_REPPORT_ERROR);
			IF_WINDOWS(
				WSACleanup();
			)
			exit(__LINE__);
		}

		// On vérifit si new client
		if( FD_ISSET(listenSocket, &rdfs) ){
			listClient[nb_clientConnected].listenSocket = accept(listenSocket, (SOCKADDR*)&listClient[nb_clientConnected].addr, &clientDataSize);// <- BLOCK
			if( listClient[nb_clientConnected].listenSocket != INVALID_SOCKET )
			{
				// Traitement ici
				nb_clientConnected++;
				stdError("Nouveau client !");
			}
			//closesocket(listClient[nb_clientConnected].listenSocket);
		}

		// On check les client actuel
		for( unsigned int i=0; i<nb_clientConnected; i++ )
		{
			if( FD_ISSET(listClient[i].listenSocket, &rdfs) ){
				// info client
				stdError("Info client !");
				closesocket(listClient[i].listenSocket);
				nb_clientConnected--;
			}
		}
	}while(1);

	// Fermeture des socket
	for( unsigned int i=0; i<nb_clientConnected; i++ )
	{
		closesocket(listClient[i].listenSocket);
	}
	closesocket(listenSocket);

/*
	// Récupération des clients et mise en thread
	while(1)
	{
		client = memoryAlloc(sizeof(socket_t));
		if( (client->sock = accept(server.sock, (SOCKADDR *)&(client->data), &clientDataSize)) != INVALID_SOCKET )
		{
			clientNumber++;
			pthread_create(&(client->clientThread), 0, clientManager, client); // Nouveau thread
			client = NULL;
		}
	}
*/
	IF_WINDOWS(
		// Arrêt de WinSock
		if( WSACleanup() == SOCKET_ERROR )
			stdErrorE("Erreur(%d) lors de l'arrêt de WinSock", SOCKET_REPPORT_ERROR);
	)
	return 0;
}
