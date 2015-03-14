#include "clavier.h"
#include "debug.h"

/***************************************************************************//*!
* @fn clavier::clavier()
* @brief Initialisation du syst�me de gestion du clavier
* @see clavier::clavier( unsigned int nb_touches )
*/
clavier::clavier()
{
	for( unsigned int i=0; i<NB_ELEMENT_t_touche; i++ )
		c_touches[i] = (SYS_CLAVIER)0;
}


/***************************************************************************//*!
* @fn clavier::~clavier()
* @brief D�sinitialisation du systeme de gestion du clavier
*/
clavier::~clavier()
{
	for( unsigned int i=0; i<NB_ELEMENT_t_touche; i++ )
		c_touches[i] = (SYS_CLAVIER)0;
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
	if( !fp || !nb_touches || nb_touches != NB_ELEMENT_t_touche ){
		stdError("Erreur lors du chargement de la configuration du clavier ! {fp=%X; nb_touches=%u, NB_ELEMENT_t_touche=%us}", (unsigned int)fp, nb_touches, NB_ELEMENT_t_touche);
		return 0;
	}

	// Cr�ation des nouvelles touches
	if( fread(c_touches, sizeof(SYS_CLAVIER), NB_ELEMENT_t_touche, fp) != NB_ELEMENT_t_touche ){
		// Remise � 0 des varriables
		for( unsigned int i=0; i<NB_ELEMENT_t_touche; i++ )
			c_touches[i] = (SYS_CLAVIER)0;

		stdError("Erreur lors de la lecture du fichier de configuration pour {clavier[i]}");
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
	for( unsigned int i=0; i<NB_ELEMENT_t_touche; i++ )
	{
		if( c_touches[(clavier::t_touche)i] == tsys )
			return (clavier::t_touche)i;
	}
	return clavier::NUL;
}


/***************************************************************************//*!
* @fn bool clavier::estDansClavier( SYS_CLAVIER tsys ) const
* @brief Permet de d�terminer si la touche tsys est dans la liste des touches de cette configuration
* @param[in] tsys La touche qui est a comparer avec celles qui appartiennent aux touches de ce clavier (configuration)
* @return true si la touche demand� est dans cette configuration
*/
bool clavier::estDansClavier( SYS_CLAVIER tsys ) const
{
	for( unsigned int i=0; i<NB_ELEMENT_t_touche; i++ )
		if( c_touches[i] == tsys )
			return true;

	return false;
}


/***************************************************************************//*!
* @fn bool clavier::enregistrerConfig( FILE* fp ) const
* @brief Charge la configuration du clavier depuis un fichier ( d�j� ouvert ! )
* @param[in,out] fp Un pointeur sur un fichier d�j� ouvert ( mode binaire )
* @return true si l'enregistrement de la configuation s'est bien pass�
* @todo Mettre un champs optionel permettant de laisser le choix quand � l'enregistrement du nombre de touches
*/
bool clavier::enregistrerConfig( FILE* fp ) const
{
	if( !fp ){
		stdError("Erreur lors de l'enregistrement de la configuration du clavier ! {fp=0}");
		return 0;
	}
	if( fwrite(c_touches, sizeof(SYS_CLAVIER), NB_ELEMENT_t_touche, fp) != NB_ELEMENT_t_touche ){
		stdError("Erreur lors de l'�criture du fichier de configuration pour {clavier[i]}");
		return 0;
	}
	return 1;
}
