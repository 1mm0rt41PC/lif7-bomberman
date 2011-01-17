#include "perso.h"
#include "debug.h"

using namespace std;

/*******************************************************************************
* Constructeur par defaut
*/
perso::perso()
{
	c_pos.x = 0;
	c_pos.y = 0;
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
	c_armements = 0;
	c_orientation = ORI_bas;
	c_isLocal = true;
	c_MySocket = INVALID_SOCKET;
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
* @fn void perso::defNom( std::string nom )
* @brief Modifie le nom du personnage
*/
void perso::defNom( std::string nom )
{
	c_nom = nom;
}


/***************************************************************************//*!
* @fn void perso::defPos( unsigned int Xpos, unsigned int Ypos )
* @brief Modifie la position du personnage
*/
void perso::defPos( unsigned int Xpos, unsigned int Ypos )
{
	c_pos.x = Xpos;
	c_pos.y = Ypos;
}


/***************************************************************************//*!
* @fn void perso::defPos( s_Coordonnees pos )
* @brief Modifie la position du personnage
*/
void perso::defPos( s_Coordonnees pos )
{
	c_pos = pos;
}


/***************************************************************************//*!
* @fn void perso::defY( unsigned int Ypos )
* @brief Permet de modifier la position Y du personnage
*/
void perso::defY( unsigned int Ypos )
{
	c_pos.y = Ypos;
}


/***************************************************************************//*!
* @fn void perso::defX( unsigned int Xpos )
* @brief Permet de modifier la position X du personnage
*/
void perso::defX( unsigned int Xpos )
{
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
* @fn void perso::defOrientation( t_Orientation ori )
* @brief Modif l'orientation du perso
*/
void perso::defOrientation( t_Orientation ori )
{
	c_orientation = ori;
}


/***************************************************************************//*!
* @fn void perso::defLocal( bool isLocal )
* @brief Permet de definir si ce perso est local ou distant
*/
void perso::defLocal( bool isLocal )
{
	c_isLocal = isLocal;
}


/***************************************************************************//*!
* @fn void perso::defSocket( SOCKET MySocket )
* @brief Permet de definir le socket utilisé par le perso
* @note Valeur par défaut: INVALID_SOCKET
*/
void perso::defSocket( SOCKET MySocket )
{
	c_MySocket = MySocket;
}


/***************************************************************************//*!
* @fn const string* perso::nom() const
* @brief Renvoie le nom du personnage
*/
const string* perso::nom() const
{
	return &c_nom;
}


/***************************************************************************//*!
* @fn unsigned int perso::X() const
* @brief Renvoie la position X du perso
*/
unsigned int perso::X() const
{
	return c_pos.x;
}


/***************************************************************************//*!
* @fn unsigned int perso::Y() const
* @brief Renvoie la position Y du perso
*/
unsigned int perso::Y() const
{
	return c_pos.y;
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

/***************************************************************************//*!
* @fn perso::t_Orientation perso::orientation() const
* @brief Renvoie l'orientation du personnage
*/
perso::t_Orientation perso::orientation() const
{
	return c_orientation;
}


/***************************************************************************//*!
* @fn bool perso::isLocal() const
* @brief Renvoie l'état du personnage vis à vis du reseau. (Local ou non)
*/
bool perso::isLocal() const
{
	return c_isLocal;
}


/***************************************************************************//*!
* @fn SOCKET perso::socket() const
* @brief Renvoie le socket utilisé (Par defaut: INVALID_SOCKET)
*/
SOCKET perso::socket() const
{
	return c_MySocket;
}
