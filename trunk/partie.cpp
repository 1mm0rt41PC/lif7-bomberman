#include "partie.h"
#include "debug.h"

typedef std::string string;

/***************************************************************************//*!
* @fn partie::partie()
* @brief Initialise la class partie
*
* Pas de new ici
*/
partie::partie()
{
	c_connection = CNX_None;
	c_nb_joueurs = 0;
	c_joueurs = 0;
	c_map = 0;

	c_server = 0;
	//c_client = 0;// <- Pas besoin ( union )

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize+1);
}


/***************************************************************************//*!
* @fn partie::~partie()
* @brief D�sinitialise la class partie et supprime tous les joueurs et la map
*/
partie::~partie()
{
	if( c_nb_joueurs && c_joueurs )
		delete[] c_joueurs;

	if( c_map )
		delete c_map;

	c_joueurs = 0;
	c_map = 0;

	if( c_connection == CNX_Host && c_server ){
		delete c_server;
		c_server = 0;
	}

	if( c_connection == CNX_Client && c_client ){
		delete c_client;
		c_client = 0;
	}
}


/***************************************************************************//*!
* @fn void partie::genMap()
* @brief Charge une map
*
* S'il existe d�j� une map -> On vire l'ancienne map<br />
* Apr�s avoir charg� la map, la fonction positionne les joueurs<br />
* <b>/!\\ Initialiser les perso en 1er !!! /!\\</b>
*/
void partie::genMap()
{
	if( !c_nb_joueurs || !c_joueurs )
		stdErrorE("Initialiser les perso avant la map !!! c_nb_joueurs(%u) c_joueurs(%u)", (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);

	if( c_map )
		delete c_map;
	c_map = new map();
	c_map->chargerMap();

	s_Coordonnees p;
	for( unsigned char i=0; i<c_nb_joueurs; i++ )
	{
		if( c_joueurs[i].isLocal() || (!c_joueurs[i].isLocal() && c_joueurs[i].socket() != INVALID_SOCKET) ){
			p = c_map->mettreJoueurA_sa_PositionInitial( i+1 );
			c_joueurs[i].defPos(p.x, p.y);
			c_joueurs[i].defOldPos(p);
		}
	}
}


/***************************************************************************//*!
* @fn void partie::def_nbJoueurs( unsigned char nb )
* @brief Definie le nombre de joueur.
*
* S'il existe d�j� des joueurs -> On vire les anciens joueurs
*/
void partie::def_nbJoueurs( unsigned char nb )
{
	if( c_nb_joueurs && c_joueurs ){
		//stdError("c_nb_joueurs(%u), c_joueurs(%X)", (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);
		delete[] c_joueurs;
	}
	if(	!nb	)
		stdErrorE("def_nbJoueurs(0) Interdit !");

	c_nb_joueurs = nb;
	c_joueurs = new perso[nb];
}


/***************************************************************************//*!
* @fn void partie::def_connection( partie::t_Connection cnx )
* @brief Permet de modifier la connection: Host, Local, None ( pas de connection )
*/
void partie::def_connection( partie::t_Connection cnx )
{
	c_connection = cnx;
	if( cnx == CNX_Host )
		c_server = new server;

	if( cnx == CNX_Client )
		c_client = new client;
}


/***************************************************************************//*!
* @fn perso* partie::joueur( unsigned int joueur_numero ) const
* @brief Renvoie un joueur
* @param joueur_numero l'id du joueur [0 � ... ]
* @return 0 en cas de bug ! Un pointeur vers le joueur demand� sinon.
*/
perso* partie::joueur( unsigned int joueur_numero ) const
{
	if( joueur_numero >= c_nb_joueurs || !c_joueurs ){
		stdError("joueur_numero(%u) >= c_nb_joueurs(%u) || !c_joueurs(%X)", (unsigned int)joueur_numero, (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);
		return 0;
	}
	return c_joueurs+joueur_numero;
}


/***************************************************************************//*!
* @fn unsigned char partie::nbJoueurVivant() const
* @brief Renvoie le nombre de joueur en vie
*/
unsigned char partie::nbJoueurVivant() const
{
	unsigned char nbJoueurVivant = 0;
	for( unsigned char i=0; i<c_nb_joueurs; i++ )
	{
		if( (c_joueurs[i].isLocal()
			|| (
				!c_joueurs[i].isLocal()
				&& c_joueurs[i].socket() != INVALID_SOCKET
				)
			)
			&& c_joueurs[i].armements()
		)
			nbJoueurVivant++;
	}
	return nbJoueurVivant;
}


/***************************************************************************//*!
* @fn unsigned char partie::nbJoueursReel() const
* @brief Retourne le nombre de joueur r�el. (Lors de mode connect�)
*/
unsigned char partie::nbJoueursReel() const
{
	unsigned char nbJoueursReel = 0;
	for( unsigned char i=0; i<c_nb_joueurs; i++ )
	{
		if( c_joueurs[i].isLocal() ){
			nbJoueursReel++;
		}else{
			if( c_joueurs[i].socket() != INVALID_SOCKET )
				nbJoueursReel++;
		}
	}
	return nbJoueursReel;
}


/***************************************************************************//*!
* @fn server* partie::getServeur() const
* @brief Renvoie le serveur utilis� pour communiquer.
* @return Le pointeur vers le serveur si tout est OK. NULL sinon
*/
server* partie::getServeur() const
{
	if( c_connection == CNX_Host )
		return c_server;

	stdError("Attention ! Cette partie n'est pas configur�e en tant que Host !");
	return NULL;
}


/***************************************************************************//*!
* @fn client* partie::getClient() const
* @brief Renvoie le client utilis� pour communiquer.
* @return Le pointeur vers le client si tout est OK. NULL sinon
*/
client* partie::getClient() const
{
	if( c_connection == CNX_Client )
		return c_client;

	stdError("Attention ! Cette partie n'est pas configur�e en tant que Client !");
	return NULL;
}


/***************************************************************************//*!
* @fn char partie::main( libAff * afficherMapEtEvent )
* @brief Lance le jeu
* @param[in] afficherMapEtEvent La fonction qui va servir � afficher la map
* @return	Un nombre > � 1 : Le nombre retourn� est le joueur qui a gagn�.<br />
*			0 : Match nul
*			-1: Retour au menu
*			-10: Probl�me
*
* @warning Toutes les variables doivent �tre correctement initialis�es !
*/
char partie::main( libAff * afficherMapEtEvent )
{
	if( !afficherMapEtEvent )
		stdErrorE("Fonction d'affichage incorrect ! (afficherMapEtEvent=0)");

	SYS_CLAVIER key;
	unsigned int i=0;
	unsigned char j=0;
	bool continuerScanClavier=1;
	options* opt = options::getInstance();
	bonus::s_Event bonusEvent;
	s_EventBombe e;
	SOCKET s;
	uint32_t X=0, Y=0;// Utilis� uniquement lors de partie online ( host & client )
	clavier::t_touche onlineClavier;// Utilis� uniquement lors de partie online ( host & client )
	c_winnerID = 0;// Aucune joueur n'a gagn� par d�faut
	c_timerAttak = 0;
	bool envoyerInfoJoueur = 0;

	// On initialise les bonus utilisables pour cette partie
	bonus::bonusProp();

	/*##########################################################################
	* Partie en mode client
	*/
	if( c_connection == CNX_Client ){
		// Teste sur les types
		if( sizeof(uint8_t) != sizeof(unsigned char) )
			stdErrorE("sizeof(uint8_t){%u} != sizeof(unsigned char){%u}", sizeof(uint8_t), sizeof(unsigned char));

		// Tentative de connection
		if( !c_client->connection() ){
			stdError("Impossible de se connecter au serveur demand� !");
			return -10;
		}

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Envoie de la config 32Bits?, IsIntel?
		sprintf(c_buffer, "%d %d %d %d", __BOMBERMAN_VERSION__, Is32Bits, (int)baseClientServer::isLittleEndian(), PACK_bufferSize);
		c_client->send_message(c_buffer, PACK_bufferSize);

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Attente de la r�ponce
		if( !c_client->readServer(c_buffer, PACK_bufferSize) ){
			stdError("Configuration 32Bits, IsIntel incorrect pour le serveur ! Connection refus� !");
			return -10;// On a �t� d�connect� !
		}

		// Envoie du nom
		if( c_nb_joueurs > 1 )
			c_client->send_message(c_joueurs[c_uniqueJoueurID].nom()->c_str(), c_joueurs[0].nom()->length());
		else
			c_client->send_message(c_joueurs[0].nom()->c_str(), c_joueurs[0].nom()->length());

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Attente de la r�ponce
		if( !c_client->readServer(c_buffer, PACK_bufferSize) ){
			stdError("Nom d�j� utilis� ! Connection refus� !");
			return -10;// On a �t� d�connect� !
		}

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Attente de la dimention de la map
		if( !c_client->readServer(c_buffer, PACK_bufferSize) ){
			stdError("Erreur de connection !");
			return -10;// On a �t� d�connect� !
		}

		// D�finition des variables qui seront utilis�es
		map::t_type type;

		unPackIt( &X, &Y, &type );// type inutile ici

		if( !X || !Y )
			stdErrorE("Taille de map incorrect !!! TailleX=%u, TailleY=%u, <%s>", X, Y, c_buffer);

		if( c_map )
			delete c_map;
		if( !(c_map = new map(X, Y)) )// Cr�ation de la map
			stdErrorE("Erreur lors de l'allocation m�moire !");

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer

		// R�c�ption de TOUTE la map et des autres infos
		while( c_client->readServer(c_buffer, PACK_bufferSize) != 0 && c_buffer[0] != '0' )
		{
			if( !c_buffer[0] ){// Paquet vide...
				stdError("Paquet vide !");
				baseClientServer::printAllBuffer( c_buffer, PACK_bufferSize, __FILE__, __LINE__ );

				// Analyse du paquet
				for( i=0; i<PACK_bufferSize; i++ )
				{
					if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){
						if( c_buffer[i+1] == ':' )
							strcpy(c_buffer, c_buffer+i);
						break;
					}
				}
				// Esqu'il nous manque une partie du paquet ?
				if( c_buffer[strlen(c_buffer)-1] == '�' ){
					stdError("Paquet r�cup�r�: <%s>", c_buffer);
				}else{
					stdError("Impossible de r�cup�rer le paquet: <%s>", c_buffer);
				}
			}

			// Esqu'il nous manque une partie du paquet ?
			if( c_buffer[strlen(c_buffer)-1] != '�' ){
				stdError("Il manque une partie du Paquet ! Paquet actuel: <%s>", c_buffer);
				while( c_client->readServer(c_buffer+strlen(c_buffer), PACK_bufferSize-strlen(c_buffer)) != 0 && c_buffer[strlen(c_buffer)-1] != '�' );
				stdError("Paquet final: <%s>", c_buffer);

				// Esqu'il nous manque une partie du paquet ?
				if( c_buffer[strlen(c_buffer)-1] == '�' ){
					stdError("Paquet r�cup�r�: <%s>", c_buffer);
				}else{
					stdError("Impossible de r�cup�rer le paquet: <%s>", c_buffer);
					continue;
				}
			}

			switch( c_buffer[0] )
			{
				case '1': {// Map
					unPackIt( &X, &Y, &type );
					c_map->setBlock(X, Y, map::vide);
					c_map->setBlock(X, Y, type);
					break;
				}
				case '2': {// Meta donn�es
					unPackIt(X, Y);
					break;
				}
				case '5': {// Nombre de joueur
					def_nbJoueurs(c_buffer[2]-'0');
					break;
				}
				case '6':
				case '7': {// Infos Joueur
					unPackIt();
					break;
				}
				case '9': {// Timer
					sscanf(c_buffer, "9:%ld�", &c_timeOut);
					c_timeOut += clock();
					break;
				}
				default:{
					stdError("Donn�e incorrect ! %s", c_buffer);
					break;
				}
			}
			memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		}

		if( !c_client->is_connected() ){
			stdError("Probl�me de d�connection avec le serveur !");
			return -10;
		}

		// Commencement de la partie
		do{
			// Affichage de la map et r�c�ption des Event claviers
			key = afficherMapEtEvent( this );

			switch( opt->clavierJoueur(0)->obtenirTouche(key) )
			{
				case clavier::NUL: {
					break;
				}
				case clavier::haut:
				case clavier::bas:
				case clavier::droite:
				case clavier::gauche:
				case clavier::lancerBombe:
				case clavier::declancheur: {
					if( c_client->is_connected() )
						c_client->send_message(packIt(opt->clavierJoueur(0)->obtenirTouche(key)), PACK_bufferSize);// Envoie de notre touche
					break;
				}
				default: {
					stdError("Touche envoy� par le joueur %d est inconnue : %d", (int)opt->clavierJoueur(i)->obtenirTouche(key), (int)key);
					break;
				}
			}

			/*******************************************************************
			* Reception des donn�es r�seaux
			*/
			if( c_client->lookupConnection() ){// A faire en 1er !
				memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
				while( c_buffer[0] != '4' && c_client->readServer(c_buffer, PACK_bufferSize) != 0 && c_buffer[0] != '0' )
				{
					if( !c_buffer[0] ){// Paquet vide...
						stdError("Paquet vide !");
						baseClientServer::printAllBuffer( c_buffer, PACK_bufferSize, __FILE__, __LINE__ );

						// Analyse du paquet
						for( i=0; i<PACK_bufferSize; i++ )
						{
							if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){
								if( c_buffer[i+1] == ':' )
									strcpy(c_buffer, c_buffer+i);
								break;
							}
						}
						// Esqu'il nous manque une partie du paquet ?
						if( c_buffer[strlen(c_buffer)-1] == '�' ){
							stdError("Paquet r�cup�r�: <%s>", c_buffer);
						}else{
							stdError("Impossible de r�cup�rer le paquet: <%s>", c_buffer);
						}
					}

					// Esqu'il nous manque une partie du paquet ?
					if( c_buffer[strlen(c_buffer)-1] != '�' ){
						stdError("Il manque une partie du Paquet ! Paquet actuel: <%s>", c_buffer);
						while( c_client->readServer(c_buffer+strlen(c_buffer), PACK_bufferSize-strlen(c_buffer)) != 0 && c_buffer[strlen(c_buffer)-1] != '�' );
						stdError("Paquet final: <%s>", c_buffer);

						// Esqu'il nous manque une partie du paquet ?
						if( c_buffer[strlen(c_buffer)-1] == '�' ){
							stdError("Paquet r�cup�r�: <%s>", c_buffer);
						}else{
							stdError("Impossible de r�cup�rer le paquet: <%s>", c_buffer);
							continue;
						}
					}

					switch( c_buffer[0] )
					{
						case '1': {// Map
							unPackIt( &X, &Y, &type );
							c_map->setBlock(X, Y, map::vide);
							c_map->setBlock(X, Y, type);
							memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
							break;
						}
						case '2': {// Meta donn�es
							unPackIt(X, Y);
							memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
							break;
						}
						case '4': {// Fin de partie
							c_buffer[strlen(c_buffer)-1] = 0;
							c_winnerID = c_buffer[2]-'0';
							break;
						}
						case '5': {// Nombre de joueur
							if( c_buffer[2]-'0' > c_nb_joueurs )
								def_nbJoueurs(c_buffer[2]-'0');
							break;
						}
						case '6':
						case '7': {// Infos Joueur
							unPackIt();
							break;
						}
						case '8': {// Supprimer le Joueur
							c_joueurs[c_buffer[2]-'0'].defLocal(false);
							c_joueurs[c_buffer[2]-'0'].defSocket(INVALID_SOCKET);
							c_joueurs[c_buffer[2]-'0'].defArmements(0);
							break;
						}
						case '9': {// Timer
							sscanf(c_buffer, "9:%ld�", &c_timeOut);
							c_timeOut += clock();
							break;
						}
						default:
							break;
					}
				}
			}

		}while( key != RETOUR_MENU_PRECEDENT && c_client->is_connected() && c_buffer[0] != '4' );

		c_client->disconnection();

		bonus::unInitBonusProp();

		return c_winnerID;
	}

	/*##########################################################################
	* Partie en mode serveur ou offline
	*/
	// On initialise les bonus pour les joueurs
	for( unsigned char i=0; i<c_nb_joueurs; i++ )
	{
		c_joueurs[i].defArmements( new bonus() );
		c_joueurs[i].armements()->param_Par_Defaut();
		c_joueurs[i].defNeed_Refresh(1);
	}

	// On g�n�re la map
	genMap();


	if( c_connection == CNX_Host ){// Si on est en mode serveur alors on connecte le serveur
		// On d�termine le nombre maximum de player online
		unsigned char nbLocal=nbJoueursReel();

		// Ajustement des param�tres
		c_server->setNbClientMax(c_nb_joueurs-nbJoueursReel());
		c_server->Listening();

		// Teste sur les types
		if( sizeof(uint8_t) != sizeof(unsigned char) )
			stdErrorE("sizeof(uint8_t){%u} != sizeof(unsigned char){%u}", sizeof(uint8_t), sizeof(unsigned char));

		// S'il n'y a qu'un perso en local, alors on wait un perso exterieur
		if( nbLocal == 1 ){
			c_server->setWait(0,100000);// 0.1 secondes
			// Ajout des player
			while( (s = c_server->lookupNewConnection()) == INVALID_SOCKET && key != RETOUR_MENU_PRECEDENT ){
				c_timeOut = clock();
				key = afficherMapEtEvent( this );// Pour r�cup les touches
			}

			if( key == RETOUR_MENU_PRECEDENT )
				return -1;

			// On a enfin un player, on lui envoie la map
			ajouterNouvelleConnection( s );
		}
		c_server->setWait(0,1000);// 0.001 secondes
	}
	//##########################################################################
	//##########################################################################

	c_timeOut = clock() + 2*60*CLOCKS_PER_SEC+30*CLOCKS_PER_SEC;// 2min:30

	if( c_connection == CNX_Host ){// Si on est en mode serveur alors on connecte le serveur
		// Envoie du timer � tout les players !
		for( i=0; i<c_nb_joueurs; i++ )
		{
			if( !c_joueurs[i].isLocal() && c_joueurs[i].socket() != INVALID_SOCKET ){
				// Envoie du timer
				sprintf(c_buffer, "9:%ld�", c_timeOut-clock());
				c_server->send_message( c_joueurs[i].socket(), c_buffer, PACK_bufferSize);
				strcpy(c_buffer, "0:end");
				c_server->send_message( s, c_buffer, PACK_bufferSize);// Fin de transmition
			}
		}
	}

	//-----------------------> Commencement de la partie <----------------------
	do{
		// Affichage de la map et r�c�ption des Event
		key = afficherMapEtEvent( this );

		/***********************************************************************
		* On parcours les joueurs
		*/
		for( i=0; i<c_nb_joueurs; i++ )
		{
			// Si le joueur est mort, on saut son scan
			if( !c_joueurs[i].armements() ) continue ;

			/*******************************************************************
			* Scan des Events bonus de chaques joueurs
			*/
			bool killPlayer = 0;
			while( c_joueurs[i].armements()->isEvent(&bonusEvent) )// Tanqu'il y a des event on tourne
			{
				switch( bonusEvent.type )
				{
					case bonus::SIGNAL_kill: {
						killPlayer = 1;
						break;
					}
					case bonus::bombe: {
						e.joueur = i+1;
						e.pos = bonusEvent.pos;
						e.repetionSuivante = 0;
						e.Nb_Repetition = 0;
						e.Nb_Repetition_MAX = c_joueurs[i].armements()->quantiteUtilisable(bonus::puissance_flamme);
						e.continue_X = true;
						e.continue_negativeX = true;
						e.continue_Y = true;
						e.continue_negativeY = true;
						c_listEventBombe.push_back( e );
						break;
					}
					case bonus::inversseur_touche: {
						c_joueurs[i].armements()->decQuantiteUtilisable(bonus::inversseur_touche);
						break;
					}
					default:
						stdError("Event bonus non pris en charge bonus=%d", bonusEvent.type);
						break;
				}
			}
			// On a re�ut le signal kill => On kill le perso
			if( killPlayer )
				c_joueurs[i].defArmements(0);

			/*******************************************************************
			* Scan du clavier pour les joueurs
			*/
			if( continuerScanClavier == 1 && c_joueurs[i].isLocal() )
			switch( opt->clavierJoueur(i)->obtenirTouche(key) )
			{
				case clavier::NUL: {
					continuerScanClavier = 1;
					break;
				}
				case clavier::haut: {
					if( c_joueurs[i].armements()->quantiteUtilisable(bonus::inversseur_touche) ){
						c_joueurs[i].defOrientation(perso::ORI_bas);
						deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()+1, i );
					}else{
						c_joueurs[i].defOrientation(perso::ORI_haut);
						deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()-1, i );
					}
					// Suplace pour afficher les mouvement de rotation lorsque le perso ne bouge pas vraiment
					c_map->setBlock(c_joueurs[i].X(), c_joueurs[i].Y(), c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
					continuerScanClavier = 0;
					envoyerInfoJoueur = 1;
					break;
				}
				case clavier::bas: {
					if( c_joueurs[i].armements()->quantiteUtilisable(bonus::inversseur_touche) ){
						c_joueurs[i].defOrientation(perso::ORI_haut);
						deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()-1, i );
					}else{
						c_joueurs[i].defOrientation(perso::ORI_bas);
						deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()+1, i );
					}
					// Suplace pour afficher les mouvement de rotation lorsque le perso ne bouge pas vraiment
					c_map->setBlock(c_joueurs[i].X(), c_joueurs[i].Y(), c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
					continuerScanClavier = 0;
					envoyerInfoJoueur = 1;
					break;
				}
				case clavier::droite: {
					if( c_joueurs[i].armements()->quantiteUtilisable(bonus::inversseur_touche) ){
						c_joueurs[i].defOrientation(perso::ORI_gauche);
						deplacer_le_Perso_A( c_joueurs[i].X()-1, c_joueurs[i].Y(), i );
					}else{
						c_joueurs[i].defOrientation(perso::ORI_droite);
						deplacer_le_Perso_A( c_joueurs[i].X()+1, c_joueurs[i].Y(), i );
					}
					// Suplace pour afficher les mouvement de rotation lorsque le perso ne bouge pas vraiment
					c_map->setBlock(c_joueurs[i].X(), c_joueurs[i].Y(), c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
					continuerScanClavier = 0;
					envoyerInfoJoueur = 1;
					break;
				}
				case clavier::gauche: {
					if( c_joueurs[i].armements()->quantiteUtilisable(bonus::inversseur_touche) ){
						c_joueurs[i].defOrientation(perso::ORI_droite);
						deplacer_le_Perso_A( c_joueurs[i].X()+1, c_joueurs[i].Y(), i );
					}else{
						c_joueurs[i].defOrientation(perso::ORI_gauche);
						deplacer_le_Perso_A( c_joueurs[i].X()-1, c_joueurs[i].Y(), i );
					}
					// Suplace pour afficher les mouvement de rotation lorsque le perso ne bouge pas vraiment
					c_map->setBlock(c_joueurs[i].X(), c_joueurs[i].Y(), c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
					continuerScanClavier = 0;
					envoyerInfoJoueur = 1;
					break;
				}
				/***************************************************************
				* On veux poser une bombe
				*/
				case clavier::lancerBombe: {
					/***********************************************************
					* IL NE RESTE PAS DE BOMBE EN STOCK
					*/
					if( !c_joueurs[i].armements()->quantiteUtilisable(bonus::bombe) )
						break;
					/***********************************************************
					* Il reste des bombes � poser ->
					* 2 Posibilit�s:
					* 1) Il n'y a que le joueur qui veut poser la bombe
					* 2) Il y a plusieurs joueurs
					*/
					switch( c_map->getBlock( c_joueurs[i].X(), c_joueurs[i].Y())->element )// Utilisation d'un switch pour la rapidit� ( en asm ebx modifi� une fois => plus rapide )
					{
						case map::UN_joueur: {
							c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_UN_joueur );
							c_map->ajouterInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );// Ajout de l'info > Mais qui a donc pos� la bombe ...
							c_joueurs[i].armements()->decQuantiteUtilisable( bonus::bombe, c_joueurs[i].X(), c_joueurs[i].Y() );
							break;
						}
						case map::plusieurs_joueurs: {
							c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_plusieurs_joueurs );
							c_map->ajouterInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );// Ajout de l'info > Mais qui a donc pos� la bombe ...
							c_joueurs[i].armements()->decQuantiteUtilisable( bonus::bombe, c_joueurs[i].X(), c_joueurs[i].Y() );
							break;
						}
						default: {// On ne pose pas la bombe ! On a pas les conditions r�uni
							break;
						}
					}
					continuerScanClavier = 0;
					break;
				}
				case clavier::declancheur: {
					if( c_joueurs[i].armements()->quantiteUtilisable(bonus::declancheur) )
						c_joueurs[i].armements()->forceTimeOut();
					continuerScanClavier = 0;
					break;
				}
				default: {
					stdErrorE("Touche envoy� par le joueur %d est inconnue : %d", (int)opt->clavierJoueur(i)->obtenirTouche(key), (int)key);
					break;
				}
			}
		}

		/*######################################################################
		* Traitement des connections ( HOST )
		*/
		if( c_connection == CNX_Host ){
			// Ajout des players
			while( (s = c_server->lookupNewConnection()) != INVALID_SOCKET )
				ajouterNouvelleConnection( s );// Nouveau joueur !

			for( i=0; i<c_nb_joueurs; i++ )
			{
				// Si le joueur est local o� non connect�, on passe
				if( c_joueurs[i].isLocal() || c_joueurs[i].socket() == INVALID_SOCKET )
					continue;

				// Si il y a une activit� cot� r�seau, on la traite
				if( c_server->lookupConnectionClient(c_joueurs[i].socket()) ){
					if( c_server->readClient(c_joueurs[i].socket(), c_buffer, PACK_bufferSize) ){

						// Traitement du clavier des joueurs online
						if( c_joueurs[i].armements() ){

							// On r�cup la touche
							unPackIt( &onlineClavier );

							switch( onlineClavier )
							{
								case clavier::haut: {
									c_joueurs[i].defOrientation(perso::ORI_haut);
									deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()-1, i );
									c_map->setBlock(c_joueurs[i].X(), c_joueurs[i].Y(), c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
									envoyerInfoJoueur = 1;
									break;
								}
								case clavier::bas: {
									c_joueurs[i].defOrientation(perso::ORI_bas);
									deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()+1, i );
									c_map->setBlock(c_joueurs[i].X(), c_joueurs[i].Y(), c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
									envoyerInfoJoueur = 1;
									break;
								}
								case clavier::droite: {
									c_joueurs[i].defOrientation(perso::ORI_droite);
									deplacer_le_Perso_A( c_joueurs[i].X()+1, c_joueurs[i].Y(), i );
									c_map->setBlock(c_joueurs[i].X(), c_joueurs[i].Y(), c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
									envoyerInfoJoueur = 1;
									break;
								}
								case clavier::gauche: {
									c_joueurs[i].defOrientation(perso::ORI_gauche);
									deplacer_le_Perso_A( c_joueurs[i].X()-1, c_joueurs[i].Y(), i );
									c_map->setBlock(c_joueurs[i].X(), c_joueurs[i].Y(), c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
									envoyerInfoJoueur = 1;
									break;
								}
								/***************************************************************
								* On veux poser une bombe
								*/
								case clavier::lancerBombe: {
									/***********************************************************
									* IL NE RESTE PAS DE BOMBE EN STOCK
									*/
									if( !c_joueurs[i].armements()->quantiteUtilisable(bonus::bombe) )
										break;
									/***********************************************************
									* Il reste des bombes � poser ->
									* 2 Posibilit�s:
									* 1) Il n'y a que le joueur qui veut poser la bombe
									* 2) Il y a plusieurs joueurs
									*/
									switch( c_map->getBlock( c_joueurs[i].X(), c_joueurs[i].Y())->element )// Utilisation d'un switch pour la rapidit� ( en asm ebx modifi� une fois => plus rapide )
									{
										case map::UN_joueur:{
											c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_UN_joueur );
											c_map->ajouterInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );// Ajout de l'info > Mais qui a donc pos� la bombe ...
											c_joueurs[i].armements()->decQuantiteUtilisable( bonus::bombe, c_joueurs[i].X(), c_joueurs[i].Y() );
											break;
										}
										case map::plusieurs_joueurs: {
											c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_plusieurs_joueurs );
											c_map->ajouterInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );// Ajout de l'info > Mais qui a donc pos� la bombe ...
											c_joueurs[i].armements()->decQuantiteUtilisable( bonus::bombe, c_joueurs[i].X(), c_joueurs[i].Y() );
											break;
										}
										default: {// On ne pose pas la bombe ! On a pas les conditions r�uni
											break;
										}
									}
									break;
								}
								case clavier::declancheur: {
									if( c_joueurs[i].armements()->quantiteUtilisable(bonus::declancheur) )
										c_joueurs[i].armements()->forceTimeOut();
									break;
								}
								default: {
									stdErrorE("Touche envoy� par le joueur %d est inconnue : %d", (int)opt->clavierJoueur(i)->obtenirTouche(key), (int)key);
									break;
								}
							}
						}
					}else{
						/*******************************************************
						* Le joueur s'est d�co. => Place propre
						*/
						c_joueurs[i].defSocket(INVALID_SOCKET);
						switch( c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element )
						{
							case map::UN_joueur: {
								c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::vide );
								break;
							}
							case map::plusieurs_joueurs: {
								c_map->rmInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );
								c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::UN_joueur );
								break;
							}
							case map::bombe_poser_AVEC_UN_joueur: {
								c_map->rmInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );
								if( c_map->getBlock( c_joueurs[i].X(), c_joueurs[i].Y())->joueur->size() )
									c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser );
								else
									c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::vide );
								break;
							}
							case map::bombe_poser_AVEC_plusieurs_joueurs: {
								if( c_map->getBlock( c_joueurs[i].X(), c_joueurs[i].Y())->joueur->at(0) == i+1 ){// Si la bombe appartien au joueur, on la vire
									if( c_map->getBlock( c_joueurs[i].X(), c_joueurs[i].Y())->joueur->size()-2 == 1 )
										c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::UN_joueur );
									else
										c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::plusieurs_joueurs );
								}else{
									if( c_map->getBlock( c_joueurs[i].X(), c_joueurs[i].Y())->joueur->size()-1 == 1 )
										c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_UN_joueur );
									else
										c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_plusieurs_joueurs );
								}

								c_map->rmInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );
								break;
							}
							default: {
								stdError("Erreur ! Condition non trait� %d", c_map->getBlock(c_joueurs[i].X(), c_joueurs[i].Y())->element);
								break;
							}
						}
						// N�toyage des Events
						c_joueurs[i].armements()->modEvent()->clear();
						for( unsigned int h=0; h<c_listEventBombe.size(); h++ )
						{
							// Si l'�vent correspond � l'event du joueur => clean all
							if( c_listEventBombe.at(h).joueur == i ){
								// Suppression des blocks en-flamm�
								for( unsigned int k=0; k<c_listEventBombe.at(h).deflagration.size(); k++ )
								{
									c_map->rmInfoJoueur(c_listEventBombe.at(h).deflagration.at(k).x, c_listEventBombe.at(h).deflagration.at(k).y, e.joueur, true);
									if( !c_map->getBlock(c_listEventBombe.at(h).deflagration.at(k))->joueur->size() )
										c_map->setBlock(c_listEventBombe.at(h).deflagration.at(k), map::vide);
								}
							}
						}
						// On peut supprimer sans probl�me les armements
						c_joueurs[i].defArmements(0);

						// Envoie des info joueurs � tout les players !
						unsigned char nb_joueur=0;
						for( j=0; j<c_nb_joueurs; j++ ){
							if( c_joueurs[j].isLocal() ){
								nb_joueur++;
							}else{// isLocal() == false
								if( c_joueurs[j].socket() != INVALID_SOCKET )
									nb_joueur++;
							}
						}

						for( j=0; j<c_nb_joueurs; j++ )
						{
							if( !c_joueurs[j].isLocal() && c_joueurs[j].socket() != INVALID_SOCKET ){
								// Envoie du nouveau nombre de joueur
								sprintf(c_buffer, "5:%d", nb_joueur);
								c_server->send_message( c_joueurs[j].socket(), c_buffer, PACK_bufferSize);

								// Envoie des donn�es du joueur
								for( unsigned char k=0; k<c_nb_joueurs; k++ ){
									if( c_joueurs[k].isLocal() || (!c_joueurs[k].isLocal() && c_joueurs[k].socket() != INVALID_SOCKET) ){
										if( j == k ){
											packIt( k, true );
										}else{
											packIt( k, false );
										}
										c_server->send_message( c_joueurs[j].socket(), c_buffer, PACK_bufferSize);// Fin de transmition de map
									}
								}

								sprintf(c_buffer, "8:%d", i);// On dit quel joueur � supprimer
								c_server->send_message( c_joueurs[j].socket(), c_buffer, PACK_bufferSize);


								strcpy(c_buffer, "0:end");
								c_server->send_message( c_joueurs[j].socket(), c_buffer, PACK_bufferSize);// Fin de transmition
							}
						}
					}
				}
			}

			/*******************************************************************
			* Scan des Events interne
			*/
			checkInternalEvent();


			/*******************************************************************
			* On envoie les modif faite sur la map au players online.
			*/
			if( c_map->getModifications()->size() ){
				for( j=0; j<c_nb_joueurs; j++ ){
					// On r�cup que les players online
					if( c_joueurs[j].isLocal() || c_joueurs[j].socket() == INVALID_SOCKET )
						continue;

					for( i=0; i<c_map->getModifications()->size(); i++ )
					{
						X = c_map->getModifications()->at(i).x;
						Y = c_map->getModifications()->at(i).y;
						c_server->send_message(c_joueurs[j].socket(), packIt(X, Y, c_map->getBlock(X, Y)->element), PACK_bufferSize);
						if( c_map->getBlock(X, Y)->joueur && c_map->getBlock(X, Y)->joueur->size() )
							c_server->send_message(c_joueurs[j].socket(), packIt( c_map->getBlock(X, Y)->joueur ), PACK_bufferSize);
					}
					if( !envoyerInfoJoueur ){
						strcpy(c_buffer, "0:end");
						c_server->send_message(c_joueurs[j].socket(), c_buffer, PACK_bufferSize);// Fin de transmition
					}
				}
			}


			/*******************************************************************
			* On envoie les infos joueur si besoin est.
			*/
			if( envoyerInfoJoueur ){
				// Envoie des info joueurs � tout les players !
				for( i=0; i<c_nb_joueurs; i++ )
				{
					if( !c_joueurs[i].isLocal() && c_joueurs[i].socket() != INVALID_SOCKET ){
						// Envoie des donn�es du joueur
						for( j=0; j<c_nb_joueurs; j++ ){
							if( c_joueurs[j].isLocal() || (!c_joueurs[j].isLocal() && c_joueurs[j].socket() != INVALID_SOCKET) ){
								if( i == j ){
									packIt( j, true );
								}else{
									packIt( j, false );
								}
								c_server->send_message( c_joueurs[i].socket(), c_buffer, PACK_bufferSize);// Fin de transmition de map
							}
						}
						strcpy(c_buffer, "0:end");
						c_server->send_message( c_joueurs[i].socket(), c_buffer, PACK_bufferSize);// Fin de transmition
					}
				}
				envoyerInfoJoueur = 0;
			}
		}else{
		//######################################################################
		//######################################################################

			/*******************************************************************
			* Scan des Events interne
			*/
			checkInternalEvent();
		}

		continuerScanClavier = 1;

	}while( key != RETOUR_MENU_PRECEDENT && (nbJoueurVivant() > 1 || c_listEventBombe.size()) );

	// On d�termine, qui a gagn�
	for( i=0; i<c_nb_joueurs; i++ )
	{
		if( (c_joueurs[i].isLocal()
			|| (
				!c_joueurs[i].isLocal()
				&& c_joueurs[i].socket() != INVALID_SOCKET
				)
			)
			&& c_joueurs[i].armements()
		){
			c_winnerID = i+1;// ( on veut un nombre entre 1 et ... )
			break;
		}
	}

	// Si on est host, alors, on envoie le nom du gagnant au clients
	if( c_connection == CNX_Host ){
		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		sprintf(c_buffer, "4:%c�", c_winnerID+'0');
		for( i=0; i<c_nb_joueurs; i++ )
		{
			// Si le joueur est local o� non connect�, on passe
			if( c_joueurs[i].isLocal() || c_joueurs[i].socket() == INVALID_SOCKET )
				continue;

			c_server->send_message(c_joueurs[i].socket(), c_buffer, PACK_bufferSize);// Envoie du signal: Fin de partie + Nom du joueur gagnant. Note => Fin de transmition.
			c_server->rmClient(c_joueurs[i].socket());
			c_joueurs[i].defSocket(INVALID_SOCKET);
		}
	}

	// On d�sinitialise les bonus utilisables pour cette partie
	bonus::unInitBonusProp();

	if( c_connection == CNX_Host )// Si on est en mode serveur alors on connecte le serveur
		c_server->endListen();

	if( nbJoueurVivant() == 1 )
		return c_winnerID;

	return 0;
}


