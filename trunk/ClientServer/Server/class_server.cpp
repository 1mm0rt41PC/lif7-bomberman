#include "class_server.h"
#include "../../debug.h"

// Permet de déconnecter tout les client, de décharger Winsock de la mémoire, puis de rapporter une erreur + exit
#define reportError( ... ) {this->~baseClientServer(); this->~server(); stdErrorE( __VA_ARGS__ )}


/***************************************************************************//*!
* @fn server::server()
* @brief Constructeur par defaut
*
* - Si la platforme est windows => on charge Winsock en mémoire
* - Paramétrage pas défaut.
*/
server::server()
{
	c_listClient = 0;
	c_nb_Max_Client = 0;
	c_nb_clientConnecter = 0;
	c_clientDataSize = sizeof(SOCKADDR_IN);
}


/***************************************************************************//*!
* @fn server::~server()
* @brief Destructeur par defaut
*
* - Appel server::endListen()
* - Si la platforme est windows => on décharge Winsock de la mémoire
*/
server::~server()
{
	endListen();
}


/***************************************************************************//*!
* @fn SOCKET server::getClientSOCK( unsigned int id ) const
* @brief Retourne le socket affecté à l'id
* @param[in] id L'id du client
* @return Le socket affecté à l'id
*/
SOCKET server::getClientSOCK( unsigned int id ) const
{
	if( id >= c_nb_clientConnecter )
		reportError("Le client(%u) demandé n'existe pas ! Il y a actuellement %u client connecté. Le numéro client doit donc être compris entre : 0 <= client < %u", id, c_nb_clientConnecter, c_nb_clientConnecter);

	return c_listClient[id].listenSocket;
}


/***************************************************************************//*!
* @fn unsigned int server::getClientID( SOCKET s ) const
* @brief Retourne l'id du client qui a le socket s
* @param[in] s Le socket dont on veux déterminer l'id qui y est affecté
* @return L'id affectée au socket
*
* @warning En cas d'erreur errno=EINVAL
*/
unsigned int server::getClientID( SOCKET s ) const
{
	for( unsigned int i=0; i<c_nb_clientConnecter; i++ )// -1 pour contrer le ++
		if( c_listClient[i].listenSocket == s )
			return i;

	stdError("Le socket demandé n'est pas dans la liste des socket référencé !");
	errno = EINVAL;
	return 0;
}


/***************************************************************************//*!
* @fn void server::rmClientID( unsigned int client )
* @brief Déconnecte le client puis le supprime de la liste
* @param[in] client id du client dans la liste. 0 <= client < server::nb_clientConnecter()
*/
void server::rmClientID( unsigned int client )
{
	if( client >= c_nb_clientConnecter )
		reportError("Le client(%u) demandé n'existe pas ! Il y a actuellement %u client connecté. Le numéro client doit donc être compris entre : 0 <= client < %u", client, c_nb_clientConnecter, c_nb_clientConnecter);

	// Fermeture de la connection avec le client
	closesocket(c_listClient[client].listenSocket);

	for( unsigned int i=client+1; i<c_nb_clientConnecter; i++ )// -1 pour contrer le ++
	{
		memcpy( c_listClient+i-1, c_listClient+i, sizeof(sClient) );
	}
	c_nb_clientConnecter--;
}


/***************************************************************************//*!
* @fn void server::Listening( unsigned int port )
* @brief Met le serveur en activité sur le port {port}
* @param[in] port Port d'écoute
*/
void server::Listening( unsigned int port )
{
	setPort( port );
	Listening();
}


