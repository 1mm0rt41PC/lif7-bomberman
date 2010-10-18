#include "options.h"

/*******************************************************************************
* Charge les options
* NB: Fichier stocké sous la forme : {port} {nb_touches} {clavier[1]} ... {clavier[i]}
* NB: fread( &DATA, sizeof(DATA), nb_DATA, *stream );
*/
options::options()
{
	FILE *fp=0;
	// Si on trouve notre fichier de configuration on le charge.
	// Sinon on prend la config par défaut
	if((fp=fopen("CONFIG_FILE", "rb"))){
		unsigned int nb_touches=0;

		/***********************************************************************
		* On récup le port
		*/
		if( fread( &c_port, sizeof(unsigned int), 1, fp ) < sizeof(unsigned int) ){
			// Si un bug lors de la lecture
			fclose(fp);
			perror("Erreur lors de la lecture du fichier CONFIG_FILE {port}");
			// On charge la config par défaut
			configParDefaut();
			return ;
		}

		/***********************************************************************
		* On récup le nombre de touches
		*/
		if( fread( &nb_touches, sizeof(unsigned int), 1, fp ) < sizeof(unsigned int) ){
			// Si un bug lors de la lecture
			fclose(fp);
			perror("Erreur lors de la lecture du fichier CONFIG_FILE {nb_touches}");
			// On charge la config par défaut
			configParDefaut();
			return ;
		}

		// Si nb_touches = 0 => BUG => reset_config !
		if( nb_touches == 0 ){
			fclose(fp);
			perror("Erreur lors de la lecture du fichier CONFIG_FILE {nb_touches == 0 !?} => resetData()");
			remise_a_zero();
			return ;
		}


		/***********************************************************************
		* On charge les touches dans les claviers
		*/
		for(unsigned int i=0; i<nb_touches; i++){
			if(!c_ClavierJoueur[i].chargerConfig( fp, nb_touches )){
				// Si un bug lors de la lecture
				fclose(fp);
				// On charge la config par defaut
				configParDefaut();
				return ;// On arrête tout
			}
		}
		fclose(fp);

	}else{
		// On charge la config par defaut
		configParDefaut();
	}
}


/*******************************************************************************
* Permet de charger la config par défaut ( port & clavier )
*/
void options::configParDefaut()
{
	c_port = DEFAULT_PORT;

	// Clavier Joueur 1
	c_ClavierJoueur[0].initClavier( 6 );// On veut 6 touches !
	c_ClavierJoueur[0].defTouche( clavier::haut, KEY_UP );
	c_ClavierJoueur[0].defTouche( clavier::bas, KEY_DOWN );
	c_ClavierJoueur[0].defTouche( clavier::droite, KEY_RIGHT );
	c_ClavierJoueur[0].defTouche( clavier::gauche, KEY_LEFT );
	c_ClavierJoueur[0].defTouche( clavier::lancerBombe, KEY_DC );// PADSTAR
	c_ClavierJoueur[0].defTouche( clavier::declancheur, KEY_END );// PADSLASH

	// Clavier Joueur 2
	c_ClavierJoueur[1].initClavier( 6 );// On veut 6 touches !
	c_ClavierJoueur[1].defTouche( clavier::haut, 'z' );
	c_ClavierJoueur[1].defTouche( clavier::bas, 's' );
	c_ClavierJoueur[1].defTouche( clavier::droite, 'd' );
	c_ClavierJoueur[1].defTouche( clavier::gauche, 'q' );
	c_ClavierJoueur[1].defTouche( clavier::lancerBombe, 't' );
	c_ClavierJoueur[1].defTouche( clavier::declancheur, 'r' );

	// Clavier Joueur 3
	c_ClavierJoueur[2].initClavier( 6 );// On veut 6 touches !
	c_ClavierJoueur[2].defTouche( clavier::haut, 'u' );
	c_ClavierJoueur[2].defTouche( clavier::bas, 'j' );
	c_ClavierJoueur[2].defTouche( clavier::droite, 'k' );
	c_ClavierJoueur[2].defTouche( clavier::gauche, 'h' );
	c_ClavierJoueur[2].defTouche( clavier::lancerBombe, 'p' );
	c_ClavierJoueur[2].defTouche( clavier::declancheur, 'o' );

	// Clavier Joueur 4
	c_ClavierJoueur[3].initClavier( 6 );// On veut 6 touches !
	c_ClavierJoueur[3].defTouche( clavier::haut, '5' );// Touche 5 Pad
	c_ClavierJoueur[3].defTouche( clavier::bas, '2' );// Touche 2 Pad ( Curses pas térrible pour les define ... )
	c_ClavierJoueur[3].defTouche( clavier::droite, '3' );// Touche 3 Pad
	c_ClavierJoueur[3].defTouche( clavier::gauche, '1' );// Touche 1 Pad
	c_ClavierJoueur[3].defTouche( clavier::lancerBombe, '0' );// Touche 0 Pad
	c_ClavierJoueur[3].defTouche( clavier::declancheur, '.' );// Touche Suppr/. Pad
}