/***************************************************************************//*!
* @fn void partie::deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur )
* @brief Test si un personnage peut se d�placer � un endroit.
*
* Si on peut d�placer le perso dans la zone demand� => on d�place le perso
* SINON rien.
* C'est aussi par cette fonction que l'on :
*		- ramasse les bonus
*		- meur ( flamme )
*		- ne passe pas a travers les objets
*		- ne d�passe pas la taille de la map
*
* @bug <b>NE GERE PAS TOUT LES CAS</b>
* @todo faire tout les cas possibles
*/
void partie::deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur )
{
	map::t_type elementNouvellePosition;

	if( !c_joueurs[joueur].armements() ){
		stdError("Le joueur %d n'a pas d'armements ! Impossible de d�placer le joueur !", joueur);
		return ;
	}

	if( joueur >= c_nb_joueurs )
		stdErrorE("Le joueur %d n'existe pas ! Impossible de d�placer le joueur !", joueur);

	/***********************************************************************
	* On check les dim avant tout !
	* NOTE: newX, newY unsigned ! => pas besoin de check => 0
	*/
	if( !(	/*0 <= newX &&*/ newX < c_map->X()// On v�rif si on ne d�passe pas la
		&&	/*0 <= newY &&*/ newY < c_map->Y()// taille de la map
	))
		return ;

	// Ce qui a sur la nouvelle case
	elementNouvellePosition = c_map->getBlock(newX, newY)->element;

	/***********************************************************************
	* Toutes les condition n�c�ssaire pour pouvoir bouger
	* Si l'une d'elle pas bonne => return void;
	* NOTE: newX, newY unsigned ! => pas besoin de check => 0
	*/
	if( !(	elementNouvellePosition != map::Mur_destructible		// On v�rif s'il n'y a pas
		&&	elementNouvellePosition != map::Mur_INdestructible		// d'objet solid
		&& (
				(
					(
						elementNouvellePosition == map::bombe_poser
						|| elementNouvellePosition == map::bombe_poser_AVEC_UN_joueur
						|| elementNouvellePosition == map::bombe_poser_AVEC_plusieurs_joueurs
					)
					&&
					c_joueurs[joueur].armements()->quantiteMAX(bonus::pousse_bombe)
					&&
					c_map->getBlock(newX, newY)->joueur
					&&
					c_map->getBlock(newX, newY)->joueur->size()
					&&
					c_joueurs[c_map->getBlock(newX, newY)->joueur->at(0)-1].armements()
					&&
					c_joueurs[c_map->getBlock(newX, newY)->joueur->at(0)-1].armements()->getEvent(newX, newY)
					&&
					(
						(
							c_joueurs[joueur].orientation() == perso::ORI_haut
							&&
							newY >= 1
							&&
							c_map->getBlock(newX, newY-1)->element == map::vide
						)
						||
						(
							c_joueurs[joueur].orientation() == perso::ORI_bas
							&&
							newY+1 < c_map->Y()
							&&
							c_map->getBlock(newX, newY+1)->element == map::vide
						)
						||
						(
							c_joueurs[joueur].orientation() == perso::ORI_droite
							&&
							newX+1 < c_map->X()
							&&
							c_map->getBlock(newX+1, newY)->element == map::vide
						)
						||
						(
							c_joueurs[joueur].orientation() == perso::ORI_gauche
							&&
							newX >= 1
							&&
							c_map->getBlock(newX-1, newY)->element == map::vide
						)
					)
				)
				||
				(
						elementNouvellePosition != map::bombe_poser
					&&	elementNouvellePosition != map::bombe_poser_AVEC_UN_joueur
					&&	elementNouvellePosition != map::bombe_poser_AVEC_plusieurs_joueurs
				)
			)
		))
	{
		return ;
	}


	/***********************************************************************
	* On ne tombe pas sur un objet inconnu
	* Anti bug
	*/
	if( elementNouvellePosition == map::inconnu )
		stdErrorE("Objet inconnu dans la map � X=%d, Y=%d !", newX, newY);

	// Tout est Ok, on d�place le perso

	/***********************************************************************
	* On enl�ve le perso de son ancienne position.
	* ( Et remise en �tat de la case )
	*/
	// On regarde le nombre d'info joueur(meta donn�es) sur la case actuel
	if( c_map->nb_InfoJoueur(c_joueurs[joueur].X(), c_joueurs[joueur].Y()) > 1 ){
		// Il reste plus de 1 joueurs sur la case
		switch( c_map->getBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y())->element )
		{
			case map::bombe_poser_AVEC_UN_joueur: {
				// On enl�ve le perso de son ancienne position
				c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::bombe_poser);
				c_map->rmInfoJoueur( c_joueurs[joueur].X(), c_joueurs[joueur].Y(), joueur+1, 0 );
				break;
			}
			case map::plusieurs_joueurs: {
				// On enl�ve le perso de son ancienne position et S'il n'y avait plus que 2 joueurs => il ne restera q'un joueur
				if( c_map->nb_InfoJoueur(c_joueurs[joueur].X(), c_joueurs[joueur].Y()) == 2 )
					c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::UN_joueur);

				c_map->rmInfoJoueur(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), joueur+1, 1);
				break;
			}
			case map::bombe_poser_AVEC_plusieurs_joueurs: {
				// On enl�ve le perso de son ancienne position
				if( c_map->nb_InfoJoueur(c_joueurs[joueur].X(), c_joueurs[joueur].Y())-2 == 1 ){
					c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::bombe_poser_AVEC_UN_joueur);
				}else{
					c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::bombe_poser_AVEC_plusieurs_joueurs);
				}
				c_map->rmInfoJoueur( c_joueurs[joueur].X(), c_joueurs[joueur].Y(), joueur+1, 0 );
				break;
			}
			default: {
				stdError("cas NON traite %d", (int)c_map->getBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y())->element);
				break;
			}
		}
	}else{// Suppression total -> case toute propre
		// On enl�ve le perso de son ancienne position
		c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::vide);
	}

	/***********************************************************************
	* On place le perso a sa nouvelle position
	*/
	switch( elementNouvellePosition )
	{
		case map::vide: {
			c_map->setBlock(newX, newY, map::UN_joueur);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		case map::UN_joueur:
		case map::plusieurs_joueurs: {
			c_map->setBlock(newX, newY, map::plusieurs_joueurs);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		case map::flamme_horizontal:
		case map::flamme_vertical:
		case map::flamme_pointe_haut:
		case map::flamme_pointe_bas:
		case map::flamme_pointe_droite:
		case map::flamme_pointe_gauche:
		case map::flamme_origine: {
			// Viens d'aller dans le feu -> Boulet ?
			c_joueurs[joueur].defPos(newX, newY);

			if( c_joueurs[joueur].armements()->quantiteUtilisable(bonus::vie) == 0 ){
				// Le perso n'a plus de vie
				// On le supprime de la carte
				// On force tout les events � ce mettre dans la file g�n�ral
				// On active toutes les bombes
				c_joueurs[joueur].armements()->kill();
			}else{
				// Il reste une vie au perso
				// On supprime une vie
				c_joueurs[joueur].armements()->decQuantiteUtilisable(bonus::vie);
				c_joueurs[joueur].armements()->decQuantiteMAX_en_stock(bonus::vie);
				placer_perso_position_initial(joueur);
			}
			break;
		}
		case map::bonus: {
			if( !c_map->getBlock(newX, newY)->joueur )
				stdErrorE("BONUS: Probl�me de pointeur ! c_map->getBlock(%u, %u)->joueur = 0 !", newX, newY);
			if( !c_map->getBlock(newX, newY)->joueur->size() )
				stdErrorE("BONUS: Probl�me de contenu ! c_map->getBlock(%u, %u)->joueur->size() = 0 !", newX, newY);

			bonus::t_Bonus b = (bonus::t_Bonus)c_map->getBlock(newX, newY)->joueur->at(0);

			if( b >= bonus::NB_ELEMENT_t_Bonus )
				stdErrorE("BONUS: Probl�me de contenu ! c_map->getBlock(%u, %u)->joueur->at(0){%u} >= {%d}NB_ELEMENT_t_Bonus !", newX, newY, (unsigned int)c_map->getBlock(newX, newY)->joueur->at(0), bonus::NB_ELEMENT_t_Bonus);
			// Ajout de l'armement
			if( !c_joueurs[joueur].armements() )
				stdErrorE("Le joueur %d n'a pas d'armement ! Erreur lors du d�placement sur le bonus !", joueur);


			/*********************************************************************
			* Bonus sp�cial : Le t�l�porteur
			*/
			if( b == bonus::teleporteur )
			{
				unsigned int rj, rx, ry;
				do{
					rj = myRand(0, c_nb_joueurs-1);
				}while( !c_joueurs[rj].armements() );

				do{
					rx = myRand(0, c_map->X()-1);
					ry = myRand(0, c_map->Y()-1);
				}while( rx >= c_map->X() || ry >= c_map->Y() || c_map->getBlock(rx, ry)->element != map::vide );

				if( rj != joueur ){
					deplacer_le_Perso_A(rx, ry, rj);
				}else{
					c_map->setBlock(newX, newY, map::vide);// On surrpime tout ce qui avait avant
					// Placement du perso
					c_map->setBlock(rx, ry, map::UN_joueur);
					c_map->ajouterInfoJoueur(rx, ry, joueur+1);
					c_joueurs[joueur].defPos(rx, ry);
					break;
				}

			}else if( b == bonus::inversseur_touche ){
				/***************************************************************
				* Bonus inversseur de touches
				*/
				unsigned char rj;
				do{
					rj = myRand(0, c_nb_joueurs-1);
				}while( !c_joueurs[rj].armements() || rj == joueur );

				if( c_joueurs[rj].armements()->quantiteMAX(bonus::inversseur_touche) )
					c_joueurs[rj].armements()->incQuantiteUtilisable_Event(bonus::inversseur_touche);
				else
					c_joueurs[rj].armements()->incQuantiteMAX_en_stock_Event(bonus::inversseur_touche);

			}else if( b == bonus::force_explosion ){
				/***************************************************************
				* Bonus inversseur de touches
				*/
				for( unsigned char j=0; j<c_nb_joueurs; j++ )
				{
					if( c_joueurs[j].armements() )
						c_joueurs[j].armements()->forceTimeOut();
				}

			}else{
				/***************************************************************
				* Bonus classique
				*/
				c_joueurs[joueur].armements()->incQuantiteMAX_en_stock(b);
			}
			c_map->setBlock(newX, newY, map::vide);// On surrpime tout ce qui avait avant
			// Placement du perso
			c_map->setBlock(newX, newY, map::UN_joueur);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		case map::bombe_poser: {
			if( !c_map->getBlock(newX, newY)->joueur )
				stdErrorE("BONUS: Probl�me de pointeur ! c_map->getBlock(%u, %u)->joueur = 0 !", newX, newY);
			if( !c_map->getBlock(newX, newY)->joueur->size() )
				stdErrorE("BONUS: Probl�me de contenu ! c_map->getBlock(%u, %u)->joueur->size() = 0 !", newX, newY);


			s_EventPousseBombe e;
			e.direction = c_joueurs[joueur].orientation();
			e.joueur = c_map->getBlock(newX, newY)->joueur->at(0)-1;
			e.repetionSuivante = clock() + bonus::VITESSE_pousseBombe;

			if( !c_joueurs[e.joueur].armements() )
				stdErrorE("Attention ! Le joueur %u est mort !", e.joueur);

			// La bombe n'est plus dans la liste des event bonus
			if( !c_joueurs[e.joueur].armements()->getEvent(newX, newY) ){
				stdError("getEvent(%u, %u) = 0", newX, newY);
				return ;
			}

			switch( c_joueurs[joueur].orientation() )
			{
				case perso::ORI_haut: {
					c_map->setBlock( newX, newY-1, map::bombe_poser );
					c_map->ajouterInfoJoueur( newX, newY-1, joueur+1);

					c_map->rmInfoJoueur(newX, newY);
					c_map->setBlock(newX, newY, map::UN_joueur);
					c_map->ajouterInfoJoueur( newX, newY, joueur+1);
					c_joueurs[joueur].defPos(newX, newY);

					e.pos = coordonneeConvert(newX, newY-1);
					break;
				}
				case perso::ORI_bas: {
					c_map->setBlock( newX, newY+1, map::bombe_poser );
					c_map->ajouterInfoJoueur( newX, newY+1, joueur+1);

					c_map->rmInfoJoueur(newX, newY);
					c_map->setBlock(newX, newY, map::UN_joueur);
					c_map->ajouterInfoJoueur( newX, newY, joueur+1);
					c_joueurs[joueur].defPos(newX, newY);

					e.pos = coordonneeConvert(newX, newY+1);
					break;
				}
				case perso::ORI_droite: {
					c_map->setBlock( newX+1, newY, map::bombe_poser );
					c_map->ajouterInfoJoueur( newX+1, newY, joueur+1);

					c_map->rmInfoJoueur(newX, newY);
					c_map->setBlock(newX, newY, map::UN_joueur);
					c_map->ajouterInfoJoueur( newX, newY, joueur+1);
					c_joueurs[joueur].defPos(newX, newY);

					e.pos = coordonneeConvert(newX+1, newY);
					break;
				}
				case perso::ORI_gauche: {
					c_map->setBlock( newX-1, newY, map::bombe_poser );
					c_map->ajouterInfoJoueur( newX-1, newY, joueur+1);

					c_map->rmInfoJoueur(newX, newY);
					c_map->setBlock(newX, newY, map::UN_joueur);
					c_map->ajouterInfoJoueur( newX, newY, joueur+1);
					c_joueurs[joueur].defPos(newX, newY);

					e.pos = coordonneeConvert(newX-1, newY);
					break;
				}
				default:
					stdErrorE("Erreur, orientation incorrect ! %d", c_joueurs[joueur].orientation());
			}

			c_joueurs[e.joueur].armements()->getEvent(newX, newY)->pos = e.pos;
			c_listEventPouseBombe<< e ;// On ajoute la bome � la liste d'event

			break;
		}
		default: {
			stdError("Cas non pris en charge c_map->getBlock(%u, %u).element=%d", newX, newY, elementNouvellePosition);
		}
	}
}

/***************************************************************************//*!
* @fn void partie::placer_perso_position_initial( unsigned char joueur )
* @brief Place un perso a sa position d'initial
*
* Si la case initial, contient :
*		- un bonus, => ramasse le bonus
*		- des flammes -> n�toyage de la case puis on place le perso
*
* @bug <b>NE GERE PAS TOUT LES CAS</b>
* @todo faire tout les cas possibles
*/
void partie::placer_perso_position_initial( unsigned char joueur )
{
	map::t_type elementNouvellePosition;
	unsigned int newX, newY;

	if( joueur >= c_nb_joueurs )
		stdErrorE("Le joueur %d n'existe pas ! Impossible de d�placer le joueur !", joueur);

	newX = c_map->positionInitialJoueur(joueur+1).x;
	newY = c_map->positionInitialJoueur(joueur+1).y;

	// Ce qui a sur la nouvelle case
	elementNouvellePosition = c_map->getBlock(newX, newY)->element;

	/***********************************************************************
	* Toutes les condition n�c�ssaire pour pouvoir bouger
	* Si l'une d'elle pas bonne => return void;
	* NOTE: newX, newY unsigned ! => pas besoin de check => 0
	*/
	if( !(	/*0 <= newX &&*/ newX < c_map->X()// On v�rif si on ne d�passe pas la
		&&	/*0 <= newY &&*/ newY < c_map->Y()// taille de la map
		&&	elementNouvellePosition != map::Mur_destructible		// On v�rif s'il n'y a pas
		&&	elementNouvellePosition != map::Mur_INdestructible))	// d'objet solid
	{
		stdError("Probl�me de positionnement ! Impossible de mettre le perso a sa position initial !");
		return ;
	}

	/***********************************************************************
	* On ne tombe pas sur un objet inconnu
	* Anti bug
	*/
	if( elementNouvellePosition == map::inconnu )
		stdErrorE("Objet inconnu dans la map � X=%d, Y=%d !", newX, newY);

	// Tout est Ok, on d�place le perso

	/***********************************************************************
	* On place le perso a sa nouvelle position
	*/
	switch( elementNouvellePosition )
	{
		case map::vide: {
			c_map->setBlock(newX, newY, map::UN_joueur);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		case map::UN_joueur:
		case map::plusieurs_joueurs: {
			c_map->setBlock(newX, newY, map::plusieurs_joueurs);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		case map::flamme_horizontal:
		case map::flamme_vertical:
		case map::flamme_pointe_haut:
		case map::flamme_pointe_bas:
		case map::flamme_pointe_droite:
		case map::flamme_pointe_gauche:
		case map::flamme_origine: {
			// Viens d'aller dans le feu -> On vire le feu
			c_map->rmInfoJoueur(newX, newY);// On surrpime tout ce qui avait avant
			c_map->setBlock(newX, newY, map::UN_joueur);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		case map::bonus: {
			if( !c_map->getBlock(newX, newY)->joueur )
				stdErrorE("BONUS: Probl�me de pointeur ! c_map->getBlock(%u, %u)->joueur = 0 !", newX, newY);
			if( !c_map->getBlock(newX, newY)->joueur->size() )
				stdErrorE("BONUS: Probl�me de contenu ! c_map->getBlock(%u, %u)->joueur->size() = 0 !", newX, newY);
			if( c_map->getBlock(newX, newY)->joueur->at(0) >= bonus::NB_ELEMENT_t_Bonus )
				stdErrorE("BONUS: Probl�me de contenu ! c_map->getBlock(%u, %u)->joueur->at(0){%u} >= {%d}NB_ELEMENT_t_Bonus !", newX, newY, (unsigned int)c_map->getBlock(newX, newY)->joueur->at(0), bonus::NB_ELEMENT_t_Bonus);
			// Ajout de l'armement
			if( !c_joueurs[joueur].armements() )
				stdErrorE("Le joueur %d n'a pas d'armement ! Erreur lors du d�placement sur le bonus !", joueur);
			c_joueurs[joueur].armements()->incQuantiteMAX_en_stock((bonus::t_Bonus)c_map->getBlock(newX, newY)->joueur->at(0));
			c_map->rmInfoJoueur(newX, newY);// On surrpime tout ce qui avait avant
			// Placement du perso
			c_map->setBlock(newX, newY, map::UN_joueur);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		default: {
			stdErrorE("Cas non pris en charge c_map->getBlock(%u, %u).element=%d", newX, newY, elementNouvellePosition);
		}
	}
}


/***************************************************************************//*!
* @fn void partie::checkInternalEventPousseBombe()
* @brief Scan les event du bonus pousse bombe
*
* @bug Date du bug: 2011/01/20<br />
* Le code contenu dans cette fonction devrait normalement aller directement dans la fonction
* partie::checkInternalEvent(). Or sous SDL, le code contenu dans cette fonction provoque un bug
* d'acc�l�ration des personnages. Ce bug ne touche que les joueurs qui ont des touches comprises
* entre a et z. M�me si le code n'est jamais trait�, le bug ce produit quand m�me. De plus si
* l'on essaye de tracer le bug avec stdError(...) au niveau du clavier ou m�me au niveau de
* perso::defPos( unsigned int Xpos, unsigned int Ypos ) alors le bug n'existe plus...<br />
* Pour info, ce bug existe UNIQUEMENT depuis la cr�ation du bonus "Pousse Bombe"<br />
* De plus le bug en question n'existe que sous windows. Le m�me code compil� sur GNU/UNIX marche parfaitement.<br />
* A premi�re vu, ce bout de code a lui seul fait apparaitre le bug, alors que le code
* en lui m�me ne fait que lire des infos : (Cette ligne n'a pas besoin d'�tre trait�e, pour que le bug apparaisse)
* @code
* // Le i provient du for( unsigned int i=0; i<c_listEventPouseBombe.size(); i++ )
* c_joueurs[c_listEventPouseBombe.at(i).joueur].armements()->getEvent(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y);
* @endcode
*/
void partie::checkInternalEventPousseBombe()
{
	for( unsigned int i=0; i<c_listEventPouseBombe.size(); i++ )
	{
		// Tant que l'on a pas atteint le temps
		if( c_listEventPouseBombe.at(i).repetionSuivante > clock() ) continue;

		c_joueurs[c_listEventPouseBombe.at(i).joueur].armements();
		c_listEventPouseBombe.at(i).pos.x;
		c_listEventPouseBombe.at(i).pos.y;
		// Ce bout de code a lui seul fait apparaitre le bug...
		c_joueurs[c_listEventPouseBombe.at(i).joueur].armements()->getEvent(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y);

		// Si le joueur est mort => on vire la bombe de la liste
		// Si la bombe a explos� => on vire la bombe de la liste
		if( !c_joueurs[c_listEventPouseBombe.at(i).joueur].armements() || !c_joueurs[c_listEventPouseBombe.at(i).joueur].armements()->getEvent(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y) ){
			c_listEventPouseBombe.remove(i);
			i--;
		}else{

			switch( c_listEventPouseBombe.at(i).direction )
			{
				case perso::ORI_haut: {
					if( c_listEventPouseBombe.at(i).pos.y >= 1 && c_map->getBlock(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y-1)->element == map::vide ){
						c_map->setBlock( c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y-1, map::bombe_poser );
						c_map->ajouterInfoJoueur( c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y-1, c_listEventPouseBombe.at(i).joueur+1);
						c_map->setBlock(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y, map::vide);
						c_listEventPouseBombe.at(i).pos.y -= 1;
						c_joueurs[c_listEventPouseBombe.at(i).joueur].armements()->getEvent(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y+1)->pos = c_listEventPouseBombe.at(i).pos;
						// On regarde si on pourra aller plus loin. Sinon => On supprime cette bombe de l'event
						if( !(c_listEventPouseBombe.at(i).pos.y >= 1 && c_map->getBlock(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y-1)->element == map::vide) ){
							c_listEventPouseBombe.remove(i);
							i--;
						}else{
							c_listEventPouseBombe.at(i).repetionSuivante = clock() + bonus::VITESSE_pousseBombe;
						}
					}else{
						c_listEventPouseBombe.remove(i);
						i--;
					}
					break;
				}
				case perso::ORI_bas: {
					if( c_listEventPouseBombe.at(i).pos.y+1 < c_map->Y() && c_map->getBlock(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y+1)->element == map::vide ){
						c_map->setBlock( c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y+1, map::bombe_poser );
						c_map->ajouterInfoJoueur( c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y+1, c_listEventPouseBombe.at(i).joueur+1);
						c_map->setBlock(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y, map::vide);
						c_listEventPouseBombe.at(i).pos.y += 1;
						c_joueurs[c_listEventPouseBombe.at(i).joueur].armements()->getEvent(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y-1)->pos = c_listEventPouseBombe.at(i).pos;
						// On regarde si on pourra aller plus loin. Sinon => On supprime cette bombe de l'event
						if( !(c_listEventPouseBombe.at(i).pos.y+1 < c_map->Y() && c_map->getBlock(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y+1)->element == map::vide) ){
							c_listEventPouseBombe.remove(i);
							i--;
						}else{
							c_listEventPouseBombe.at(i).repetionSuivante = clock() + bonus::VITESSE_pousseBombe;
						}
					}else{
						c_listEventPouseBombe.remove(i);
						i--;
					}
					break;
				}
				case perso::ORI_droite: {
					if( c_listEventPouseBombe.at(i).pos.x+1 < c_map->X() && c_map->getBlock(c_listEventPouseBombe.at(i).pos.x+1, c_listEventPouseBombe.at(i).pos.y)->element == map::vide ){
						c_map->setBlock( c_listEventPouseBombe.at(i).pos.x+1, c_listEventPouseBombe.at(i).pos.y, map::bombe_poser );
						c_map->ajouterInfoJoueur( c_listEventPouseBombe.at(i).pos.x+1, c_listEventPouseBombe.at(i).pos.y, c_listEventPouseBombe.at(i).joueur+1);
						c_map->setBlock(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y, map::vide);
						c_listEventPouseBombe.at(i).pos.x += 1;
						c_joueurs[c_listEventPouseBombe.at(i).joueur].armements()->getEvent(c_listEventPouseBombe.at(i).pos.x-1, c_listEventPouseBombe.at(i).pos.y)->pos = c_listEventPouseBombe.at(i).pos;
						// On regarde si on pourra aller plus loin. Sinon => On supprime cette bombe de l'event
						if( !(c_listEventPouseBombe.at(i).pos.x+1 < c_map->X() && c_map->getBlock(c_listEventPouseBombe.at(i).pos.x+1, c_listEventPouseBombe.at(i).pos.y)->element == map::vide) ){
							c_listEventPouseBombe.remove(i);
							i--;
						}else{
							c_listEventPouseBombe.at(i).repetionSuivante = clock() + bonus::VITESSE_pousseBombe;
						}
					}else{
						c_listEventPouseBombe.remove(i);
						i--;
					}
					break;
				}
				case perso::ORI_gauche: {
					if( c_listEventPouseBombe.at(i).pos.x >= 1 && c_map->getBlock(c_listEventPouseBombe.at(i).pos.x-1, c_listEventPouseBombe.at(i).pos.y)->element == map::vide ){
						c_map->setBlock( c_listEventPouseBombe.at(i).pos.x-1, c_listEventPouseBombe.at(i).pos.y, map::bombe_poser );
						c_map->ajouterInfoJoueur( c_listEventPouseBombe.at(i).pos.x-1, c_listEventPouseBombe.at(i).pos.y, c_listEventPouseBombe.at(i).joueur+1);
						c_map->setBlock(c_listEventPouseBombe.at(i).pos.x, c_listEventPouseBombe.at(i).pos.y, map::vide);
						c_listEventPouseBombe.at(i).pos.x -= 1;
						c_joueurs[c_listEventPouseBombe.at(i).joueur].armements()->getEvent(c_listEventPouseBombe.at(i).pos.x+1, c_listEventPouseBombe.at(i).pos.y)->pos = c_listEventPouseBombe.at(i).pos;
						// On regarde si on pourra aller plus loin. Sinon => On supprime cette bombe de l'event
						if( !(c_listEventPouseBombe.at(i).pos.x >= 1 && c_map->getBlock(c_listEventPouseBombe.at(i).pos.x-1, c_listEventPouseBombe.at(i).pos.y)->element == map::vide) ){
							c_listEventPouseBombe.remove(i);
							i--;
						}else{
							c_listEventPouseBombe.at(i).repetionSuivante = clock() + bonus::VITESSE_pousseBombe;
						}
					}else{
						c_listEventPouseBombe.remove(i);
						i--;
					}
					break;
				}
				default:{
					stdErrorE("Erreur, orientation incorrect ! %d", c_listEventPouseBombe.at(i).direction);
				}
			}
		}
	}
}


/***************************************************************************//*!
* @fn void partie::checkInternalEvent()
* @brief Analyse et traite les Event internes pour une partie F4A
*/
void partie::checkInternalEvent()
{
	static unsigned int i;

	/***************************************************************************
	* Horloge
	*/
	// Vague 1: On enl�ve tout les blocks Mur_destructible
	// Vague >1: On met 2 Mur_INdestructible
	if( clock() >= c_timeOut && c_timeOut != 0 ){
		if( c_timerAttak == 0 ){
			for( unsigned x=0, y=0; y<c_map->Y(); y++ )
			{
				for( x=0; x<c_map->X(); x++ )
				{
					if( c_map->getBlock(x, y)->element == map::Mur_destructible ){
						c_map->setBlock(x, y, map::vide);
						c_timerAttak++;// On compte le nombre d'emplacement vide
					}else{
						if( c_map->getBlock(x, y)->element == map::vide )
							c_timerAttak++;// On compte le nombre d'emplacement vide
					}
				}
			}
			c_timeOut = clock()+ 5*CLOCKS_PER_SEC;// 5secs avant l'attaque final
		}else{
			unsigned x=0, y=0;
			if( c_timerAttak > 5 ){
				bool positionInitial = 0;
				do{
					y = myRand(0, c_map->Y()-1);
					x = myRand(0, c_map->X()-1);
					for( i=0; i<c_map->nb_PointDeDepartJoueur(); i++ )
					{
						if( c_map->positionInitialJoueur(i+1).x == x && c_map->positionInitialJoueur(i+1).y == y ){
							positionInitial = 1;
							break;
						}else{
							positionInitial = 0;
						}
					}

				}while( c_map->getBlock(x, y)->element != map::vide || positionInitial == true );
				c_map->setBlock(x, y, map::Mur_INdestructible);

				c_timeOut = clock()+ CLOCKS_PER_SEC;
				c_timerAttak--;// On a une case vide en moins
			}else{
				c_timeOut = 0;
			}
		}
	}

	/***************************************************************************
	* Events Pousse Bombe
	*/
	checkInternalEventPousseBombe();

	/***************************************************************************
	* Events Bombe
	*/
	for( i=0; i<c_listEventBombe.size(); i++ )
	{
		// Tant que l'on a pas atteint le temps
		if( c_listEventBombe.at(i).repetionSuivante > clock() ) continue;

		s_EventBombe& e = c_listEventBombe.at(i);

		/***************************************************************
		* Encore un tour ?
		* On efface les trace des bombes
		*/
		if( e.Nb_Repetition > e.Nb_Repetition_MAX ){// On a fini ! Plus de tour
			// Suppression des blocks en-flamm�
			for( unsigned int j=0; j<e.deflagration.size(); j++ )
			{
				c_map->rmInfoJoueur(e.deflagration.at(j).x, e.deflagration.at(j).y, e.joueur, true);
				if( !c_map->getBlock(e.deflagration.at(j))->joueur->size() )
					c_map->setBlock(e.deflagration.at(j), map::vide);
			}

			// Pop des bonus
			for( unsigned int j=0; j<e.listBlockDetruit.size(); j++ )
			{
				if( !c_map->getBlock(e.listBlockDetruit.at(j))->joueur->size() ){// One ne fait rien pop si il y a d�j� quelqu'un � cette emplacement ou si il y a des flammes enemis
					bonus::t_Bonus tmp = bonus::getBonusAleatoire();
					if( tmp == bonus::__RIEN__ ){
						c_map->setBlock(e.listBlockDetruit.at(j), map::vide);
					}else{
						c_map->rmInfoJoueur(e.listBlockDetruit.at(j).x, e.listBlockDetruit.at(j).y);// Pour supprimer tout les meta-info
						c_map->setBlock(e.listBlockDetruit.at(j), map::bonus);
						c_map->ajouterInfoJoueur(e.listBlockDetruit.at(j).x,e.listBlockDetruit.at(j).y, (unsigned char)tmp, 1);
					}
				}
			}
			// Fin de l'event => out de la liste
			c_listEventBombe.erase(c_listEventBombe.begin()+i);
			i--;

		}else{// ENCORE un tour

			// Cr�ation de la d�flagration
			if( e.Nb_Repetition == 0 ){// On ne traite que le centre le 1er tour
				killPlayers(&e, e.pos.x, e.pos.y);// Adition des flammes avec le reste de l'environement
				c_map->setBlock(e.pos.x, e.pos.y, map::flamme_origine);
				e.deflagration.push_back( coordonneeConvert(e.pos.x, e.pos.y) );

				c_listEventBombe.at(i).Nb_Repetition++;
				c_listEventBombe.at(i).repetionSuivante = clock() + bonus::VITESSE_flammes;// Ajustement du temps

				continue;
			}

			// Flamme gauche
			if( e.continue_negativeX ){
				char r = actionSurLesElements( &e, e.pos.x-e.Nb_Repetition, e.pos.y, -1 );
				if( r == -1 || r == 0 )
					e.continue_negativeX = false;
				if( r == 0 || r == 1 )
					e.deflagration.push_back( coordonneeConvert(e.pos.x-e.Nb_Repetition, e.pos.y) );
			}

			// Flamme droite
			if( e.continue_X ){
				char r = actionSurLesElements( &e, e.pos.x+e.Nb_Repetition, e.pos.y, +1 );
				if( r == -1 || r == 0 )
					e.continue_X = false;
				if( r == 0 || r == 1 )
					e.deflagration.push_back( coordonneeConvert(e.pos.x+e.Nb_Repetition, e.pos.y) );
			}

			// Flamme haut
			if( e.continue_negativeY ){
				char r = actionSurLesElements( &e, e.pos.x, e.pos.y-e.Nb_Repetition, -2 );
				if( r == -1 || r == 0 )
					e.continue_negativeY = false;
				if( r == 0 || r == 1 )
					e.deflagration.push_back( coordonneeConvert(e.pos.x, e.pos.y-e.Nb_Repetition) );
			}

			// Flamme bas
			if( e.continue_Y ){
				char r = actionSurLesElements( &e, e.pos.x, e.pos.y+e.Nb_Repetition, +2 );
				if( r == -1 || r == 0 )
					e.continue_Y = false;
				if( r == 0 || r == 1 )
					e.deflagration.push_back( coordonneeConvert(e.pos.x, e.pos.y+e.Nb_Repetition) );
			}

			if( !e.continue_negativeX && !e.continue_X && !e.continue_negativeY && !e.continue_Y )
				e.Nb_Repetition = e.Nb_Repetition_MAX;

			c_listEventBombe.at(i).Nb_Repetition++;
			c_listEventBombe.at(i).repetionSuivante = clock() + bonus::VITESSE_flammes;// * CLOCKS_PER_SEC;// Ajustement du temps
		}

	}
}


/***************************************************************************//*!
* @fn char partie::actionSurLesElements( s_EventBombe* e, unsigned int x, unsigned int y, char direction )
* @brief Positionne les flammes en fonction de l'environement
* @param[in,out] e L'event en cours
* @param[in] x Position X de la map a check
* @param[in] y Position Y de la map a check
* @param[in] direction Direction des flammes
* @return 3 Valeurs de retoures possible<br />
*			- -1 On stop tout et on ne touche pas au block � cette adresse ! (Out of map ou Mur_INdestructible)
*			-  0 On stop tout mais on traite le block � l'adresse (Mur_destructible)
*			-  1 On continue
*
* @note On retourne la valeur de partie::killPlayers( s_EventBombe* e, unsigned int x, unsigned int y )
*/
char partie::actionSurLesElements( s_EventBombe* e, unsigned int x, unsigned int y, char direction )
{
	char continuer = 1;
	switch( killPlayers(e, x, y) )
	{
		case -1: {
			return -1;
		}
		case 0: {
			continuer = 0;
		}// Pas de break pour lire le block du dessous
		case 1: {
			switch( direction )
			{
				case -2:{// -Y
					if( c_map->getBlock(x, y+1)->element != map::flamme_origine )
						c_map->setBlock(x, y+1, map::flamme_vertical);

					c_map->setBlock(x, y, map::flamme_pointe_haut);
					break;
				}
				case 2:{// +Y
					if( c_map->getBlock(x, y-1)->element != map::flamme_origine )
						c_map->setBlock(x, y-1, map::flamme_vertical);

					c_map->setBlock(x, y, map::flamme_pointe_bas);
					break;
				}
				case -1:{// -X
					if( c_map->getBlock(x+1, y)->element != map::flamme_origine )
						c_map->setBlock(x+1, y, map::flamme_horizontal);

					c_map->setBlock(x, y, map::flamme_pointe_gauche);
					break;
				}
				case 1:{// +X
					if( c_map->getBlock(x-1, y)->element != map::flamme_origine )
						c_map->setBlock(x-1, y, map::flamme_horizontal);

					c_map->setBlock(x, y, map::flamme_pointe_droite);
					break;
				}
			}
			c_map->rmInfoJoueur(x, y);// On supprime toutes les info qui �tait pr�sente
			c_map->ajouterInfoJoueur(x, y, e->joueur, 1);// On dit a qui appartient la flamme
			break;
		}
	}
	return continuer;
}


/***************************************************************************//*!
* @fn char partie::killPlayers( s_EventBombe* e, unsigned int x, unsigned int y )
* @brief Tue les joueurs et d�truit les block destructibles (Mode: F4A)
* @param[in,out] e L'event en cours
* @param[in] x Position X de la map a check
* @param[in] y Position Y de la map a check
* @return 3 Valeurs de retoures possible<br />
*			- -1 On stop tout et on ne touche pas au block � cette adresse ! (Out of map ou Mur_INdestructible)
*			-  0 On stop tout mais on traite le block � l'adresse (Mur_destructible)
*			-  1 On continue
*
* @note Les valeurs de X et de Y sont compar�es � la taille de la map => Pas de risque de sortir de la map
*/
char partie::killPlayers( s_EventBombe* e, unsigned int x, unsigned int y )
{
	// k sert dans les boucle for comme compteur.
	// Peut �tre utilis�e sans probl�me
	static unsigned int k;// �a ne sert � rien de la recr�er � chaque fois
	static s_EventBombe eAdd;
	unsigned char idJoueur;

	if( x >= c_map->X() || y >= c_map->Y() )
		return -1;

	switch( c_map->getBlock(x, y)->element )
	{
		case map::UN_joueur: {// Le pauvre joueur qui est au millieu du chemin de la flamme -> dead
			idJoueur = c_map->getBlock(x, y)->joueur->at(0)-1;
			if( c_joueurs[idJoueur].armements()->quantiteUtilisable(bonus::vie) == 0 ){
				// Le perso n'a plus de vie
				// On le supprime de la carte
				// On force tout les events � ce mettre dans la file g�n�ral
				// On active toutes les bombes
				c_joueurs[idJoueur].armements()->kill();
			}else{
				// Il reste une vie au perso
				// On supprime une vie
				c_joueurs[idJoueur].armements()->decQuantiteUtilisable(bonus::vie);
				c_joueurs[idJoueur].armements()->decQuantiteMAX_en_stock(bonus::vie);
				placer_perso_position_initial(idJoueur);
			}
			c_map->rmInfoJoueur(x, y);

			return 1;
		}
		case map::plusieurs_joueurs: {// Les pauvres joueurs qui sont au millieu du chemin de la flamme -> dead
			for( k=0; k<c_map->getBlock(x, y)->joueur->size(); k++ )
			{
				idJoueur = c_map->getBlock(x, y)->joueur->at(k)-1;
				if( c_joueurs[idJoueur].armements()->quantiteUtilisable(bonus::vie) == 0 ){
					// Le perso n'a plus de vie
					// On le supprime de la carte
					// On force tout les events � ce mettre dans la file g�n�ral
					// On active toutes les bombes
					c_joueurs[idJoueur].armements()->kill();
				}else{
					// Il reste une vie au perso
					// On supprime une vie
					c_joueurs[idJoueur].armements()->decQuantiteUtilisable(bonus::vie);
					c_joueurs[idJoueur].armements()->decQuantiteMAX_en_stock(bonus::vie);
					placer_perso_position_initial(idJoueur);
				}
			}
			c_map->rmInfoJoueur(x, y);
			return 1;
		}
		case map::bombe_poser_AVEC_UN_joueur: {// Le pauvre joueur qui est au millieu du chemin de la flamme -> dead
			idJoueur = c_map->getBlock(x, y)->joueur->at(0)-1;

			// On fait explos� la bombe
			if( c_joueurs[idJoueur].armements() )
				c_joueurs[idJoueur].armements()->forceTimeOut(x, y);

			// On tue le player
			idJoueur = c_map->getBlock(x, y)->joueur->at(1)-1;
			if( c_joueurs[idJoueur].armements()->quantiteUtilisable(bonus::vie) == 0 ){
				// Le perso n'a plus de vie
				// On le supprime de la carte
				// On force tout les events � ce mettre dans la file g�n�ral
				// On active toutes les bombes
				c_joueurs[idJoueur].armements()->kill();

			}else{
				// Il reste une vie au perso
				// On supprime une vie
				c_joueurs[idJoueur].armements()->decQuantiteUtilisable(bonus::vie);
				c_joueurs[idJoueur].armements()->decQuantiteMAX_en_stock(bonus::vie);
				placer_perso_position_initial(idJoueur);
			}
			c_map->rmInfoJoueur(x, y, idJoueur+1, 0);

			// On supprime la bombe (anti-bug)
			/*
			if( !c_joueurs[idJoueur].armements() )
				c_map->rmInfoJoueur(x, y, idJoueur+1, 1);
			*/


			return 0;
		}
		case map::bombe_poser_AVEC_plusieurs_joueurs: {// Les pauvres joueurs qui sont au millieu du chemin de la flamme -> dead
				idJoueur = c_map->getBlock(x, y)->joueur->at(0)-1;

				// On fait explos� la bombe
				if( c_joueurs[idJoueur].armements() )
					c_joueurs[idJoueur].armements()->forceTimeOut(x, y);

				// On tue les players
				for( unsigned int h=1; h<c_map->getBlock(x, y)->joueur->size(); h++ ){
					idJoueur = c_map->getBlock(x, y)->joueur->at(h)-1;
					if( c_joueurs[idJoueur].armements()->quantiteUtilisable(bonus::vie) == 0 ){
						// Le perso n'a plus de vie
						// On le supprime de la carte
						// On force tout les events � ce mettre dans la file g�n�ral
						// On active toutes les bombes
						c_joueurs[idJoueur].armements()->kill();
					}else{
						// Il reste une vie au perso
						// On supprime une vie
						c_joueurs[idJoueur].armements()->decQuantiteUtilisable(bonus::vie);
						c_joueurs[idJoueur].armements()->decQuantiteMAX_en_stock(bonus::vie);
						placer_perso_position_initial(idJoueur);
					}
					c_map->rmInfoJoueur(x, y, idJoueur+1, 0);
					h--;
				}

				// On supprime la bombe (anti-bug)
				/*
				if( !c_joueurs[idJoueur].armements() )
					c_map->rmInfoJoueur(x, y, idJoueur+1, 1);
				*/
			return 0;
		}
		case map::bombe_poser: {
			if( !c_map->getBlock(x, y)->joueur )
				stdErrorE("Probl�me de pointeur vide ! c_map->getBlock(%u, %u)->joueur = 0", x, y);
			if( !c_map->getBlock(x, y)->joueur->size() )
				stdErrorE("Vector vide � x=%u, y=%u", x, y);

			// On fait explos� la bombe
			if( c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].armements() )
				c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].armements()->forceTimeOut(x, y);

			break;
			return 0;//<- Mettre le return pour emp�cher l'effet super d�flagration
		}
		case map::Mur_destructible: {
			//c_map->ajouterInfoJoueur(x, y, 0, 1);// On dit au block -> Now tu as �t� d�truit et tu est dans une liste
			// Ajout du block � la liste des blocks qui ont �t� d�truit
			e->listBlockDetruit.push_back( coordonneeConvert(x, y) );
			return 0;
		}
		case map::Mur_INdestructible: {
			return -1;
		}
		case map::flamme_pointe_haut:
		case map::flamme_pointe_bas:
		case map::flamme_pointe_droite:
		case map::flamme_pointe_gauche:{
			break;
		}
		case map::bonus: {
			c_map->rmInfoJoueur(x, y);
			break;
		}
		default: {// On eject les autres cas
			break;
		}
	}

	return 1;
}


/***************************************************************************//*!
* @fn const char* partie::packIt( uint32_t X, uint32_t Y, map::t_type type )
* @brief Empaquette les variables X, Y, type pour les envoyer sur le r�seau
* @return Les variables format�es dans une chaine de caract�re
*/
const char* partie::packIt( uint32_t X, uint32_t Y, map::t_type type )
{
	unsigned int PACK_size = snprintf(c_buffer, PACK_bufferSize, "1:%u,%u,%u�", X, Y, type);
	if( PACK_size >= PACK_bufferSize )
		stdError("ATTENTION ! Perte de donnees ! %d donnees lues et %d donnees ecrites Maximum ( Ne pas oublier dans les calcules le \\0 ! )", PACK_size, PACK_bufferSize);

	return c_buffer;
}


/***************************************************************************//*!
* @fn const char* partie::packIt( std::vector<unsigned char>* list )
* @brief Empaquette tout ce que contient le tableau : map info joueur
* @return Les variables format�es dans une chaine de caract�re
*/
const char* partie::packIt( std::vector<unsigned char>* list )
{
	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);

	unsigned int p=2;

	// Ent�te
	c_buffer[0] = '2';
	c_buffer[1] = ':';

	for( unsigned int i=0; i<list->size(); i++ ){
		if( p >= PACK_bufferSize )
			stdErrorE("D�passement de capacit� !");

		c_buffer[p] = list->at(i)+'0';
		p++;
		c_buffer[p] = ' ';
		p++;
	}
	c_buffer[p-1] = '�';
	c_buffer[p] = 0;

	return c_buffer;
}


/***************************************************************************//*!
* @fn const char* partie::packIt( clavier::t_touche t )
* @brief Empaquette la touche
* @return La variable format� dans une chaine de caract�re + un indicateur de contenue
*/
const char* partie::packIt( clavier::t_touche t )
{
	if( 0 > t || t > 9 )
		stdError("Erreur lors du packing du clavier ! Le nombre %d ne peut etre pack !", t);

	strcpy(c_buffer, "3:0�");// Ent�te
	c_buffer[2] += (int)t;

	return c_buffer;
}


/***************************************************************************//*!
* @fn const char* partie::packIt( unsigned char joueur, bool includeWeapon )
* @brief Empaquette la touche
* @return La variable format� dans une chaine de caract�re + un indicateur de contenue
*/
const char* partie::packIt( unsigned char joueur, bool includeWeapon )
{
	// Taille max 43 caract�res (au pire des cas) Or PACK_bufferSize == 47
	if( includeWeapon ){
		if( c_joueurs[joueur].armements() ){
			unsigned int PACK_size = snprintf(c_buffer, PACK_bufferSize, "7:%d,%d,%d,%d,%d,%d,%d,%s�"
					,joueur
					,c_joueurs[joueur].orientation()
					,c_joueurs[joueur].armements()->quantiteMAX(bonus::bombe)
					,c_joueurs[joueur].armements()->quantiteMAX(bonus::puissance_flamme)
					,c_joueurs[joueur].armements()->quantiteMAX(bonus::declancheur)
					,c_joueurs[joueur].armements()->quantiteMAX(bonus::pousse_bombe)
					,c_joueurs[joueur].armements()->quantiteMAX(bonus::vie)
					,c_joueurs[joueur].nom()->c_str()
			);
			if( PACK_size >= PACK_bufferSize )
				stdError("ATTENTION ! Perte de donnees ! %d donnees lues et %d donnees ecrites Maximum ( Ne pas oublier dans les calcules le \\0 ! )", PACK_size, PACK_bufferSize);
		}else{
			sprintf(c_buffer, "7:%d,%d,0,0,0,0,0,%s�", joueur, c_joueurs[joueur].orientation(), c_joueurs[joueur].nom()->c_str());
		}
	}else{
		sprintf(c_buffer, "6:%d,%d,%s�", joueur, c_joueurs[joueur].orientation(), c_joueurs[joueur].nom()->c_str());
	}

	return c_buffer;
}


/***************************************************************************//*!
* @fn void partie::unPackIt( uint32_t* X, uint32_t* Y, map::t_type* type )
* @brief D�sempaquette le buffer et met les donn�es dans les variables X, Y, type
*/
void partie::unPackIt( uint32_t* X, uint32_t* Y, map::t_type* type )
{
	unsigned int i=2;
	bool firstLap = 1;

	//stdError("UnPack this: %s", c_buffer+2);

	if( c_buffer[0] != '1' || c_buffer[strlen(c_buffer)-1] != '�' ){
		stdError("Erreur ! Mauvais packet ! {%s}", c_buffer);
		*X = 0;
		*Y = 0;
		*type = map::vide;
		return ;
	}
	c_buffer[strlen(c_buffer)-1] = 0;// On vire la fin de chaine �

	*X = 0;
	firstLap = 1;
	for( ; c_buffer[i] != ',' ; i++ )
	{
		if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
			if( firstLap ){
				firstLap = 0;
			}else{
				*X *= 10;
			}
			*X += c_buffer[i]-'0';
		}
	}
	i++;// Pour virer la ,

	*Y = 0;
	firstLap = 1;
	for( ; c_buffer[i] != ','; i++ )
	{
		if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
			if( firstLap ){
				firstLap = 0;
			}else{
				*Y *= 10;
			}
			*Y += c_buffer[i]-'0';
		}
	}
	i++;// Pour virer la ,

	*type = (map::t_type)0;
	firstLap = 1;
	for( ; c_buffer[i]; i++ )
	{
		if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
			if( firstLap ){
				firstLap = 0;
			}else{
				*type = (map::t_type)(*type*10);
			}
			*type = (map::t_type)(*type+(c_buffer[i]-'0'));
		}
	}

	//sscanf(c_buffer, "%u,%u,%u", X, Y, type );
	//stdError("UnPacked: %u,%u,%u", *X, *Y, *type);
}


