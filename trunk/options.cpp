#include "options.h"

// NOTE A MOI M�ME : NE PAS OUBLIER D'INITIALISER CETTE SALTE DE MEMBRE STATIC AINSI !!! ( principe singleton )
options* options::c_Instance = NULL;

/***************************************************************************//*!
* @fn options::options()
* @brief Initialise la class options
*
* Lit le fichier de configuration s'il existe, sinon on charge la config par d�faut
* @note Fichier stock� sous la forme : {port} {nb_touches} {clavier[1]} ... {clavier[i]}
* @note fread( &DATA, sizeof(DATA), nb_DATA, *stream ); (renvoie le nombre de varible correctement initialis�es)
* @warning NE PAS PASSER PAR LE CONSTRUCTEUR ! Utilisez options::getInstance()
* @see options::getInstance()
*/
options::options()
{
	FILE *fp=0;
	// Si on trouve notre fichier de configuration on le charge.
	// Sinon on prend la config par d�faut
	if( (fp=fopen(CONFIG_FILE, "rb")) ){
		unsigned int nb_touches=0;

		/***********************************************************************
		* On r�cup le port
		*/
		if( fread( &c_port, sizeof(unsigned int), 1, fp ) != 1 ){
			// Si un bug lors de la lecture, on charge la config par d�faut
			fclose(fp);
			stdErrorVar("Erreur lors de la lecture du fichier %s {port}", CONFIG_FILE);

			configParDefaut();
			return ;
		}

		/***********************************************************************
		* On r�cup le nombre de touches
		*/
		if( fread( &nb_touches, sizeof(unsigned int), 1, fp ) != 1 ){
			// Si un bug lors de la lecture, on charge la config par d�faut
			fclose(fp);
			stdErrorVar("Erreur lors de la lecture du fichier %s {nb_touches}", CONFIG_FILE);

			configParDefaut();
			return ;
		}

		// Si nb_touches = 0 => BUG => reset_config !
		if( nb_touches == 0 ){
			// Si un bug lors de la lecture, alors on RESET la config
			fclose(fp);
			stdErrorVar("Erreur lors de la lecture du fichier %s {nb_touches == 0 !?} => resetData()", CONFIG_FILE);

			remise_a_zero();
			return ;
		}


		/***********************************************************************
		* On charge les touches dans les claviers
		*/
		for( unsigned int i=0; i<4; i++ ){
			if( !c_ClavierJoueur[i].chargerConfig( fp, nb_touches ) ){
				// Si un bug lors de la lecture, on charge la config par d�faut
				fclose(fp);
				stdErrorVar("Erreur lors de la lecture du clavier <%u>", i);

				configParDefaut();
				return ;// On arr�te tout
			}
		}
		fclose(fp);

	}else{
		// On charge la config par defaut
		configParDefaut();
	}
}


/***************************************************************************//*!
* @fn options* options::getInstance()
* @brief Initialise la class options
* @return L'instance UNIQUE de la class.
*
* Si aucune instance de la class n'existe, alors appel au constructeur options::options()<br />
* S'il existe d�j� une instance de la class alors on renvoie simplement l'instance en cours.
*/
options* options::getInstance()
{
	if( !c_Instance )// Pas encore d'instance ? Bon bah on la cr�er
		c_Instance = new options();

	return c_Instance;
}


/***************************************************************************//*!
* @fn void options::uInit()
* @brief D�sinitialise la class options
*
* Si aucune instance de la class n'existe, alors cette fonction ne fait rien.
*/
void options::uInit()
{
	if( c_Instance ){
		//c_Instance->~options();// <- Pas besoin de �a ici, il n'y a pas de new
		delete c_Instance;
	}
	c_Instance = 0;
}


/***************************************************************************//*!
* @fn void options::configParDefaut()
* @brief Permet de charger la config par d�faut ( port & clavier )
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
	c_ClavierJoueur[3].defTouche( clavier::bas, '2' );// Touche 2 Pad ( Curses pas t�rrible pour les define ... )
	c_ClavierJoueur[3].defTouche( clavier::droite, '3' );// Touche 3 Pad
	c_ClavierJoueur[3].defTouche( clavier::gauche, '1' );// Touche 1 Pad
	c_ClavierJoueur[3].defTouche( clavier::lancerBombe, '0' );// Touche 0 Pad
	c_ClavierJoueur[3].defTouche( clavier::declancheur, '.' );// Touche Suppr/. Pad
}


/***************************************************************************//*!
* @fn void options::remise_a_zero()
* @brief Remet (et enregistre) les param�tres par d�faut
*
* Cette fonction effectue :
*	- Une suppression du fichier de configuration
*	- Un chargement des param�tres par d�faut ( options::configParDefaut() )
*/
void options::remise_a_zero()
{
	FILE *fp=0;
	// Si le fichier existe alors
	if( (fp=fopen(CONFIG_FILE, "r")) ){
		fclose(fp);
		// On le supprime
		supprimerFichierConfiguation();
	}

	// On charge la config par d�faut
	configParDefaut();
}


