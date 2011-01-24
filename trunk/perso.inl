/***************************************************************************//*!
* @fn void perso::defNom( std::string nom )
* @brief Modifie le nom du personnage
*/
inline void perso::defNom( std::string nom )
{
	c_nom = nom;
}


/***************************************************************************//*!
* @fn void perso::defOrientation( t_Orientation ori )
* @brief Modif l'orientation du perso
*/
inline void perso::defOrientation( t_Orientation ori )
{
	c_orientation = ori;
}


/***************************************************************************//*!
* @fn void perso::defLocal( bool isLocal )
* @brief Permet de definir si ce perso est local ou distant
*/
inline void perso::defLocal( bool isLocal )
{
	c_isLocal = isLocal;
}


/***************************************************************************//*!
* @fn void perso::defSocket( SOCKET MySocket )
* @brief Permet de definir le socket utilisé par le perso
* @note Valeur par défaut: INVALID_SOCKET
*/
inline void perso::defSocket( SOCKET MySocket )
{
	c_MySocket = MySocket;
}


/***************************************************************************//*!
* @fn void perso::defNeed_Refresh( bool refresh )
* @brief Permet de definir si le perso a besoin d'avoir un raffraichissement graphique
*/
inline void perso::defNeed_Refresh( bool refresh )
{
	c_need_refresh = refresh;
}


/***************************************************************************//*!
* @fn const std::string* perso::nom() const
* @brief Renvoie le nom du personnage
*/
inline const std::string* perso::nom() const
{
	return &c_nom;
}


/***************************************************************************//*!
* @fn unsigned int perso::X() const
* @brief Renvoie la position X du perso
*/
inline unsigned int perso::X() const
{
	return c_pos.x;
}


/***************************************************************************//*!
* @fn unsigned int perso::Y() const
* @brief Renvoie la position Y du perso
*/
inline unsigned int perso::Y() const
{
	return c_pos.y;
}


/***************************************************************************//*!
* @fn unsigned int perso::old_X() const
* @brief Renvoie l'ancienne position X du perso
*/
inline unsigned int perso::old_X() const
{
	return c_OldPos.x;
}


/***************************************************************************//*!
* @fn unsigned int perso::old_Y() const
* @brief Renvoie l'ancienne position Y du perso
*/
inline unsigned int perso::old_Y() const
{
	return c_OldPos.y;
}


/***************************************************************************//*!
* @fn const s_Coordonnees* perso::old_Pos() const
* @brief Renvoie l'ancienne position du perso ( X et Y )
*/
inline const s_Coordonnees* perso::old_Pos() const
{
	return &c_OldPos;
}


/***************************************************************************//*!
* @fn bonus* perso::armements() const
* @brief Renvoie les armes et bonus du personnage
*/
inline bonus* perso::armements() const
{
	return c_armements;
}

/***************************************************************************//*!
* @fn perso::t_Orientation perso::orientation() const
* @brief Renvoie l'orientation du personnage
*/
inline perso::t_Orientation perso::orientation() const
{
	return c_orientation;
}


/***************************************************************************//*!
* @fn bool perso::isLocal() const
* @brief Renvoie l'état du personnage vis à vis du reseau. (Local ou non)
*/
inline bool perso::isLocal() const
{
	return c_isLocal;
}


/***************************************************************************//*!
* @fn SOCKET perso::socket() const
* @brief Renvoie le socket utilisé (Par defaut: INVALID_SOCKET)
*/
inline SOCKET perso::socket() const
{
	return c_MySocket;
}


/***************************************************************************//*!
* @fn void perso::need_refresh()
* @brief Renvoie si le perso a besoin d'avoir un raffraichissement graphique
*/
inline bool perso::need_refresh()
{
	if( !c_need_refresh )
		return false;
	c_need_refresh = 0;
	return true;
}


/***************************************************************************//*!
* @fn void perso::defOldPos( unsigned int Xpos, unsigned int Ypos )
* @brief Modifie l'ancienne position du personnage
* @warning Attention ! Une mausaise utilisation peut causer des erreurs graphiques !
*/
void perso::defOldPos( unsigned int Xpos, unsigned int Ypos )
{
	c_OldPos.x = Xpos;
	c_OldPos.y = Ypos;
}


/***************************************************************************//*!
* @fn void perso::defOldPos( const s_Coordonnees& OldPos )
* @brief Modifie l'ancienne position du personnage
* @warning Attention ! Une mausaise utilisation peut causer des erreurs graphiques !
*/
void perso::defOldPos( const s_Coordonnees& OldPos )
{
	c_OldPos = OldPos;
}