/***************************************************************************//*!
* @fn void partie::unPackIt( uint32_t X, uint32_t Y )
* @brief D�sempaquette le buffer et met les donn�es dans le tableau
*/
void partie::unPackIt( uint32_t X, uint32_t Y )
{
	//stdError("UnPack this: %s at x=%u, y=%u", c_buffer+2, X, Y);

	if( c_buffer[0] != '2' || c_buffer[strlen(c_buffer)-1] != '�' ){
		stdError("Erreur ! Mauvais packet ! {%s}", c_buffer);
		return ;
	}
	c_buffer[strlen(c_buffer)-1] = 0;// On vire la fin de chaine �

	for( unsigned int i=2; c_buffer[i]; i++ ){
		if( i >= PACK_bufferSize )
			stdErrorE("D�passement de capacit� !");

		if( '0' <= c_buffer[i] && c_buffer[i] <= '9' )
			c_map->ajouterInfoJoueur(X, Y, c_buffer[i]-'0');
	}
}


/***************************************************************************//*!
* @fn void partie::unPackIt( clavier::t_touche* t )
* @brief D�sempaquette le buffer et met les donn�es dans la variable
*/
void partie::unPackIt( clavier::t_touche* t )
{
	if( c_buffer[0] != '3' || c_buffer[strlen(c_buffer)-1] != '�' ){
		stdError("Erreur ! Mauvais packet ! {%s}", c_buffer);
		*t = clavier::NUL;
		return ;
	}

	*t = (clavier::t_touche)(c_buffer[2]-'0');
}