/***************************************************************************//*!
* @fn bool options::defPort( unsigned int port )
* @brief Permet de d�finir un port compris entre [0 et 9999].
* @param[in] port Le port a utiliser.
* @return Vrais si le port est acc�pt�
*/
bool options::defPort( unsigned int port )
{
	if( port > 9999 ){// Il existe 65 536 mais pour l'affichage, on limite � 9999
		return 0;// On a un num > 9999 donc on le prend pas !
	}

	// Le nouveau port est diff�rent donc on l'enregistre
	if( port != c_port ){
		c_port = port;
		enregistrerConfig();// On sav la config
	}
	return 1;
}


/***************************************************************************//*!
* @fn unsigned int options::port() const
* @brief Donne le port utilis� pour le jeu
* @return Le port utilis� actuellement
*/
unsigned int options::port() const
{
	return c_port;
}



/***************************************************************************//*!
* @fn clavier* options::clavierJoueur( unsigned char clavier_numero )
* @brief Renvoie le clavier num�ro {clavier_numero}
* @param[in] Le clavier que l'on veut ( un nombre entre [0 et 3] )
* @return Le port utilis� actuellement
*/
clavier* options::clavierJoueur( unsigned char clavier_numero )
{
	if( clavier_numero > 4 ){// Si le numero demand� est pas bon -> out
		stdErrorVar("options::clavierJoueur( %d )", (int)clavier_numero);
		return 0;
	}

	return &c_ClavierJoueur[clavier_numero];
}


/***************************************************************************//*!
* @fn clavier* options::operator[]( unsigned char clavier_numero )
* @brief Alias de options::clavierJoueur( unsigned char clavier_numero )
* @param[in] Le clavier que l'on veut ( un nombre entre [0 et 3] )
* @return Le port utilis� actuellement
*/
clavier* options::operator[]( unsigned char clavier_numero )
{
	return clavierJoueur( clavier_numero );
}


/***************************************************************************//*!
* @fn void options::enregistrerConfig()
* @brief Permet d'enregistrer la config
* @note Fichier stock� sous la forme : {port} {nb_touches} {clavier[1]} ... {clavier[i]}
* @note fwrite( &DATA, sizeof(DATA), nb_DATA, *stream ); (retourne le nombre de varaibles correctement �crites)
*/
void options::enregistrerConfig()
{
	FILE *fp=0;
	if( !(fp=fopen(CONFIG_FILE, "wb")) ){
		stdErrorVar("Erreur lors de l'ouverture du fichier <%s>", CONFIG_FILE);
		return ;
	}

	/***********************************************************************
	* Ecriture du port
	*/
	if( fwrite( &c_port, sizeof(unsigned int), 1, fp) != 1 ){
		// Si bug d'�criture on le signal et on stop tout !
		fclose(fp);
		stdErrorVar("Erreur lors de l'enregistrement du fichier <%s> {c_port=%d} => resetData()", CONFIG_FILE, c_port);

		// On remet � z�ro la config du fichier pour �viter les bug zarb lors d'une lecture => del All !
		supprimerFichierConfiguation();
		return ;
	}


	unsigned int nb_touches = c_ClavierJoueur[0].nb_touches();// On a au minimum le joueur 1
	if( nb_touches == 0 ){// ON PEUT PAS AVOIR 0 TOUCHES !
		fclose(fp);
		stdErrorVar("Erreur lors de l'enregistrement du fichier %s {nb_touches == 0 !?} => resetData()", CONFIG_FILE);

		// On remet � z�ro la config du fichier pour �viter les bug zarb lors d'une lecture => del All !
		supprimerFichierConfiguation();
		return ;
	}

	/***********************************************************************
	* Ecriture du nombre de touches
	*/
	if( fwrite( &nb_touches , sizeof(unsigned int), 1, fp) != 1 ){
		// Si bug d'�criture on le signal et on stop tout !
		fclose(fp);
		stdErrorVar("Erreur lors de l'enregistrement du fichier <%s> {nb_touches=%d;}", CONFIG_FILE, nb_touches);

		// On remet � z�ro la config du fichier pour �viter les bug zarb lors d'une lecture => del All !
		supprimerFichierConfiguation();
		return ;
	}


	/***********************************************************************
	* Ecriture des claviers
	*/
	for( unsigned char i=0; i<4; i++ ){
		if(! c_ClavierJoueur[i].enregistrerConfig(fp) ){
			// Si bug d'�criture on le signal et on stop tout !
			fclose(fp);
			stdErrorVar("Erreur lors de l'enregistrement du clavier <%d>", (int)i);

			// On remet � z�ro la config du fichier pour �viter les bug zarb lors d'une lecture => del All !
			supprimerFichierConfiguation();
			return ;// On arr�te tout
		}
	}

	fclose(fp);
}


/***************************************************************************//*!
* @fn void options::supprimerFichierConfiguation() const
* @brief Supprime le fichier de configuration
*/
void options::supprimerFichierConfiguation() const
{
	if( remove(CONFIG_FILE) != 0 )
		stdErrorVar("Erreur lors de la suppression du fichier <%s>", CONFIG_FILE);
}
