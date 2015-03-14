/***************************************************************************//*!
* @fn void clavier::defTouche( clavier::t_touche t, SYS_CLAVIER tsys )
* @brief Définition d'une touche
* @param[in] t		L'actions que l'on veut affecté a la touche tsys
* @param[in] tsys	Touche qui sera affecté à l'action t
*/
inline void clavier::defTouche( clavier::t_touche t, SYS_CLAVIER tsys )
{
	c_touches[t] = tsys;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER clavier::touche( t_touche t ) const
* @brief Renvoie la touche affecté pour une action
* @param[in] t L'action dont on veut déterminer la touche affectée.
* @return Touche affecté à l'action
*/
inline SYS_CLAVIER clavier::touche( t_touche t ) const
{
	return c_touches[t];
}