/***************************************************************************//*!
* @fn void partie::unPackIt()
* @brief D�sempaquette le buffer et met les donn�es dans le joueur correspondant
*/
void partie::unPackIt()
{
	unsigned char idJoueur = 0;
	unsigned int i = 2;
	bool firstLap = 1;
	perso::t_Orientation ori = (perso::t_Orientation)0;
	unsigned char tmpBonusValue;

	if( (c_buffer[0] != '6' && c_buffer[0] != '7') || c_buffer[strlen(c_buffer)-1] != '�' ){
		stdError("Packet incorrect ! {%s}", c_buffer);
		return ;
	}
	c_buffer[strlen(c_buffer)-1] = 0;// On vire la fin de chaine �

	/***************************************************************************
	* On d�termine l'ID du joueur
	*/
	idJoueur = 0;
	firstLap = 1;
	for( ; c_buffer[i] != ','; i++ )
	{
		if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
			if( firstLap ){
				firstLap = 0;
			}else{
				idJoueur *= 10;
			}
			idJoueur += c_buffer[i]-'0';
		}
	}
	i++;// Pour virer la,

	if( idJoueur >= c_nb_joueurs )
		stdErrorE("Erreur ! idJoueur incorrect ! idJoueur=%u, c_nb_joueurs=%u", idJoueur, c_nb_joueurs);

	c_joueurs[idJoueur].defNeed_Refresh(1);

	/***************************************************************************
	* On d�termine l'orientation du joueur
	*/
	firstLap = 1;
	for( ; c_buffer[i] != ','; i++ )
	{
		if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
			if( firstLap ){
				firstLap = 0;
			}else{
				ori = (perso::t_Orientation)(ori*((perso::t_Orientation)10));
			}
			ori = (perso::t_Orientation)(ori+(perso::t_Orientation)(c_buffer[i]-'0'));
		}
	}
	i++;// Pour virer la,
	c_joueurs[idJoueur].defOrientation(ori);


	/***************************************************************************
	* On d�termine l'armements du joueur
	*/
	if( c_buffer[0] == '7' ){
		c_uniqueJoueurID = idJoueur;

		if( !c_joueurs[idJoueur].armements() ){
			c_joueurs[idJoueur].defArmements(new bonus);
			c_joueurs[idJoueur].armements()->param_Par_Defaut();
		}

		/***************************************************************************
		* On d�termine l'armements du joueur
		*/
		// Calcule le nombre de bombe
		tmpBonusValue = 0;
		for( ; c_buffer[i] != ','; i++ )
		{
			if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
				if( firstLap ){
					firstLap = 0;
				}else{
					tmpBonusValue *= 10;
				}
				tmpBonusValue = tmpBonusValue+(c_buffer[i]-'0');
			}
		}
		i++;// Pour virer la,
		c_joueurs[idJoueur].armements()->defQuantiteMAX(bonus::bombe, tmpBonusValue);

		// Calcule puissance flamme
		tmpBonusValue = 0;
		for( ; c_buffer[i] != ','; i++ )
		{
			if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
				if( firstLap ){
					firstLap = 0;
				}else{
					tmpBonusValue *= 10;
				}
				tmpBonusValue = tmpBonusValue+(c_buffer[i]-'0');
			}
		}
		i++;// Pour virer la,
		c_joueurs[idJoueur].armements()->defQuantiteMAX(bonus::puissance_flamme, tmpBonusValue);

		// Calcule declancheur
		tmpBonusValue = 0;
		for( ; c_buffer[i] != ','; i++ )
		{
			if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
				if( firstLap ){
					firstLap = 0;
				}else{
					tmpBonusValue *= 10;
				}
				tmpBonusValue = tmpBonusValue+(c_buffer[i]-'0');
			}
		}
		i++;// Pour virer la,
		c_joueurs[idJoueur].armements()->defQuantiteMAX(bonus::declancheur, tmpBonusValue);

		// Calcule pousse bombe
		tmpBonusValue = 0;
		for( ; c_buffer[i] != ','; i++ )
		{
			if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
				if( firstLap ){
					firstLap = 0;
				}else{
					tmpBonusValue *= 10;
				}
				tmpBonusValue = tmpBonusValue+(c_buffer[i]-'0');
			}
		}
		i++;// Pour virer la,
		c_joueurs[idJoueur].armements()->defQuantiteMAX(bonus::pousse_bombe, tmpBonusValue);

		// Calcule vie
		tmpBonusValue = 0;
		for( ; c_buffer[i] != ','; i++ )
		{
			if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
				if( firstLap ){
					firstLap = 0;
				}else{
					tmpBonusValue *= 10;
				}
				tmpBonusValue = tmpBonusValue+(c_buffer[i]-'0');
			}
		}
		i++;// Pour virer la,
		c_joueurs[idJoueur].armements()->defQuantiteMAX(bonus::vie, tmpBonusValue);
	}

	// On def le nom du joueur
	c_joueurs[idJoueur].defNom(string(c_buffer+i));
}


