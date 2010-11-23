#ifndef COORDONNEES_h
#define COORDONNEES_h

/*!
* @enum s_Coordonnees
* @brief struct de coordonnées ( x,y )
*/
typedef struct {
	unsigned int 	x,
					y;
} s_Coordonnees;



/*!
* @fn s_Coordonnees coordonneeConvert( unsigned int x, unsigned int y )
* @brief Permet de créer facillement une stucture s_Coordonnees initialisée
*/
inline s_Coordonnees coordonneeConvert( unsigned int x, unsigned int y )
{
	s_Coordonnees pos;
	pos.x = x;
	pos.y = y;
	return pos;
}


/*!
* @fn bool coordonneeEgal( s_Coordonnees& c1, s_Coordonnees& c2 )
* @brief Permet comparer le contenue de 2 variables de type s_Coordonnees
*/
//#define coordonneeEgal( c1, c2 ) (c1.x == c2.x && c1.y == c2.y)
inline bool coordonneeEgal( s_Coordonnees& c1, s_Coordonnees& c2 )
{
	return ( c1.x == c2.x && c1.y == c2.y );
}

#endif
