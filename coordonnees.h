#ifndef COORDONNEES_h
#define COORDONNEES_h

/*!
* @struct s_Coordonnees
* @brief struct de coordonnées ( x,y )
*/
typedef struct {
	unsigned int 	x,
					y;
} s_Coordonnees;



/*!
* @fn s_Coordonnees coordonneeConvert( unsigned int x, unsigned int y )
* @brief Permet de créer facillement une stucture s_Coordonnees initialisée
* @param[in] x Coordonnée X
* @param[in] y Coordonnée Y
*/
inline s_Coordonnees coordonneeConvert( unsigned int x, unsigned int y )
{
	s_Coordonnees pos;
	pos.x = x;
	pos.y = y;
	return pos;
}


/*!
* @fn inline void coordonneeConvert( unsigned int x, unsigned int y, s_Coordonnees& pos )
* @param pos La variable dans laquelle on va mettre X et Y
* @brief Permet de facillement initialisée une stucture s_Coordonnees
* @param[in] x Coordonnée X
* @param[in] y Coordonnée Y
* @param[in,out] pos Là où vont être stockées les coordonnées
*/
inline void coordonneeConvert( unsigned int x, unsigned int y, s_Coordonnees& pos )
{
	pos.x = x;
	pos.y = y;
}


/*!
* @fn bool coordonneeEgal( const s_Coordonnees& c1, const s_Coordonnees& c2 )
* @brief Permet comparer le contenue de 2 variables de type s_Coordonnees
* @param[in] c1 Coordonnées à comparer avec c2
* @param[in] c2 Coordonnées à comparer avec c1
*/
//#define coordonneeEgal( c1, c2 ) (c1.x == c2.x && c1.y == c2.y)
inline bool coordonneeEgal( const s_Coordonnees& c1, const s_Coordonnees& c2 )
{
	return ( c1.x == c2.x && c1.y == c2.y );
}

#endif