/***************************************************************************//*!
* @fn void partie::ajouterNouvelleConnection( SOCKET s )
* @brief Permet d'ajouter un nouveau joueur � la partie
*
* @include Protocole_Socket.txt
*/
void partie::ajouterNouvelleConnection( SOCKET s )
{
	int				l_isIntel=1,
					l_is32Bits=1,
					bufferSize = 0,
					version = 0;
	unsigned char	i,
					j,
					idJoueur = 0;
	unsigned char nb_joueur = 0;

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);
	c_server->readClient( s, c_buffer, PACK_bufferSize );// On attend les donn�es du client
	sscanf(c_buffer, "%d %d %d %d", &version, &l_is32Bits, &l_isIntel, &bufferSize );// Utilisation d'un buffer pour eviter tout probl�me de lecture

	if( version != __BOMBERMAN_VERSION__ || Is32Bits != l_is32Bits || l_isIntel != (int)baseClientServer::isLittleEndian() || bufferSize != PACK_bufferSize ){
		stdError("Vous avez refus� une connection ! La confiuration du client n'est pas compactible avec le serveur ! Client:{Version=%d, Is32Bits=%d, IsIntel=%d, bufferSize=%d}, Le serveur:{Version=%d, Is32Bits=%d, IsIntel=%d, bufferSize=%d}", version, l_is32Bits, l_isIntel, bufferSize, __BOMBERMAN_VERSION__, Is32Bits, (int)baseClientServer::isLittleEndian(), PACK_bufferSize);
		c_server->rmClient( s );// Connection refus�
		return ;
	}

	c_server->send_message( s, "1", 2);// Connection acc�pt�

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);
	c_server->readClient( s, c_buffer, PACK_bufferSize );// On attend le nom du client

	// On cherche un SLOT pour notre joueur
	// Et on v�rifit si le nom n'est pas dans la liste
	bool slotFinded = false;
	for( i=0; i<c_nb_joueurs; i++ ){
		if( c_joueurs[i].isLocal() ){
			if( *c_joueurs[i].nom() == c_buffer ){// Nom d�j� utilis�
				c_server->rmClient( s );// Nom refus� => D�co
				return ;
			}
			nb_joueur++;
		}else{// isLocal() == false
			if( c_joueurs[i].socket() == INVALID_SOCKET && !slotFinded ){// Note, on est oblig� de trouver un slot libre, sinon le server n'aurait pas acc�pt� la connection
				idJoueur = i;
				slotFinded = 1;
				nb_joueur++;
			}
			if( c_joueurs[i].socket() != INVALID_SOCKET )
				nb_joueur++;
		}
	}

	// Cr�ation du joueur
	c_joueurs[idJoueur].defSocket( s );
	c_joueurs[idJoueur].defNom( c_buffer );
	c_joueurs[idJoueur].defArmements( new bonus() );
	c_joueurs[idJoueur].armements()->param_Par_Defaut();
	placer_perso_position_initial( idJoueur );


	c_server->send_message( s, "1", 2);// Nom acc�pt�


	// Envoie des dim de la map
	c_server->send_message(s, packIt(c_map->X(), c_map->Y(), map::inconnu), PACK_bufferSize);

	// Envoie de la map
	for( unsigned int x=0, y=0; y<c_map->Y(); y++ )
	{
		for( x=0; x<c_map->X(); x++ )
		{
			c_server->send_message(s, packIt(x, y, c_map->getBlock(x, y)->element), PACK_bufferSize);
			if( c_map->getBlock(x, y)->joueur && c_map->getBlock(x, y)->joueur->size() )
				c_server->send_message(s, packIt( c_map->getBlock(x, y)->joueur ), PACK_bufferSize);
		}
	}


	// Envoie des info joueurs � tout les players !
	for( i=0; i<c_nb_joueurs; i++ )
	{
		if( !c_joueurs[i].isLocal() && c_joueurs[i].socket() != INVALID_SOCKET ){
			// Envoie du nouveau nombre de joueur
			sprintf(c_buffer, "5:%d�", nb_joueur);
			c_server->send_message( c_joueurs[i].socket(), c_buffer, PACK_bufferSize);

			// Envoie des donn�es du joueur
			for( j=0; j<c_nb_joueurs; j++ ){
				if( c_joueurs[j].isLocal() || (!c_joueurs[j].isLocal() && c_joueurs[j].socket() != INVALID_SOCKET) ){
					if( i == j ){
						packIt( j, true );
					}else{
						packIt( j, false );
					}
					c_server->send_message( c_joueurs[i].socket(), c_buffer, PACK_bufferSize);// Fin de transmition de map
				}
			}

			// Envoie du timer
			sprintf(c_buffer, "9:%ld�", c_timeOut-clock());
			c_server->send_message( c_joueurs[i].socket(), c_buffer, PACK_bufferSize);// Fin de transmition de map
		}
	}

	strcpy(c_buffer, "0:end");
	c_server->send_message( s, c_buffer, PACK_bufferSize);// Fin de transmition

	c_joueurs[idJoueur].defNeed_Refresh(1);
}
