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
	this->c_block = new type[ tailleX * tailleY ];
	this->c_tailleX = tailleX;
	this->c_tailleY = tailleY;

	for(unsigned int i=0; i<tailleX*tailleY; i++)
	{
		//this->c_block[i] = map::type::vide;
	}
}


/*******************************************************************************
*
*/
map::~map()
{
	if(c_block)
	{
		delete this->c_block;
	}
	this->c_block = 0;
	this->c_tailleX = 0;
	this->c_tailleY = 0;
}


/*******************************************************************************
* Permet d'obtenir les information sur un block position X, Y
* -1 = ERREUR !
*/
map::type map::getBlock(unsigned int X, unsigned int Y)
{
	if(!c_block || X >= c_tailleX || Y >= c_tailleY)
	{
		return inconnu;
	}
	return c_block[X * c_tailleX + Y];
}


/*******************************************************************************
* Permet d'obtenir les information sur un block position X, Y
* -1 = ERREUR !
*/
void map::setBlock(unsigned int X, unsigned int Y, map::type what)
{
	if(!c_block || X >= c_tailleX || Y >= c_tailleY)
	{
		return ;
	}
	c_block[X * c_tailleX + Y] = what;
}