/*******************************************************************************
* Remet et enregistre les paramètres par défaut
* ( Suppression du fichier de config )
*/
void options::remise_a_zero()
{
	if(fichierExiste("CONFIG_FILE")){
		if( remove("CONFIG_FILE") != 0 ){
			// On envoie un message sur le cannal
			// d'erreur si on a un problème
			perror("Erreur lors de la suppression du fichier CONFIG_FILE");
		}
	}

	// On charge la config par défaut
	configParDefaut();
}


/*******************************************************************************
* Permet de définir un port compris entre [0 et 9999].
*/
bool options::defPort( unsigned int port )
{
	if( port > 9999 ){// Il existe 65 536 mais pour l'affichage, on limite à 9999
		return 0;// On a un num > 9999 donc on le prend pas !
	}

	// Le nouveau port est différent donc on l'enregistre
	if( port != c_port ){
		c_port = port;
		enregistrerConfig();// On sav la config
	}
	return 1;
}


/*******************************************************************************
* Donne le port utilisé pour le jeu
*/
unsigned int options::port() const
{
	return c_port;
}


/*******************************************************************************
* Donne un clavier ( [0 à 3] )
*/
clavier* options::clavierJoueur( unsigned char clavier_numero )
{
	if(clavier_numero > 4)// Si le numero demandé est pas bon -> out
		return 0;

	return &c_ClavierJoueur[clavier_numero];
}


/*******************************************************************************
* Donne un clavier ( [0 à 3] )
* Simple alias
*/
clavier* options::operator[]( unsigned char clavier_numero )
{
	return clavierJoueur( clavier_numero );
}


/*******************************************************************************
* Permet d'enregistrer la config
* NB: Fichier stocké sous la forme : {port} {nb_touches} {clavier[1]} ... {clavier[i]}
* NB : fwrite( &DATA, sizeof(DATA), nb_DATA, *stream );
*/
void options::enregistrerConfig()
{
	FILE *fp=0;
	// Si on trouve notre fichier de configuration on le charge.
	// Sinon on prend la config par défaut
	if((fp=fopen("CONFIG_FILE", "wb"))){
		/***********************************************************************
		* Ecriture du port
		*/
		if( fwrite( &c_port, sizeof(unsigned int), 1, fp) < sizeof(unsigned int) ){
			// Si bug d'écriture on le signal et on stop tout !
			fclose(fp);
			perror("Erreur lors de l'enregistrement du fichier CONFIG_FILE {port}");
			return ;
		}


		unsigned int nb_touches = c_ClavierJoueur[0].nb_touches();// On a au minimum le joueur 1
		if(nb_touches == 0){// ON PEUT PAS AVOIR 0 TOUCHES !
			fclose(fp);
			perror("Erreur lors de l'enregistrement du fichier CONFIG_FILE {nb_touches == 0 !?} => resetData()");
			if( remove("CONFIG_FILE") != 0 ){
				// On envoie un message sur le cannal
				// d'erreur si on a un problème
				perror("Erreur lors de la suppression du fichier CONFIG_FILE");
			}
			return ;
		}

		/***********************************************************************
		* Ecriture du nombre de touches
		*/
		if( fwrite( &nb_touches , sizeof(unsigned int), 1, fp) < sizeof(unsigned int) ){
			// Si bug d'écriture on le signal et on stop tout !
			fclose(fp);
			perror("Erreur lors de l'enregistrement du fichier CONFIG_FILE {nb_touches}");
			return ;
		}


		/***********************************************************************
		* Ecriture des touches des claviers
		*/
		for(unsigned int i=0; i<nb_touches; i++){
			if(!c_ClavierJoueur[i].enregistrerConfig( fp )){
				// Si un bug lors de l'écriture
				fclose(fp);
				// On remet à zéro la config du fichier => del All !
				if( remove("CONFIG_FILE") != 0 ){
					// On envoie un message sur le cannal
					// d'erreur si on a un problème
					perror("Erreur lors de la suppression du fichier CONFIG_FILE");
				}
				return ;// On arrête tout
			}
		}

		fclose(fp);
	}
}

/*******************************************************************************
* Test si {fichier} existe
*/
bool options::fichierExiste( const char fichier[] ) const
{
	FILE *fp=0;
	if((fp=fopen(fichier, "r"))){
		fclose(fp);
		return 1;
	}
	return 0;
}
