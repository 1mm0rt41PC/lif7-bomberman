#include "clavier.h"
#include "debug.h"

/***************************************************************************//*!
* @fn clavier::clavier()
* @brief Initialisation du syst�me de gestion du clavier
* @see clavier::clavier( unsigned int nb_touches )
*/
clavier::clavier()
{
	c_touches = 0;
	c_nb_touches = 0;
}


/***************************************************************************//*!
* @fn clavier::~clavier()
* @brief D�sinitialisation du systeme de gestion du clavier
*/
clavier::~clavier()
{
	if( !c_touches || !c_nb_touches )
		return ;
	delete[] c_touches;
	c_touches = 0;
	c_nb_touches = 0;
}


/***************************************************************************//*!
* @fn void clavier::initClavier( unsigned int nb_touches )
* @brief D�finition du nombre de touche
* @param[in] nb_touches Le nombre d'actions a g�rer (Haut + bas + droit + gauche => 4 actions soit 4 touches)
*/
void clavier::initClavier( unsigned int nb_touches )
{
	// On supprime les anciennes touches si elles existaient
	if( c_touches && c_nb_touches )
		delete[] c_touches;

	// Cr�ation des nouvelles touches
	c_nb_touches = nb_touches;
	c_touches = new SYS_CLAVIER[nb_touches];
}


/***************************************************************************//*!
* @fn void clavier::defTouche( clavier::t_touche t, SYS_CLAVIER tsys )
* @brief D�finition d'une touche
* @param[in] t		L'actions que l'on veut affect� a la touche tsys
* @param[in] tsys	Touche qui sera affect� � l'action t
*/
void clavier::defTouche( clavier::t_touche t, SYS_CLAVIER tsys )
{
	c_touches[t] = tsys;
}


/***************************************************************************//*!
* @fn bool clavier::chargerConfig( FILE* fp, unsigned int nb_touches )
* @brief Charge la configuration du clavier depuis un fichier ( d�j� ouvert ! )
* @param[in,out] fp		Un pointeur vers un fichier d�j� ouvert ( mode binaire )
* @param[in] nb_touches	Nombre de touches qui vont �tre charg�s
* @return true si tout s'est bien pass�
* @todo Faire en sorte que l'on ne soit pas oblig� de mettre le nombre de touche<br />
* et que cela => lecture du nombre de touche avant
*/
bool clavier::chargerConfig( FILE* fp, unsigned int nb_touches )
{
	if( !fp || !nb_touches ){
		stdError("Erreur lors du chargement de la configuration du clavier ! {fp=%X; nb_touches=%u}\n", (unsigned int)fp, nb_touches);
		return 0;
	}

	// Cr�ation des nouvelles touches
	c_nb_touches = nb_touches;
	c_touches = new SYS_CLAVIER[nb_touches];

	if( fread(c_touches, sizeof(SYS_CLAVIER), nb_touches, fp) != nb_touches ){
		// Remise � 0 des varriables
		c_nb_touches = 0;
		delete[] c_touches;
		c_touches = 0;

		stdError("Erreur lors de la lecture du fichier de configuration pour {clavier[i]}\n");
		return 0;
	}
	return 1;
}


/***************************************************************************//*!
* @fn t_touche clavier::obtenirTouche( SYS_CLAVIER tsys ) const
* @brief D�termine si la touche tsys envoy�e fait partie des actions de ce clavier
* @param[in] tsys La touche que dont on veux connaitre l'appartenance vis � vis de ce clavier
* @return l'action associ�e a la touche ( NUL => Touches n'appartenant pas � ce clavier )
*/
clavier::t_touche clavier::obtenirTouche( SYS_CLAVIER tsys ) const
{
	for( unsigned int i=0; i<c_nb_touches; i++ )
	{
		if( c_touches[(clavier::t_touche)i] == tsys )
			return (clavier::t_touche)i;
	}
	return clavier::NUL;
}


/***************************************************************************//*!
* @fn unsigned int clavier::nb_touches() const
* @brief Renvoie le nombre d'actions possibles sur ce clavier.
* @return Renvoie le nombre d'actions possibles sur ce clavier.
*
* Renvoie donc aussi le nombre de touches g�r�es par ce clavier.
*/
unsigned int clavier::nb_touches() const
{
	return c_nb_touches;
}


/***************************************************************************//*!
* @fn bool clavier::estDansClavier( SYS_CLAVIER tsys ) const
* @brief Permet de d�terminer si la touche tsys est dans la liste des touches de cette configuration
* @param[in] tsys La touche qui est a comparer avec celles qui appartiennent aux touches de ce clavier (configuration)
* @return true si la touche demand� est dans cette configuration
*/
bool clavier::estDansClavier( SYS_CLAVIER tsys ) const
{
	for( unsigned int i=0; i<c_nb_touches; i++ )
		if( c_touches[i] == tsys )
			return true;

	return false;
}


/***************************************************************************//*!
* @fn SYS_CLAVIER clavier::touche( t_touche t ) const
* @brief Renvoie la touche affect� pour une action
* @param[in] t L'action dont on veut d�terminer la touche affect�e.
* @return Touche affect� � l'action
*/
SYS_CLAVIER clavier::touche( t_touche t ) const
{
	return c_touches[t];
}


/***************************************************************************//*!
* @fn bool clavier::enregistrerConfig( FILE* fp )
* @brief Charge la configuration du clavier depuis un fichier ( d�j� ouvert ! )
* @param[in,out] fp Un pointeur sur un fichier d�j� ouvert ( mode binaire )
* @return true si l'enregistrement de la configuation s'est bien pass�
* @todo Mettre un champs optionel permettant de laisser le choix quand � l'enregistrement du nombre de touches
*/
bool clavier::enregistrerConfig( FILE* fp )
{
	if( !c_touches || !c_nb_touches ){
		stdError("Erreur lors de l'enregistrement de la configuration du clavier ! {c_touches=%X; c_nb_touches=%u}\n", (unsigned int)c_touches, c_nb_touches);
		return 0;
	}
	if( fwrite(c_touches, sizeof(SYS_CLAVIER), c_nb_touches, fp) != c_nb_touches ){
		stdError("Erreur lors de l'�criture du fichier de configuration pour {clavier[i]}\n");
		return 0;
	}
	return 1;
}
