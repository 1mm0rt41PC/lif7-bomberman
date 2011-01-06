#include "class_BaseClientServer.h"
#include "../debug.h"

IF_WINDOWS(
	WSADATA baseClientServer::c_wsaData;
)

// Permet de déconnecter tout les client, de décharger Winsock de la mémoire, puis de rapporter une erreur + exit
#define reportError( ... ) {this->~baseClientServer(); stdErrorE( __VA_ARGS__ )}


/***************************************************************************//*!
* @fn baseClientServer::baseClientServer()
* @brief Constructeur par defaut
*
* - Si la platforme est windows => on charge Winsock en mémoire
* - Paramétrage pas défaut.
*/
baseClientServer::baseClientServer()
{
	IF_WINDOWS(
		// MAKEWORD(2, 2) := La version que l'on veux avoir (2.2)
		// Démarrage winsock
		int err = WSAStartup(MAKEWORD(2, 2), &c_wsaData);
		if( err )
			stdErrorE("Erreur(%d) lors du démmarage de WinSock", err);
		//stdError("Info sur le serveur: wVersion=%u, wHighVersion=%u, szDescription=%s, szSystemStatus=%s", c_wsaData.wVersion, c_wsaData.wHighVersion, c_wsaData.szDescription, c_wsaData.szSystemStatus);

		if( LOBYTE(c_wsaData.wVersion) != 2 || HIBYTE(c_wsaData.wVersion) != 2 )
			stdError("La version WinSock 2.2 n'est pas disponnible sur cette ordinateur.");
	);

	c_wait.tv_sec = 0;
	c_wait.tv_usec = 10000;// 0.01 secondes


	c_listenSocket = 0;
	c_port = 0;
}


/***************************************************************************//*!
* @fn baseClientServer::~baseClientServer()
* @brief Destructeur par defaut
*
* - Appel baseClientServer::endListen()
* - Si la platforme est windows => on décharge Winsock de la mémoire
*/
baseClientServer::~baseClientServer()
{
	IF_WINDOWS(
		// Arrêt de WinSock
		if( WSACleanup() == SOCKET_ERROR )
			stdErrorE("Erreur(%d) lors de l'arrêt de WinSock", SOCKET_REPPORT_ERROR);
	)
}


/***************************************************************************//*!
* @fn void baseClientServer::setPort( unsigned int port )
* @brief Modifier le port d'écoute
* @param[in] port Le port a utiliser de 1 à 65 536
* @note Par défaut, le port = 0 => port alléatoire.
*/
void baseClientServer::setPort( unsigned int port )
{
	if( port > 65536 )
		reportError("Vous ne pouvez pas indiquer un port > à 65 536");
	c_port = port;
}


/***************************************************************************//*!
* @fn void baseClientServer::setWait( long tv_sec, long tv_usec )
* @brief Modifier le temps d'attente d'une réponse
* @param[in] tv_sec Le temps en secs
* @param[in] tv_usec Le temps en ms
* @note Le temps d'attente par défaut est de 0.01sec
*/
void baseClientServer::setWait( long tv_sec, long tv_usec )
{
	if( tv_sec < 0 || tv_usec < 0 )
		reportError("Temps d'attente incorrect !");

	c_wait.tv_sec = tv_sec;
	c_wait.tv_usec = tv_usec;
}


/***************************************************************************//*!
* @fn int baseClientServer::readClient( SOCKET sock, char* buffer, unsigned int bufferSize )
* @brief Récupère les données envoyées par un client
* @param[in] sock Le socket du client
* @param[in,out] buffer La variables où seront stockées les données
* @param[in] bufferSize Taille du buffer
* @return Le nombre de byte reçut
*
* @note si retour = 0 => Client déconnecté (Le client est automatiquement viré de la liste dans ce cas la)<br />
* En cas d'erreur, le client sera déconnecté
*/
int baseClientServer::readClient( SOCKET sock, char* buffer, unsigned int bufferSize )
{
	int nbByte = recv(sock, buffer, bufferSize, 0);

	stdError("rcv %s", buffer);

	if( nbByte == SOCKET_ERROR ){
		stdError("Erreur(%d) lors de la lecture du client %d. Detail: <%s>", SOCKET_REPPORT_ERROR, sock, strerror(SOCKET_REPPORT_ERROR));
		nbByte = 0;// if recv error we disonnect the client
	}

	return nbByte;
}


