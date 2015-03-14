/***************************************************************************//*!
* @fn std::vector<bonus::s_Event>* bonus::modEvent()
* @brief La liste des events non termin�
* @return Renvoie La liste des events non termin�
*/
inline std::vector<bonus::s_Event>* bonus::modEvent()
{
	return &c_listEvent;
}


/***************************************************************************//*!
* @fn unsigned int bonus::nbEvent() const
* @brief Renvoie le nombre d'event Trait� on non.
* @return Renvoie un bonus al�atoire
*/
inline unsigned int bonus::nbEvent() const
{
	return c_listEvent.size();
}


/***************************************************************************//*!
* @fn bool bonus::decQuantiteUtilisable( t_Bonus b, s_Coordonnees pos )
* @brief D�cr�mente quantite_utilisable
* @param[in] b Le bonus que l'on veut modifier
* @param[in] pos Position X, Y du bonus
* @return Renvoie vrais si on a pu d�cr�ment� la valeur
*
* Cette fonction permet d'activer le d�clancheur d'event du bonus
* @note Alias de la fonction bonus::decQuantiteUtilisable( t_Bonus b, unsigned int x, unsigned int y )
*/
inline bool bonus::decQuantiteUtilisable( t_Bonus b, s_Coordonnees pos )
{
	return decQuantiteUtilisable( b, pos.x, pos.y );
}
