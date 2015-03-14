/***************************************************************************//*!
* @fn std::vector<bonus::s_Event>* bonus::modEvent()
* @brief La liste des events non terminé
* @return Renvoie La liste des events non terminé
*/
inline std::vector<bonus::s_Event>* bonus::modEvent()
{
	return &c_listEvent;
}


/***************************************************************************//*!
* @fn unsigned int bonus::nbEvent() const
* @brief Renvoie le nombre d'event Traité on non.
* @return Renvoie un bonus aléatoire
*/
inline unsigned int bonus::nbEvent() const
{
	return c_listEvent.size();
}


/***************************************************************************//*!
* @fn bool bonus::decQuantiteUtilisable( t_Bonus b, s_Coordonnees pos )
* @brief Décrémente quantite_utilisable
* @param[in] b Le bonus que l'on veut modifier
* @param[in] pos Position X, Y du bonus
* @return Renvoie vrais si on a pu décrémenté la valeur
*
* Cette fonction permet d'activer le déclancheur d'event du bonus
* @note Alias de la fonction bonus::decQuantiteUtilisable( t_Bonus b, unsigned int x, unsigned int y )
*/
inline bool bonus::decQuantiteUtilisable( t_Bonus b, s_Coordonnees pos )
{
	return decQuantiteUtilisable( b, pos.x, pos.y );
}
