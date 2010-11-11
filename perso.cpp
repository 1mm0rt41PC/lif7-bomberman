#include "perso.h"

using namespace std;

/*******************************************************************************
* Constructeur par defaut
*/
perso::perso()
{
	c_X_pos = 0;
	c_Y_pos = 0;
	c_armements = 0;
}


/***************************************************************************//*!
* @fn perso::perso( std::string nom, unsigned int Xpos, unsigned int Ypos )
* @brief Constructeur surchargé
*/
perso::perso( string nom, unsigned int Xpos, unsigned int Ypos )
{
	c_nom = nom;
	c_X_pos = Xpos;
	c_Y_pos = Ypos;
	c_armements = 0;
}


/*******************************************************************************
* Destructeur
*/
perso::~perso()
{
	if( c_armements )
		delete c_armements;
}


/***************************************************************************//*!
* @fn void perso::defNom( string nom )
* @brief Modifie le nom du personnage
*/
void perso::defNom( string nom )
{
	c_nom = nom;
}


/***************************************************************************//*!
* @fn void perso::defPos( unsigned int Xpos, unsigned int Ypos )
* @brief Modifie la position du personnage
*/
void perso::defPos( unsigned int Xpos, unsigned int Ypos )
{
	c_X_pos = Xpos;
	c_Y_pos = Ypos;
}


/***************************************************************************//*!
* @fn void perso::defY( unsigned int Ypos )
* @brief Permet de modifier la position Y du personnage
*/
void perso::defY( unsigned int Ypos )
{
	c_Y_pos = Ypos;
}


/***************************************************************************//*!
* @fn void perso::defX( unsigned int Xpos )
* @brief Permet de modifier la position X du personnage
*/
void perso::defX( unsigned int Xpos )
{
	c_X_pos = Xpos;
}


/***************************************************************************//*!
* @fn void perso::defArmements( bonus* a )
* @brief Modif l'armement du perso
*
* Modifie implicitement l'état : vivant / mort<br />
* Armé => vivant si (une ou plusieurs vie) ou (quantite_MAX_Ramassable = 0)<br />
* Désarmé => mort<br />
*/
void perso::defArmements( bonus* a )
{
	c_armements = a;
}


/***************************************************************************//*!
* @fn string perso::nom() const
* @brief Renvoie le nom du personnage
*/
string perso::nom() const
{
	return c_nom;
}


/***************************************************************************//*!
* @fn unsigned int perso::X() const
* @brief Renvoie la position X du perso
*/
unsigned int perso::X() const
{
	return c_X_pos;
}


/***************************************************************************//*!
* @fn unsigned int perso::Y() const
* @brief Renvoie la position Y du perso
*/
unsigned int perso::Y() const
{
	return c_Y_pos;
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


/***************************************************************************//*!
* @fn bonus* perso::armements() const
* @brief Renvoie les armes et bonus du personnage
*/
bonus* perso::armements() const
{
	return c_armements;
}
