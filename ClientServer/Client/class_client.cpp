#include "class_client.h"
#include "../../debug.h"


/***************************************************************************//*!
* @fn client::client()
* @brief Constructeur par défaut
* @note L'adresse par défaut de connection au serveur est 127.0.0.1
*/
client::client()
{
	IF_WINDOWS(
		c_serverAdress.S_un.S_addr = inet_addr("127.0.0.1");
	)
	IF_LINUX(
		c_serverAdress.s_addr = inet_addr("127.0.0.1");
	)
}


/***************************************************************************//*!
* @fn client::~client()
* @brief Destructeur par défaut
* @note Déconnect le client
*/
client::~client()
{
	disconnection();
}


/***************************************************************************//*!
* @fn bool client::connection()
* @brief Permet de se connecter au serveur
* @return True si connection réussi !
* @note Cette fonction est la seule a permettre la connection.<br />
* TCP_NoDelay = true
*/
bool client::connection()
{
	// AF_INET, PF_INET := The Internet Protocol version 4 (IPv4) address family.
	// AF_INET6, PF_INET6 := The Internet Protocol version 6 (IPv6) address family.
	// AF_INET = Address Format, Internet = IP Addresses
	// PF_INET = Packet Format, Internet = IP, TCP/IP or UDP/IP
	// SOCK_STREAM := A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism. This socket type uses the Transmission Control Protocol (TCP) for the Internet address family (AF_INET or AF_INET6).
	// SOCK_DGRAM := A socket type that supports datagrams, which are connectionless, unreliable buffers of a fixed (typically small) maximum length. This socket type uses the User Datagram Protocol (UDP) for the Internet address family (AF_INET or AF_INET6).
	c_listenSocket = socket(PF_INET, SOCK_STREAM, 0/* Pas de protocole */);
	if( c_listenSocket == INVALID_SOCKET ){
		this->~client();
		stdErrorE("Erreur(%d) lors de l'ouverture du socket. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));
	}

	// Paramètrage du serveur
	c_addr.sin_family		= AF_INET;			// Address family
	c_addr.sin_port			= htons(c_port);	// IP port ( The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).)
	// The in_addr structure is the IPv4 equivalent of the IPv6-based in6_addr structure.
	c_addr.sin_addr			= c_serverAdress;
	// c_addr.sin_zero // padding ( packed )

	setTCP_NoDelay( __TCP_NODELAY__ );

	if( connect(c_listenSocket, (SOCKADDR *)&c_addr, sizeof(c_addr)) == SOCKET_ERROR ){
		stdError("Erreur(%d) lors de la connection au serveur. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));
		c_listenSocket = 0;
		return false;
	}

	return true;
}


/***************************************************************************//*!
* @fn void client::disconnection()
* @brief Permet de se déconnecter au serveur
*/
void client::disconnection()
{
	// Fermeture de la connection
	if( c_listenSocket )
		closesocket(c_listenSocket);
		//shutdown(c_listenSocket, SHUT_RDWR);// SHUT_RDWR = No more receptions or transmissions.

	c_listenSocket = 0;
}


/***************************************************************************//*!
* @fn bool client::is_connected()
* @brief Permet de savoir si l'on est connecté au serveur
*/
bool client::is_connected()
{
	return (bool)c_listenSocket;
}


/***************************************************************************//*!
* @fn void client::setServerAdress( const char ip[] )
* @brief Permet de définir le serveur où se connecter
* @param[in] ip L'adresse IP du serveur où l'on veut se connecter ( PAS DE NOM DE DOMAINE ! )
*/
void client::setServerAdress( const char ip[] )
{
	// inet_ntoa();// Reverse IP -> char IP[]
	IF_WINDOWS(
		c_serverAdress.S_un.S_addr = inet_addr(ip);
	)
	IF_LINUX(
		c_serverAdress.s_addr = inet_addr(ip);
	)
}


/***************************************************************************//*!
* @fn void client::setServerAdress( const IN_ADDR* ip )
* @brief Permet de définir le serveur où se connecter
* @param[in] ip L'adresse IP du serveur où l'on veut se connecter
*/
void client::setServerAdress( const IN_ADDR* ip )
{
	c_serverAdress = *ip;
}


/***************************************************************************//*!
* @fn const IN_ADDR* client::setServerHost( const char domain[] )
* @brief Permet de définir le serveur où se connecter
* @param[in] domain Le nom de domaine où se connecter. (ex: google.fr)
* @return NULL si bug
*/
const IN_ADDR* client::setServerHost( const char domain[] )
{
	//inet_pton()// Linux IPv4, IPv6

	HOSTENT* hostinfo = 0;

	hostinfo = gethostbyname(domain);
	if( !hostinfo ){
	   stdError("Unknown host %s", domain);
	   return 0;
   }

	c_serverAdress = *((IN_ADDR *)hostinfo->h_addr);

	return &c_serverAdress;
}


/***************************************************************************//*!
* @fn bool client::lookupConnection()
* @brief Permet de récupérer détecter l'activiter sur le réseau
*/
bool client::lookupConnection()
{
	// Remise à 0 du fd read
	FD_ZERO(&c_rdfs);

	// Ajout du socket principal dans la liste des écoute non bloquante
	FD_SET(c_listenSocket, &c_rdfs);

	// Lecture du socket principal
	if( select(c_listenSocket+1, &c_rdfs, NULL, NULL, &c_wait) == SOCKET_ERROR ){
		stdError("Erreur(%d) lors de la lecture (select) du socket principal. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));
		return false;
	}

	// On vérifit si new client
	if( FD_ISSET(c_listenSocket, &c_rdfs) )
		return true;

	return false;
}


/***************************************************************************//*!
* @fn int client::readServer( char* buffer, unsigned int bufferSize )
* @brief Récupère les données envoyées par le serveur
* @param[in,out] buffer La variables où seront stockées les données
* @param[in] bufferSize Taille du buffer
* @return Le nombre de byte reçut
*
* @note si retour = 0 => Client déconnecté (Le client est automatiquement viré de la liste dans ce cas la)<br />
* En cas d'erreur, le client sera déconnecté
*/
int client::readServer( char* buffer, unsigned int bufferSize )
{
	int nbByte = baseClientServer::readClient( c_listenSocket, buffer, bufferSize );
	if( !nbByte )
		disconnection();
		//stdError("Le serveur nous a deco !");

	return nbByte;
}
