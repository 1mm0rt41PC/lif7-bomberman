#include "outils.h"

/***************************************************************************//*!
* @fn int myRand( int a, int b )
* @brief Créer un nombre aléatoire compris entre a et b
*/
int myRand( int a, int b )
{
	double r;

	r = (double) rand() / RAND_MAX;
	r *= (double) b+1.-a;
	r += (double) a;
	if( (int)r > b )
		return b;
	return (int) r;
}


/***************************************************************************//*!
* @fn unsigned char getTailleNombre( int nb )
* @brief Calcule la longueur d'un nombre.
* @param[in] nb Le nombre dont on veut savoir la taille
* @return Renvoie le nombre de chiffre qui compose le nombre
*
* Exemple getTailleNombre(-10) => 3<br />
*/
unsigned char getTailleNombre( int nb )
{
	if( nb < 0 )// On met le nombre en positif si ce n'est pas le cas
		return getTailleNombre( (unsigned int) (nb*(-1)) )+1;// +1 pour le symbole négatif

	return getTailleNombre( (unsigned int) nb );
}


/***************************************************************************//*!
* @fn unsigned char getTailleNombre( unsigned int nb )
* @brief Calcule la longueur d'un nombre.
* @param[in] nb Le nombre dont on veut savoir la taille
* @return Renvoie le nombre de chiffre qui compose le nombre
*
* Exemple getTailleNombre(122) => 3
*/
unsigned char getTailleNombre( unsigned int nb )
{
	unsigned char taille_NB;
	for( taille_NB=1; nb>=10; taille_NB++ )
		nb /=10;

	return taille_NB;
}


/***************************************************************************//*!
* @fn char* trimString( char texte[] )
* @brief Vire les espaces avant et après une chaine de caractère
* @param[in,out] texte Le texte a trim. Le texte est directement modifié
* @return Renvoie le pointeur @e texte
*
* Exemple : "  toto " => "toto"
*/
char* trimString( char texte[] )
{
	unsigned int	posDebut=0,
					posFin=strlen(texte)-2;// -2 car -1 => '\0' et 0 => out du tableau

	// On remplis tous les espaces qui sont en fin de chaine de '\0'
	while( isspace(*(texte+posFin)) && texte[posFin] && posFin>0 )
	{
		texte[posFin] = 0;
		posFin--;
	}

	// On trouve la 1ère lettre qui n'est pas un espace
	while( isspace(*(texte+posDebut)) && texte[posDebut] && ++posDebut>0 );

	// On déplace les caractères
	return strcpy(texte, texte+posDebut);
}


#if defined(OS_WINDOWS)
/***************************************************************************//*!
* @fn int gettimeofday( struct timeval* tp, void* tz )
* @brief Renvoie l'heure actuel
* @param[in,out]	tp Un pointeur sur une structure timeval existante. tp va être
* modifié et contiendra l'heure actuel.
* @param			tz Paramètre inutilisé ! METTRE NULL !
* @return Renvoie toujours 0
*/
int gettimeofday( struct timeval* tp, void* tz )
{
	(void)tz;
	struct _timeb timebuffer;
	_ftime(&timebuffer);
	tp->tv_sec = timebuffer.time;
	tp->tv_usec = timebuffer.millitm * 1000;
	return 0;
}
#endif


/***************************************************************************//*!
* @fn int getSeconde()
* @brief Renvoie l'heure actuel en seconde
* @return Renvoie l'heure actuel en seconde
*/
int getSeconde()
{
	timeval tmp = {0,0};
	gettimeofday(&tmp, 0);
	return tmp.tv_sec;
}