/***************************************************************************//*!
* @fn void server::Listening()
* @brief Met le serveur en activité. Lance l'écoute sur le port demandé
*/
void server::Listening()
{
	// AF_INET, PF_INET := The Internet Protocol version 4 (IPv4) address family.
	// AF_INET6, PF_INET6 := The Internet Protocol version 6 (IPv6) address family.
	// AF_INET = Address Format, Internet = IP Addresses
	// PF_INET = Packet Format, Internet = IP, TCP/IP or UDP/IP
	// SOCK_STREAM := A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism. This socket type uses the Transmission Control Protocol (TCP) for the Internet address family (AF_INET or AF_INET6).
	// SOCK_DGRAM := A socket type that supports datagrams, which are connectionless, unreliable buffers of a fixed (typically small) maximum length. This socket type uses the User Datagram Protocol (UDP) for the Internet address family (AF_INET or AF_INET6).
	c_listenSocket = socket(PF_INET, SOCK_STREAM, 0/* Pas de protocole */);
	if( c_listenSocket == INVALID_SOCKET )
		reportError("Erreur(%d) lors de l'ouverture du socket. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));

	// Paramètrage du serveur
	c_addr.sin_family      = AF_INET;			// Address family
	c_addr.sin_port        = htons(c_port);		// IP port ( The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).)
	// The in_addr structure is the IPv4 equivalent of the IPv6-based in6_addr structure.
	c_addr.sin_addr.s_addr = INADDR_ANY;		// IP address ( ex d'une autre utilisation: inet_aton // inet_addr("127.0.0.1") )
	// c_addr.sin_zero // padding ( packed )

	IF_WINDOWS(
		char reuse = 1;
	)
	IF_LINUX(
		int reuse = 1;
	)
	if( setsockopt(c_listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == SOCKET_ERROR )
		reportError("Erreur(%d). Impossible de permettre la réutilisation du port ! setsockopt(). Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));

	setTCP_NoDelay( __TCP_NODELAY__ );

	// Bind lie un socket avec une structure sockaddr.
	if( bind(c_listenSocket, (SOCKADDR*)&c_addr, sizeof(c_addr)) == SOCKET_ERROR )
		reportError("Erreur(%d) lors de l'association de l'adresse local avec le socket. Port deja occupe ? Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));

	// The listen function places a socket in a state in which it is listening for an incoming connection.
	// Nb de connection 0=oo
	if( listen(c_listenSocket, c_nb_Max_Client) == SOCKET_ERROR )
		reportError("Erreur(%d) lors de l'écoute sur le socket. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));

	c_max_fd = (int)c_listenSocket;// Pour le fd de select
}


/***************************************************************************//*!
* @fn void server::endListen()
* @brief Met fin a une écoute sur un port
*
* - Déconnection des client
* - Déconnection du port
*
* Après cette fonction, un nouvel appel server::Listening() peut être fait.
*/
void server::endListen()
{
	// Fermeture des socket
	for( unsigned int i=0; i<c_nb_clientConnecter; i++ )
	{
		closesocket(c_listClient[i].listenSocket);
		//shutdown(c_listClient[i].listenSocket, SHUT_RDWR);// SHUT_RDWR = No more receptions or transmissions.
	}
	if( c_listClient )
		delete[] c_listClient;
	c_listClient = 0;
	c_nb_clientConnecter = 0;

	if( c_listenSocket ){
		closesocket(c_listenSocket);
		//shutdown(c_listenSocket, SHUT_RDWR);// SHUT_RDWR = No more receptions or transmissions.
	}

	c_listenSocket = 0;
}


/***************************************************************************//*!
* @fn SOCKET server::lookupNewConnection()
* @brief Envoie une message (buffer) au client
* @return	- INVALID_SOCKET Si pas d'activité ( pas de nouvelle connection )
*			- Le SOCKET du nouveau client
*
* Cette fonction met à jour les fd, puis execute le select
*/
SOCKET server::lookupNewConnection()
{
	// Remise à 0 du fd read
	FD_ZERO(&c_rdfs);

	// Ajout du socket principal dans la liste des écoute non bloquante
	FD_SET(c_listenSocket, &c_rdfs);

	// Ajout du socket principal dans la liste des écoute non bloquante
	for( unsigned int i=0; i<c_nb_clientConnecter; i++ )
	{
		FD_SET(c_listClient[i].listenSocket, &c_rdfs);
	}

	// Lecture du socket principal
	if( select(c_max_fd+1, &c_rdfs, NULL, NULL, &c_wait) == SOCKET_ERROR )
		reportError("Erreur(%d) lors de la lecture (select) du socket principal. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));

	// On vérifit si new client
	if( FD_ISSET(c_listenSocket, &c_rdfs) ){
		sClient newClient;
		newClient.listenSocket = accept(c_listenSocket, (SOCKADDR*)&newClient.addr, &c_clientDataSize);
		if( newClient.listenSocket != INVALID_SOCKET ){
			if( c_nb_Max_Client == 0 || c_nb_clientConnecter+1 <= c_nb_Max_Client ){// On regarde si on a encore de la place
				//stdError("Nouveau client ! Actuelement %u client.", c_nb_clientConnecter+1);
				c_nb_clientConnecter++;
				sClient* tmp = new sClient[c_nb_clientConnecter];
				for( unsigned int i=0; i<c_nb_clientConnecter-1; i++ )// -1 pour contrer le ++
				{
					memcpy( tmp+i, c_listClient+i, sizeof(sClient) );
				}
				if( c_listClient )
					delete[] c_listClient;
				c_listClient = tmp;
				// Ajout du nouveau client
				c_listClient[c_nb_clientConnecter-1].listenSocket = newClient.listenSocket;
				c_listClient[c_nb_clientConnecter-1].addr = newClient.addr;

				setTCP_NoDelay( newClient.listenSocket, TCP_NoDelay() );

				// Ne pas oublier de changer le fd maximum !
				if( newClient.listenSocket > (SOCKET)c_max_fd )
					c_max_fd = newClient.listenSocket;

				return newClient.listenSocket;
			}else{// Plus de place => OUT
				stdError("Un client a ete refoule.");
				closesocket(newClient.listenSocket);
			}
		}else{
			stdError("Erreur(%d) lors de la connection d'un client. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));
		}
	}

	return INVALID_SOCKET;
}


/***************************************************************************//*!
* @fn SOCKET server::lookupConnectionClient()
* @brief Recherche une activité pour tout les clients
* @return Le SOCKET du client qui a eu une activité OU INVALID_SOCKET si pas d'activité
*
* Pour récup l'activité server::readClient( SOCKET sock, char* buffer, unsigned int bufferSize )
*
* @warning Veuillez executer server::lookupNewConnection() avant !
*/
SOCKET server::lookupConnectionClient()
{
	// On check les client actuel
	for( unsigned int i=0; i<c_nb_clientConnecter; i++ )
		if( FD_ISSET(c_listClient[i].listenSocket, &c_rdfs) )
			return c_listClient[i].listenSocket;

	return INVALID_SOCKET;
}



int server::readClient( SOCKET sock, char* buffer, unsigned int bufferSize )
{
	int nbByte = baseClientServer::readClient( sock, buffer, bufferSize );
	if( !nbByte )
		//stdError("Le client s'est deco !");
		rmClient(sock);

	return nbByte;
}
