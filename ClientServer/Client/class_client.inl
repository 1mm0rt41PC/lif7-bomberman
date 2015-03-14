/***************************************************************************//*!
* @fn void client::send_message( const char* buffer, unsigned int bufferSize )
* @brief Envoie un message (buffer) au serveur
* @param[in,out] buffer La variable sont stockées les données à envoyer
* @param[in] bufferSize Taille du buffer
* @note alias de baseClientServer::send_message( SOCKET sock, const char* buffer, unsigned int bufferSize )
*/
inline void client::send_message( const char* buffer, unsigned int bufferSize )
{
	send_message( c_listenSocket, buffer, bufferSize );
}


/***************************************************************************//*!
* @fn const IN_ADDR* client::serverAdress() const
* @brief Renvoie le socket utilisé pour la connection avec le serveur
*/
inline const IN_ADDR* client::serverAdress() const
{
	return &c_serverAdress;
}
