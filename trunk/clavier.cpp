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
		perror("Erreur lors du chargement de la configuration du clavier !");
		return 0;
	}

	// Création des nouvelles touches
	c_nb_touches = nb_touches;
	c_touches = new SYS_CLAVIER[nb_touches];

	if( fread( c_touches, sizeof(SYS_CLAVIER), nb_touches, fp ) < sizeof(SYS_CLAVIER) ){
		// Remise à 0 des varriables
		c_nb_touches = 0;
		delete[] c_touches;
		c_touches = 0;

		perror("Erreur lors de la lecture du fichier de configuration pour {clavier[i]}");
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
* Charge la configuration du clavier depuis un fichier ( déjà ouvert ! )
* Retour:	1 si tout OK
*			0 si BUG
*/
bool clavier::enregistrerConfig( FILE* fp )
{
	if(!c_touches || !c_nb_touches){
		perror("Erreur lors de l'enregistrement de la configuration du clavier !");
		return 0;
	}
	if( fwrite( c_touches, sizeof(SYS_CLAVIER), c_nb_touches, fp ) < sizeof(SYS_CLAVIER) ){
		perror("Erreur lors de l'écriture du fichier de configuration pour {clavier[i]}");
		return 0;
	}
	return 1;

}
