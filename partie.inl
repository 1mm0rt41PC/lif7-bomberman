/*******************************************************************************
*
*/
inline void partie::def_modeJeu( partie::t_MODE m )
{
	c_mode = m;
}


/***************************************************************************//*!
* @fn unsigned char partie::nbJoueurs() const
* @brief Retourne le nombre de joueur.
*/
inline unsigned char partie::nbJoueurs() const
{
	return c_nb_joueurs;
}


/*******************************************************************************
*
*/
inline partie::t_MODE partie::modeJeu() const
{
	return c_mode;
}


/***************************************************************************//*!
* @fn map* partie::getMap() const
* @brief Renvoie la map actuel
*/
inline map* partie::getMap() const
{
	return c_map;
}


/***************************************************************************//*!
* @fn partie::t_Connection partie::connection() const
* @brief Renvoie le type de connection: Host, Local, None ( pas de connection )
*/
inline partie::t_Connection partie::connection() const
{
	return c_connection;
}


/***************************************************************************//*!
* @fn unsigned char partie::getWinnerID() const
* @brief Renvoie l'ID du gagnant de la partie
* @warning Attention l'ID est comprise entre [1-c_nb_joueur] ! Si = 0 => Pas de winner !
*/
inline unsigned char partie::getWinnerID() const
{
	return c_winnerID;
}


/***************************************************************************//*!
* @fn time_t partie::timeOut() const
* @brief Renvoie le temps de fin / mort subite. (Ne change pas en cours de partie)
*/
inline time_t partie::timeOut() const
{
	return c_timeOut;
}


/***************************************************************************//*!
* @fn unsigned char partie::getUniqueJoueurID() const
* @brief Renvoie l'id du joueur de ce PC (client)
*/
inline unsigned char partie::getUniqueJoueurID() const
{
	return c_uniqueJoueurID;
}
