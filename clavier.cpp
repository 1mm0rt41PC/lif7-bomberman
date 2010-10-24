#include "clavier.h"

/*******************************************************************************
* Initialisation du systeme de gestion du clavier
*/
clavier::clavier()
{
	c_touches = 0;
	c_nb_touches = 0;
}


/*******************************************************************************
* Désinitialisation du systeme de gestion du clavier
*/
clavier::~clavier()
{
	if(!c_touches || !c_nb_touches) return ;
	delete[] c_touches;
	c_touches = 0;
	c_nb_touches = 0;
}


/*******************************************************************************
* Définition du nombre de touche
*/
void clavier::initClavier( unsigned int nb_touches )
{
	// On supprime les anciennes touches si elles existaient
	if(c_touches && c_nb_touches) delete c_touches;

	// Création des nouvelles touches
	c_nb_touches = nb_touches;
	c_touches = new SYS_CLAVIER[nb_touches];
}


/*******************************************************************************
* Définition d'une touche
*/
void clavier::defTouche(clavier::t_touche t, SYS_CLAVIER tsys )
{
	c_touches[t] = tsys;
}


/*******************************************************************************
* Charge la configuration du clavier depuis un fichier ( déjà ouvert ! )
* Retour:	1 si tout OK
*			0 si BUG
*/
bool clavier::chargerConfig( FILE* fp, unsigned int nb_touches )
{
	if(!fp || !nb_touches){
		fprintf(stderr, "Erreur lors du chargement de la configuration du clavier ! {fp=%d; nb_touches=%d}\n", (int)fp, nb_touches);
		return 0;
	}

	// Création des nouvelles touches
	c_nb_touches = nb_touches;
	c_touches = new SYS_CLAVIER[nb_touches];

	if( fread( c_touches, sizeof(SYS_CLAVIER), nb_touches, fp ) != nb_touches ){
		// Remise à 0 des varriables
		c_nb_touches = 0;
		delete[] c_touches;
		c_touches = 0;

		fprintf(stderr, "Erreur lors de la lecture du fichier de configuration pour {clavier[i]}\n");
		return 0;
	}
	return 1;
}

/*******************************************************************************
* Détection si une touche est appuyée
*/
bool clavier::estAppuyer( SYS_CLAVIER tsys ) const
{
	for(unsigned int i=0; i<c_nb_touches; i++){
		if( c_touches[(clavier::t_touche)i] == tsys ) return 1;
	}

	return 0;
}


/*******************************************************************************
* Renvoie le nombre de touche pour ce clavier
*/
unsigned int clavier::nb_touches() const
{
	return c_nb_touches;
}


/*******************************************************************************
* @fn SYS_CLAVIER* clavier::touche( t_touche t );
*
* Renvoie la touche affecté pour une action
* @param Action t_touche
* @return Touche affecté à l'action
*/
SYS_CLAVIER clavier::touche( t_touche t ) const
{
	return c_touches[t];
}


/*******************************************************************************
* Charge la configuration du clavier depuis un fichier ( déjà ouvert ! )
* Retour:	1 si tout OK
*			0 si BUG
*/
bool clavier::enregistrerConfig( FILE* fp )
{
	if(!c_touches || !c_nb_touches){
		fprintf(stderr, "Erreur lors de l'enregistrement de la configuration du clavier ! {c_touches=%d; c_nb_touches=%d}\n", (int)c_touches, c_nb_touches);
		return 0;
	}
	if( fwrite( c_touches, sizeof(SYS_CLAVIER), c_nb_touches, fp ) != c_nb_touches ){
		fprintf(stderr, "Erreur lors de l'écriture du fichier de configuration pour {clavier[i]}\n");
		return 0;
	}
	return 1;

}
