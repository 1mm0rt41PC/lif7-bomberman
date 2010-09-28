#include "perso.h"

perso::perso()
{
	c_est_vivant = 1;
}


perso::perso(string nom, unsigned int Xpos, unsigned int Ypos)
{
	c_nom = nom
	c_X_pos = Xpos;
	c_Y_pos = Ypos;
}


perso::~perso()
{
	c_est_vivant = 0;
}

/*******************************************************************************
*
*/
void perso::setPos(unsigned int Xpos, unsigned int Ypos)
{
	c_X_pos = Xpos;
	c_Y_pos = Ypos;
}
