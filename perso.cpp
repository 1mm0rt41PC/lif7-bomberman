#include "perso.h"
#include "debug.h"

using namespace std;

/***************************************************************************//*!
* @fn perso::perso()
* @brief Constructeur par defaut
*/
perso::perso()
{
	c_pos.x = 0;
	c_pos.y = 0;
	c_OldPos.x = 0;
	c_OldPos.y = 0;
	c_armements = 0;
	c_orientation = ORI_bas;
	c_isLocal = true;
	c_MySocket = INVALID_SOCKET;
}


/***************************************************************************//*!
* @fn perso::perso( std::string nom, unsigned int Xpos, unsigned int Ypos )
* @brief Constructeur surchargé
*/
perso::perso( std::string nom, unsigned int Xpos, unsigned int Ypos )
{
	c_nom = nom;
	c_pos.x = Xpos;
	c_pos.y = Ypos;
	c_OldPos.x = Xpos;
	c_OldPos.y = Ypos;
	c_armements = 0;
	c_orientation = ORI_bas;
	c_isLocal = true;
	c_MySocket = INVALID_SOCKET;
}


/***************************************************************************//*!
* @fn perso::~perso()
* @brief Destructeur par defaut
*/
perso::~perso()
{
	if( c_armements )
		delete c_armements;
	c_armements = 0;
}


/***************************************************************************//*!
* @fn void perso::defPos( unsigned int Xpos, unsigned int Ypos )
* @brief Modifie la position du personnage
*/
void perso::defPos( unsigned int Xpos, unsigned int Ypos )
{
	//stdError("Anti-bug SDL");// Anti-bug SDL

	//stdError("X=%u, Y=%u || OLD_X=%u, OLD_Y=%u", Xpos, Ypos, c_OldPos.x, c_OldPos.y);

	if( c_OldPos.x+1 == Xpos || c_OldPos.x-1 == Xpos || c_OldPos.x == Xpos )// Pour eviter d'avoir des saut de plusieurs blocks
		c_OldPos.x = c_pos.x;
	else
		c_OldPos.x = Xpos;

	if( c_OldPos.y+1 == Ypos || c_OldPos.y-1 == Ypos || c_OldPos.y == Ypos )// Pour eviter d'avoir des saut de plusieurs blocks
		c_OldPos.y = c_pos.y;
	else
		c_OldPos.y = Ypos;

	//stdError("Final OLD_X=%u, OLD_Y=%u", c_OldPos.x, c_OldPos.y);

	c_pos.x = Xpos;
	c_pos.y = Ypos;
}


/***************************************************************************//*!
* @fn void perso::defPos( const s_Coordonnees& pos )
* @brief Modifie la position du personnage
*/
void perso::defPos( const s_Coordonnees& pos )
{
	if( c_OldPos.x+1 == pos.x || c_OldPos.x-1 == pos.x || c_OldPos.x == pos.x )// Pour eviter d'avoir des saut de plusieurs blocks
		c_OldPos.x = c_pos.x;
	else
		c_OldPos.x = pos.x;
	if( c_OldPos.y+1 == pos.y || c_OldPos.y-1 == pos.y || c_OldPos.y == pos.y )// Pour eviter d'avoir des saut de plusieurs blocks
		c_OldPos.y = c_pos.y;
	else
		c_OldPos.y = pos.y;

	c_OldPos = c_pos;
	c_pos = pos;
}


/***************************************************************************//*!
* @fn void perso::defY( unsigned int Ypos )
* @brief Permet de modifier la position Y du personnage
*/
void perso::defY( unsigned int Ypos )
{
	if( c_OldPos.y+1 == Ypos || c_OldPos.y-1 == Ypos || c_OldPos.y == Ypos )// Pour eviter d'avoir des saut de plusieurs blocks
		c_OldPos.y = c_pos.y;
	else
		c_OldPos.y = Ypos;

	c_pos.y = Ypos;
}


/***************************************************************************//*!
* @fn void perso::defX( unsigned int Xpos )
* @brief Permet de modifier la position X du personnage
*/
void perso::defX( unsigned int Xpos )
{
	if( c_OldPos.x+1 == Xpos || c_OldPos.x-1 == Xpos || c_OldPos.x == Xpos )// Pour eviter d'avoir des saut de plusieurs blocks
		c_OldPos.x = c_pos.x;
	else
		c_OldPos.x = Xpos;

	c_pos.x = Xpos;
}


/***************************************************************************//*!
* @fn void perso::defArmements( bonus* a )
* @brief Modif l'armement du perso
*
* Modifie implicitement l'état : vivant / mort<br />
* Armé => vivant<br />
* Désarmé => mort<br />
*/
void perso::defArmements( bonus* a )
{
	if( c_armements )
		delete c_armements;
	c_armements = a;
}


/***************************************************************************//*!
* @fn bool perso::estVivant() const
* @brief Renvoie si le perso est vivant ou pas
*
* <b>Info</b> : Un perso est vivant si il dispose d'un armement et de
* [ Soit une quantitéRamassable de vie = à 0 ] [ Soit un nombre de vie > 0]
*/
bool perso::estVivant() const
{
	if( c_armements && (c_armements->quantiteMAX_Ramassable(bonus::vie) == 0 || c_armements->quantiteUtilisable(bonus::vie) >= 1) )
		return 1;
	return 0;
}
