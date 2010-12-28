/***************************************************************************//*!
* @fn void baseClientServer::setWait( const timeval* wait )
* @brief Modifier le temps d'attente d'une r�ponse
* @param[in] wait Le temps d'attente
* @note Le temps d'attente par d�faut est de 0.01sec<br />
* Cette fonction est un simple alias de baseClientServer::setWait( long tv_sec, long tv_usec )
*/
inline void baseClientServer::setWait( const timeval* wait )
{
	setWait( wait->tv_sec, wait->tv_usec );
}


/***************************************************************************//*!
* @fn unsigned int baseClientServer::port() const
* @brief Defini le port � utiliser.
* @note Le port n'est effectivement pri en compte qu'apr�s un appel � baseClientServer::Listening()
*/
inline unsigned int baseClientServer::port() const
{
	return c_port;
}


/***************************************************************************//*!
* @fn void baseClientServer::setTCP_NoDelay( SOCKET s, bool activer )
* @brief Permet de modifier le param�tre TCP_NoDelay ( Algorithme de Nagle )
* sur le socket c_listenSocket
*
* Cette fonction est un alias de baseClientServer::setTCP_NoDelay( SOCKET s, bool activer )
*/
inline void baseClientServer::setTCP_NoDelay( bool activer )
{
	setTCP_NoDelay( c_listenSocket, activer );
}


/***************************************************************************//*!
* @fn bool baseClientServer::TCP_NoDelay()
* @brief Permet de savoir l'�tat du param�tre TCP_NoDelay ( Algorithme de Nagle )
* pour le socket c_listenSocket
*
* Cette fonction est un alias de baseClientServer::TCP_NoDelay( SOCKET s )
*/
inline bool baseClientServer::TCP_NoDelay()
{
	return TCP_NoDelay( c_listenSocket );
}
