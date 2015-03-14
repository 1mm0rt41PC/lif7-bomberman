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
* @fn void map::setBlock( s_Coordonnees& pos, map::t_type what, bonus::t_Bonus typeBonus=bonus::__RIEN__ )
* @brief Modifie le block qui est à la position X, Y
* @note Alias de setBlock( unsigned int X, unsigned int Y, map::t_type what )
*/
inline void map::setBlock( s_Coordonnees& pos, map::t_type what, bonus::t_Bonus typeBonus )
{
	setBlock(pos.x, pos.y, what, typeBonus);
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
* @fn const std::vector<s_Coordonnees>* getModifications() const
* @brief Retourne la liste des modifications.
* @warning NE pas oublier d'appeler map::cleanModifications() après totale utilisation de cette fonction
* @note Si vous n'utiliser cette fonction qu'a un seul endroit, pour une utilisation plus correcte de la liste, préférez l'utilisation de map::getModifications( s_Coordonnees& pos )
*/
inline const std::vector<s_Coordonnees>* map::getModifications() const
{
	return &c_listDesChangement;
}


/***************************************************************************//*!
* @fn const void map::cleanModifications()
* @brief Vide la liste des modifications.
* @see std::vector<s_Coordonnees>* getModifications()
*/
inline void map::cleanModifications()
{
	c_listDesChangement.clear();
}
