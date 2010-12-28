// ! Pas d'include map.h ici !

/***************************************************************************//*!
* @fn unsigned int map::X() const
* @brief Renvoie la taille X de la map
*/
inline unsigned int map::X() const
{
	return c_taille.x;
}


/***************************************************************************//*!
* @fn unsigned int map::Y() const
* @brief Renvoie la taille Y de la map
*/
inline unsigned int map::Y() const
{
	return c_taille.y;
}


/***************************************************************************//*!
* @fn const map::s_Case* map::getBlock( s_Coordonnees& pos ) const
* @brief Retourn le block qui est à la position X, Y
* @note Alias de map::getBlock( unsigned int X, unsigned int Y )
*/
inline const map::s_Case* map::getBlock( s_Coordonnees& pos ) const
{
	return getBlock( pos.x, pos.y );
}


/***************************************************************************//*!
* @fn void map::setBlock( s_Coordonnees& pos, map::t_type what )
* @brief Modifie le block qui est à la position X, Y
* @note Alias de setBlock( unsigned int X, unsigned int Y, map::t_type what )
*/
inline void map::setBlock( s_Coordonnees& pos, map::t_type what )
{
	setBlock(pos.x, pos.y, what);
}


/***************************************************************************//*!
* @fn unsigned char map::nb_PointDeDepartJoueur() const
* @brief Renvoie le nombre de joueur maximum sur la carte en cours.
*/
inline unsigned char map::nb_PointDeDepartJoueur() const
{
	return c_nb_PointDeDepartJoueur;
}


/***************************************************************************//*!
* @fn unsigned char map::nb_MAX_Joueur() const
* @brief Alias de map::nb_PointDeDepartJoueur()
*/
inline unsigned char map::nb_MAX_Joueur() const
{
	return nb_PointDeDepartJoueur();
}


/***************************************************************************//*!
* @fn const std::vector<s_Coordonnees>* readModification() const
* @brief Retourne la liste des modifications
* @note Pour une utilisation plus correcte de la liste, préférez l'utilisation de map::getModification( s_Coordonnees& pos )
*/
inline const std::vector<s_Coordonnees>* map::readModification() const
{
	return &c_listDesChangement;
}
