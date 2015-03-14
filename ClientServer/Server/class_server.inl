/***************************************************************************//*!
* @fn void server::setNbClientMax( unsigned int nbClient )
* @brief Defini le nombre maximum de client simultané
*/
inline void server::setNbClientMax( unsigned int nbClient )
{
	c_nb_Max_Client = nbClient;
}


/***************************************************************************//*!
* @fn void server::rmClient( SOCKET s )
* @brief Supprime le client qui a le socket s
* @param[in] s Le socket a supprimer de la liste
*
* Alias de server::rmClientID( server::getClientID( s ) );
*/
inline void server::rmClient( SOCKET s )
{
	rmClientID( getClientID( s ) );
}


/***************************************************************************//*!
* @fn unsigned int server::nb_Max_Client() const
* @brief Retourne le nombre maximum de client simultané
* @note Si = 0 => infini
*/
inline unsigned int server::nb_Max_Client() const
{
	return c_nb_Max_Client;
}


/***************************************************************************//*!
* @fn unsigned int server::nb_clientConnecter() const
* @brief Retourne le nombre de client actuellement connecté
*/
inline unsigned int server::nb_clientConnecter() const
{
	return c_nb_clientConnecter;
}


/***************************************************************************//*!
* @fn const server::sClient* server::listClient() const
* @brief Renvoie le tableau des client actuellement connecté
*/
inline const server::sClient* server::listClient() const
{
	return c_listClient;
}


/***************************************************************************//*!
* @fn bool server::lookupConnectionClient( SOCKET client )
* @brief Recherche une activité pour le client
* @param[in] client Le socket du client dont on veut voir l'activité
* @return True si le client a une une activité, False sinon
*
* @warning Veuillez executer server::lookupNewConnection() avant !
*/
inline bool server::lookupConnectionClient( SOCKET client )
{
	return FD_ISSET(client, &c_rdfs);
}