/***************************************************************************//*!
* @fn void baseClientServer::send_message( SOCKET sock, const char* buffer, unsigned int bufferSize )
* @brief Envoie un message (buffer) au client
* @param[in] sock Le socket du client
* @param[in,out] buffer La variable sont stockées les données à envoyer
* @param[in] bufferSize Taille du buffer
*/
void baseClientServer::send_message( SOCKET sock, const char* buffer, unsigned int bufferSize )
{
	stdError("send %s", buffer);
	if( send(sock, buffer, bufferSize, 0) == SOCKET_ERROR )
	   stdError("Erreur(%d) lors de l'envoie du message. Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));
}


/***************************************************************************//*!
* @fn template<class Type> Type baseClientServer::reverseBit( Type var )
* @brief Inverse les bits.
* @param[in] var La variable a qui il faut inversé les bits
* @return Renvoie la {var} avec les bits inversé
* @warning FONCTION commenté car Innutile pour l'instant
*/
/*
template<class Type> Type baseClientServer::reverseBit( Type var )// Inverse les bits
{
	const unsigned int nbByte = sizeof(Type)*CHAR_BIT;

	Type out = 0;
	unsigned int i=0;

	do{
		out|= var&0x01;		//copie du bit de poids faible
		i++;				//compter les bits copiés
		if( i == nbByte)	//si tous les bits sont traités...
			break;			//...arrêter
		var >>= 1;			//vider vers la droite (préparer le suivant)...
		out<<= 1;			//...pour remplir vers la gauche (préparer la place)
	}while(1);

	return out;
}
*/


/***************************************************************************//*!
* @fn template<class Type> Type baseClientServer::reverseByte( Type var )
* @brief Conversion Little Endian <-> Big Endian (Inversion des octets)
* @param[in] var La variable a qui il faut inversé les octets
* @return Renvoie la {var} avec les octets inversé
*/
template<class Type> Type baseClientServer::reverseByte( Type var )// Inverse les octets
{
	Type out = 0;
	const	char* pIn = (const		char*) &var;
			char* pOut= (			char*)(&out+1)-1;

	for( unsigned int i=sizeof(Type); i>0 ; i-- )
	{
		*pOut-- = *pIn++;
	}
	return out;
}


/***************************************************************************//*!
* @fn bool baseClientServer::isLittleEndian(void)
* @brief Permet de savoir si l'on est sur une machine Intel, une machine qui lit
* les octects en Little Endian.
* @see baseClientServer::reverseByte( Type var )
*/
bool baseClientServer::isLittleEndian(void)// isIntel
{
	long l = 1;
	return (bool) *((char *)(&l));
}


/***************************************************************************//*!
* @fn void baseClientServer::setTCP_NoDelay( SOCKET s, bool activer )
* @brief Permet de modifier le paramètre TCP_NoDelay ( Algorithme de Nagle )
* @param[in] s Le socket a modifier
*/
void baseClientServer::setTCP_NoDelay( SOCKET s, bool activer )
{
	IF_WINDOWS(
		char use = (char)activer;
	)
	IF_LINUX(
		int use = (int)activer;
	)
	if( setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &use, sizeof(use)) == SOCKET_ERROR )
		reportError("Erreur(%d). Impossible de modifier l'utilisation de l'Algorithme de Nagle ! setsockopt(). Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));
}


/***************************************************************************//*!
* @fn bool baseClientServer::TCP_NoDelay( SOCKET s )
* @brief Permet de savoir l'état du paramètre TCP_NoDelay ( Algorithme de Nagle )
* @param[in] s Le socket a analyser
*/
bool baseClientServer::TCP_NoDelay( SOCKET s )
{
	IF_WINDOWS(
		int optLen = sizeof(char);
		char use = 0;
	)
	IF_LINUX(
		int use = 0;
		socklen_t optLen = sizeof(socklen_t);
	)
	if( getsockopt(s, IPPROTO_TCP, TCP_NODELAY, &use, &optLen) == SOCKET_ERROR )
		reportError("Erreur(%d). Impossible d'accèder au paramètre de l'Algorithme de Nagle ! getsockopt(). Detail: <%s>", SOCKET_REPPORT_ERROR, strerror(SOCKET_REPPORT_ERROR));

	return (bool)use;
}
