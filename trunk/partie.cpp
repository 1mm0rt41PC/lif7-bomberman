#include "partie.h"

/*******************************************************************************
*
*/
partie::partie()
{
	c_nb_joueurs = 0;
	c_joueurs = 0;
	c_map = 0;
	//getkey(&c_nb_joueurs);
}


/*******************************************************************************
*
*/
partie::~partie()
{
	if(c_nb_joueurs && c_joueurs) delete c_joueurs;
}


/*******************************************************************************
*
*/
void partie::genMap()
{
/* NOT IMPLANTED */
}


/*******************************************************************************
*
*/
void partie::def_nbJoueurs( unsigned char nb )
{
	c_nb_joueurs = nb;
}


/*******************************************************************************
*
*/
void partie::def_nbMAX_joueurs( unsigned char nb )
{
	c_nb_MAX_joueurs = nb;
}


/*******************************************************************************
*
*/
void partie::def_modeJeu( partie::MODE m )
{
	c_mode = m;
}


/*******************************************************************************
*
*/
unsigned char partie::nbJoueurs() const
{
	return c_nb_joueurs;
}


/*******************************************************************************
*
*/
unsigned char partie::nbMAX_joueurs() const
{
	return c_nb_MAX_joueurs;
}


/*******************************************************************************
*
*/
partie::MODE partie::modeJeu() const
{
	return c_mode;
}
