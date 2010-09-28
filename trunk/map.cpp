#include "map.h"

/*******************************************************************************
*
*/
map::map()
{
	c_block = 0;
	c_tailleX = 0;
	c_tailleY = 0;
}


/*******************************************************************************
* Créer une map vide !
*/
map::map(unsigned int tailleX, unsigned int tailleY)
{
	c_block = new char[ tailleX * tailleY ];
	c_tailleX = tailleX;
	c_tailleY = tailleY;

	for(unsigned int i=0; i<tailleX*tailleY; i++)
	{
		c_block[i] = type::vide;
	}
}


/*******************************************************************************
*
*/
~map::map()
{
	if(c_block)
	{
		delete c_block;
	}
	c_block = 0;
	c_tailleX = 0;
	c_tailleY = 0;
}


/*******************************************************************************
* Permet d'obtenir les information sur un block position X, Y
* -1 = ERREUR !
*/
char map::getBlock(unsigned int X, unsigned int Y)
{
	if(!c_block || X >= c_tailleX || Y >= c_tailleY)
	{
		return -1;
	}
	return c_block[X * c_tailleX + Y];
}


/*******************************************************************************
* Permet d'obtenir les information sur un block position X, Y
* -1 = ERREUR !
*/
char map::setBlock(unsigned int X, unsigned int Y, char what)
{
	if(!c_block || X >= c_tailleX || Y >= c_tailleY)
	{
		return -1;
	}
	return c_block[X * c_tailleX + Y];
}
