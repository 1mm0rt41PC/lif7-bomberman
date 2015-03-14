/***************************************************************************//*!
* @fn void clavier::defTouche( clavier::t_touche t, SYS_CLAVIER tsys )
* @brief D�finition d'une touche
* @param[in] t		L'actions que l'on veut affect� a la touche tsys
* @param[in] tsys	Touche qui sera affect� � l'action t
*/
inline void clavier::defTouche( clavier::t_touche t, SYS_CLAVIER tsys )
{
	c_touches[t] = tsys;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER clavier::touche( t_touche t ) const
* @brief Renvoie la touche affect� pour une action
* @param[in] t L'action dont on veut d�terminer la touche affect�e.
* @return Touche affect� � l'action
*/
inline SYS_CLAVIER clavier::touche( t_touche t ) const
{
	return c_touches[t];
}
