#include "partie.h"
#include "debug.h"

using namespace std;

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
	//c_joueursOrientation = 0;// <- Pas besoin ( union )
	c_map = 0;

	c_server = 0;
	//c_client = 0;// <- Pas besoin ( union )

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);
}


/***************************************************************************//*!
* @fn partie::~partie()
* @brief D�sinitialise la class partie et supprime tous les joueurs et la map
*/
partie::~partie()
{
	if( c_connection == CNX_Client ){// Si on est en mode client !
		if( c_nb_joueurs && c_joueursOrientation )
			delete[] c_joueursOrientation;
	}else{
		if( c_nb_joueurs && c_joueurs )
			delete[] c_joueurs;
	}
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
		p = c_map->mettreJoueurA_sa_PositionInitial( i+1 );
		c_joueurs[i].defPos(p.x, p.y);
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
		stdError("c_nb_joueurs(%u), c_joueurs(%X)", (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);
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
		if( c_joueurs[i].armements() && (c_joueurs[i].armements()->est_Dans_La_Liste(bonus::vie) == -1 || (c_joueurs[i].armements()->quantiteUtilisable(bonus::vie) > 0 || c_joueurs[i].armements()->quantiteMAX_Ramassable(bonus::vie) == 0)) )
			nbJoueurVivant++;
	}
	return nbJoueurVivant;
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
*			0 : Retour au menu
*			-1: Tout Quitter
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
	s_Event e;
	SOCKET s;
	uint32_t X=0, Y=0;// Utilis� uniquement lors de partie online ( host & client )
	clavier::t_touche onlineClavier;// Utilis� uniquement lors de partie online ( host & client )
	c_winnerName.clear();
	c_timeOut = clock() + 2*60*CLOCKS_PER_SEC+30*CLOCKS_PER_SEC;// 2min:30

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
		sprintf(c_buffer, "%d %d", Is32Bits, (int)baseClientServer::isLittleEndian());
		c_client->send_message(c_buffer, PACK_bufferSize);

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Attente de la r�ponce
		if( !c_client->readServer(c_buffer, PACK_bufferSize) ){
			stdError("Configuration 32Bits, IsIntel incorrect pour le serveur ! Connection refus� !");
			return -10;// On a �t� d�connect� !
		}

		// Envoie du nom
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
		uint32_t nb_MetaDonnee=0;
		map::t_type type;

		unPackIt( &X, &Y, &type, &nb_MetaDonnee );// type et nb_MetaDonnee inutile ici

		if( !X || !Y )
			stdErrorE("Taille de map incorrect !!! TailleX=%u, TailleY=%u, <%s>", X, Y, c_buffer);

		if( c_map )
			delete c_map;
		if( !(c_map = new map(X, Y)) )// Cr�ation de la map
			stdErrorE("Erreur lors de l'allocation m�moire !");

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer

		// R�c�ption de TOUTE la map
		while( c_client->readServer(c_buffer, PACK_bufferSize) != 0 && string("end") != c_buffer )
		{
			if( !nb_MetaDonnee ){
				unPackIt( &X, &Y, &type, &nb_MetaDonnee );
				c_map->setBlock(X, Y, type);
			}else{
				nb_MetaDonnee = 0;
				unPackIt(X, Y);
			}
			memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
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
				if( c_client->readServer(c_buffer, PACK_bufferSize) ){
					if( strcmp("endGame", c_buffer) != 0 ){
						nb_MetaDonnee = 0;
						do{
							if( !nb_MetaDonnee ){
								unPackIt( &X, &Y, &type, &nb_MetaDonnee );
								c_map->setBlock(X, Y, map::vide);
								c_map->setBlock(X, Y, type);
							}else{
								nb_MetaDonnee = 0;
								unPackIt(X, Y);
							}
							memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
						}while( c_client->readServer(c_buffer, PACK_bufferSize) != 0 && strcmp("end", c_buffer) != 0 && strcmp("endGame", c_buffer) != 0 );
					}
				}else{
					stdError("Vous avez �t� d�connect�.");
					break;// On a �t� d�connect� !
				}
			}

		}while( key != RETOUR_MENU_PRECEDENT && c_client->is_connected() && string("endGame") != c_buffer );

		// On d�termine, qui a gagn�
		char JoueurGagnant = 0;

		// Si c'est la fin de la partie ( avec un gagnant )
		stdError("ici");
		if( string("endGame") == c_buffer && c_client->is_connected() ){
			stdError("passe :)");
			memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
			c_client->readServer(c_buffer, PACK_bufferSize);
			c_client->readServer(c_buffer, PACK_bufferSize);
			c_client->readServer(c_buffer, PACK_bufferSize);
			c_client->readServer(c_buffer, PACK_bufferSize);
			c_client->readServer(c_buffer, PACK_bufferSize);
			c_client->readServer(c_buffer, PACK_bufferSize);

			stdError("donn�es: %s", c_buffer);
			JoueurGagnant = c_buffer[0]-'0';
			c_winnerName = c_buffer+2;
		}
		stdError("ici-fin");

		c_client->disconnection();

		return JoueurGagnant;
	}

	/*##########################################################################
	* Partie en mode serveur ou offline
	*/
	// On initialise les bonus utilisables pour cette partie
	bonus::bonusProp();
	// On initialise les bonus pour les joueurs
	for( unsigned char i=0; i<c_nb_joueurs; i++ )
	{
		c_joueurs[i].defArmements( new bonus() );
		c_joueurs[i].armements()->param_Par_Defaut();
	}

	// On g�n�re la map
	genMap();

	if( c_connection == CNX_Host ){// Si on est en mode serveur alors on connecte le serveur
		// On d�termine le nombre maximum de player online
		unsigned int nbLocal=0;
		for( i=0; i<c_nb_joueurs; i++ )
			nbLocal += c_joueurs[i].isLocal();

		//stdError("Nombre de player online max: %u",c_nb_joueurs-nbLocal);

		// Ajustement des param�tres
		c_server->setNbClientMax(c_nb_joueurs-nbLocal);
		c_server->Listening();

		// Teste sur les types
		if( sizeof(uint8_t) != sizeof(unsigned char) )
			stdErrorE("sizeof(uint8_t){%u} != sizeof(unsigned char){%u}", sizeof(uint8_t), sizeof(unsigned char));

		// S'il n'y a qu'un perso en local, alors on wait un perso exterieur
		if( nbLocal == 1 ){
			c_server->setWait(0,100000);// 0.1 secondes
			// Ajout des player
			while( (s = c_server->lookupNewConnection()) == INVALID_SOCKET && key != RETOUR_MENU_PRECEDENT )
				key = afficherMapEtEvent( this );// Pour r�cup les touches

			// On a enfin un player, on lui envoie la map
			ajouterNouvelleConnection( s );
		}
		c_server->setWait(0,1000);// 0.001 secondes
	}
	//##########################################################################
	//##########################################################################


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
			while( c_joueurs[i].armements()->isEvent(&bonusEvent) )// Tanqu'il y a des event on tourne
			{
				//c_map->setBlock(pos.x, pos.y, map::vide);
				e.type = bonusEvent.type;
				e.joueur = i+1;
				e.pos = bonusEvent.pos;
				e.repetionSuivante = 0;
				e.Nb_Repetition = 0;
				e.Nb_Repetition_MAX = c_joueurs[i].armements()->quantiteUtilisable(bonus::puissance_flamme);
				e.continue_X = true;
				e.continue_negativeX = true;
				e.continue_Y = true;
				e.continue_negativeY = true;
				c_listEvent.push_back( e );
			}

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
					c_joueurs[i].defOrientation(perso::ORI_haut);
					deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()-1, i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::bas: {
					c_joueurs[i].defOrientation(perso::ORI_bas);
					deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()+1, i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::droite: {
					c_joueurs[i].defOrientation(perso::ORI_droite);
					deplacer_le_Perso_A( c_joueurs[i].X()+1, c_joueurs[i].Y(), i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::gauche: {
					c_joueurs[i].defOrientation(perso::ORI_gauche);
					deplacer_le_Perso_A( c_joueurs[i].X()-1, c_joueurs[i].Y(), i );
					continuerScanClavier = 0;
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
									break;
								}
								case clavier::bas: {
									c_joueurs[i].defOrientation(perso::ORI_bas);
									deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()+1, i );
									break;
								}
								case clavier::droite: {
									c_joueurs[i].defOrientation(perso::ORI_droite);
									deplacer_le_Perso_A( c_joueurs[i].X()+1, c_joueurs[i].Y(), i );
									break;
								}
								case clavier::gauche: {
									c_joueurs[i].defOrientation(perso::ORI_gauche);
									deplacer_le_Perso_A( c_joueurs[i].X()-1, c_joueurs[i].Y(), i );
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
						stdError("Joueur d�co");
					}
				}
			}

			/*******************************************************************
			* Scan des Events interne
			*/
			checkInternalEvent();

			// On envoie les modif faite sur la map au players online.
			if( c_map->getModifications()->size() ){
				for( j=0; j<c_nb_joueurs; j++ ){
					// On r�cup que les players online
					if( c_joueurs[j].isLocal() || c_joueurs[j].socket() == INVALID_SOCKET )
						continue;

					for( i=0; i<c_map->getModifications()->size(); i++ )
					{
						X = c_map->getModifications()->at(i).x;
						Y = c_map->getModifications()->at(i).y;
						if( c_map->getBlock(X, Y)->joueur && c_map->getBlock(X, Y)->joueur->size() ){
							c_server->send_message(c_joueurs[j].socket(), packIt(X, Y, c_map->getBlock(X, Y)->element, c_map->getBlock(X, Y)->joueur->size()), PACK_bufferSize);
							c_server->send_message(c_joueurs[j].socket(), packIt( c_map->getBlock(X, Y)->joueur ), PACK_bufferSize);
						}else{
							c_server->send_message(c_joueurs[j].socket(), packIt(X, Y, c_map->getBlock(X, Y)->element, 0), PACK_bufferSize);
						}
					}
					c_server->send_message(c_joueurs[j].socket(), "end", 4);// Fin de transmition
				}
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

	}while( key != RETOUR_MENU_PRECEDENT && (nbJoueurVivant() > 1 || c_listEvent.size()) );

	// On d�termine, qui a gagn�
	char JoueurGagnant = 0;
	for( i=0; i<c_nb_joueurs; i++ )
	{
		if( c_joueurs[i].armements() && (c_joueurs[i].armements()->est_Dans_La_Liste(bonus::vie) == -1 || (c_joueurs[i].armements()->quantiteUtilisable(bonus::vie) > 0 || c_joueurs[i].armements()->quantiteMAX_Ramassable(bonus::vie) == 0)) ){
			JoueurGagnant = i+1;// ( on veut un nombre entre 1 et ... )
			c_winnerName = *c_joueurs[i].nom();
			break;
		}
	}

	// Si on est host, alors, on envoie le nom du gagnant au clients
	if( c_connection == CNX_Host ){
		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		sprintf(c_buffer, "%d,%s", (int)JoueurGagnant, c_winnerName.c_str());
		for( i=0; i<c_nb_joueurs; i++ )
		{
			// Si le joueur est local o� non connect�, on passe
			if( c_joueurs[i].isLocal() || c_joueurs[i].socket() == INVALID_SOCKET )
				continue;

			c_server->send_message(c_joueurs[i].socket(), "endGame", 8);// Envoie du signal: Fin de partie.
			c_server->send_message(c_joueurs[i].socket(), c_buffer, PACK_bufferSize);// Nom du joueur gagnant. Note => Fin de transmition.
		}
	}

	// On d�sinitialise les bonus utilisables pour cette partie
	bonus::unInitBonusProp();

	if( c_connection == CNX_Host )// Si on est en mode serveur alors on connecte le serveur
		c_server->endListen();

	if( nbJoueurVivant() == 1 )
		return JoueurGagnant;

	return 0;
}


/***************************************************************************//*!
* @fn void partie::deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur )
* @brief Test si un personnage peut se d�placer � un endroit.
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

	if( !c_map )
		stdErrorE("Initialiser la map avant !");

	if( !c_joueurs )
		stdErrorE("Initialiser les joueurs avant !");

	if( joueur >= c_nb_joueurs )
		stdErrorE("Le joueur %d n'existe pas ! Impossible de d�placer le joueur !", joueur);

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
		&&	elementNouvellePosition != map::Mur_INdestructible		// d'objet solid
		&&	elementNouvellePosition != map::bombe_poser
		&&	elementNouvellePosition != map::bombe_poser_AVEC_UN_joueur
		&&	elementNouvellePosition != map::bombe_poser_AVEC_plusieurs_joueurs))
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
			c_joueurs[joueur].defArmements(0);// Mort
			c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::vide);
			break;
		}
		case map::bonus: {
			if( !c_map->getBlock(newX, newY)->joueur )
				stdErrorE("BONUS: Probl�me de pointeur ! c_map->getBlock(%u, %u)->joueur = 0 !", newX, newY);
			if( !c_map->getBlock(newX, newY)->joueur->size() )
				stdErrorE("BONUS: Probl�me de contenu ! c_map->getBlock(%u, %u)->joueur->size() = 0 !", newX, newY);
			if( c_map->getBlock(newX, newY)->joueur->at(0) >= NB_ELEMENT_t_Bonus )
				stdErrorE("BONUS: Probl�me de contenu ! c_map->getBlock(%u, %u)->joueur->at(0){%u} >= {%d}NB_ELEMENT_t_Bonus !", newX, newY, (unsigned int)c_map->getBlock(newX, newY)->joueur->at(0), NB_ELEMENT_t_Bonus);
			// Ajout de l'armement
			c_joueurs[joueur].armements()->incQuantiteMAX_en_stock((bonus::t_Bonus)c_map->getBlock(newX, newY)->joueur->at(0));
			c_map->setBlock(newX, newY, map::vide);// On surrpime tout ce qui avait avant
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
* @fn void partie::checkInternalEvent()
* @brief Analyse et traite les Event internes pour une partie F4A
* @todo la selection d'Event
*/
void partie::checkInternalEvent()
{
	//static s_Event& e;
	static unsigned int i;


	if( clock() >= c_timeOut ){
		for( unsigned x=0, y=0; y<c_map->Y(); y++ )
		{
			for( x=0; x<c_map->X(); x++ )
			{
				if( c_map->getBlock(x, y)->element == map::Mur_destructible )
					c_map->setBlock(x, y, map::vide);
			}
		}
	}

	for( i=0; i<c_listEvent.size(); i++ )
	{
		// Tant que l'on a pas atteint le temps
		if( c_listEvent.at(i).repetionSuivante > clock() ) continue;


		s_Event& e = c_listEvent.at(i);
		// A FAIRE
		// On select le type d'event
		//

		// Encore un tour ?
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
			c_listEvent.erase(c_listEvent.begin()+i);

		}else{// ENCORE un tour

			// Cr�ation de la d�flagration
			if( e.Nb_Repetition == 0 ){// On ne traite que le centre le 1er tour
				killPlayers(&e, e.pos.x, e.pos.y);// Adition des flammes avec le reste de l'environement
				c_map->setBlock(e.pos.x, e.pos.y, map::flamme_origine);
				e.deflagration.push_back( coordonneeConvert(e.pos.x, e.pos.y) );

				c_listEvent.at(i).Nb_Repetition++;
				c_listEvent.at(i).repetionSuivante = clock() + VITESSE_DEFLAGRATION_FLAMME;// * CLOCKS_PER_SEC;// Ajustement du temps

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

			c_listEvent.at(i).Nb_Repetition++;
			c_listEvent.at(i).repetionSuivante = clock() + VITESSE_DEFLAGRATION_FLAMME;// * CLOCKS_PER_SEC;// Ajustement du temps
		}
	}
}



char partie::actionSurLesElements( s_Event* e, unsigned int x, unsigned int y, char direction )
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
					if( e->pos.y+(unsigned int)e->Nb_Repetition == e->Nb_Repetition_MAX || continuer == false ){// Si on a atteint la taille max
						c_map->setBlock(x, y, map::flamme_pointe_haut);
					}else{
						c_map->setBlock(x, y, map::flamme_vertical);
					}
					break;
				}
				case 2:{// +Y
					if( e->pos.y+(unsigned int)e->Nb_Repetition == e->Nb_Repetition_MAX || continuer == false ){// Si on a atteint la taille max
						c_map->setBlock(x, y, map::flamme_pointe_bas);
					}else{
						c_map->setBlock(x, y, map::flamme_vertical);
					}
					break;
				}
				case -1:{// -X
					if( e->pos.x+(unsigned int)e->Nb_Repetition == e->Nb_Repetition_MAX || continuer == false ){// Si on a atteint la taille max
						c_map->setBlock(x, y, map::flamme_pointe_gauche);
					}else{
						c_map->setBlock(x, y, map::flamme_horizontal);
					}
					break;
				}
				case 1:{// +X
					if( e->pos.x+(unsigned int)e->Nb_Repetition == e->Nb_Repetition_MAX || continuer == false ){// Si on a atteint la taille max
						c_map->setBlock(x, y, map::flamme_pointe_droite);
					}else{
						c_map->setBlock(x, y, map::flamme_horizontal);
					}
					break;
				}
			}
			c_map->ajouterInfoJoueur(x, y, e->joueur, 1);// On dit a qui appartient la flamme
			break;
		}
	}
	return continuer;
}


/***************************************************************************//*!
* @fn char partie::killPlayers( s_Event* e, unsigned int x, unsigned int y )
* @brief Tue les joueurs et d�truit les block destructibles (Mode: F4A)
* @param[in,out] e L'event en cours
* @param[in] x Position X de la map a check
* @param[in] y Position Y de la map a check
* @return 3 Valeur de retoures possible<br />
*			- -1 On stop tout et on ne touche pas au block � cette adresse ! (Out of map ou Mur_INdestructible)
*			-  0 On stop tout mais on traite le block � l'adresse (Mur_destructible)
*			-  1 On continue
*
* @note Les valeurs de X et de Y sont compar�es � la taille de la map => Pas de risque de sortir de la map
*/
char partie::killPlayers( s_Event* e, unsigned int x, unsigned int y )
{
	// k sert dans les boucle for comme compteur.
	// Peut �tre utilis�e sans probl�me
	static unsigned int k;// �a ne sert � rien de la recr�er � chaque fois

	if( x >= c_map->X() || y >= c_map->Y() )
		return -1;

	switch( c_map->getBlock(x, y)->element )// Mont� vers le haut
	{
		case map::UN_joueur: {// Le pauvre joueur qui est au millieu du chemin de la flamme -> dead
			c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].defArmements(0);
			return 1;
		}
		case map::plusieurs_joueurs: {// Les pauvres joueurs qui sont au millieu du chemin de la flamme -> dead
				for( k=0; k<c_map->getBlock(x, y)->joueur->size(); k++ )
				{
					c_joueurs[c_map->getBlock(x, y)->joueur->at(k)-1].defArmements(0);
				}
			return 1;
		}
		case map::bombe_poser_AVEC_UN_joueur: {// Le pauvre joueur qui est au millieu du chemin de la flamme -> dead
			// On fait explos� la bombe
			c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].armements()->forceTimeOut(x, y);
			c_joueurs[c_map->getBlock(x, y)->joueur->at(1)-1].defArmements(0);
			return 0;
		}
		case map::bombe_poser_AVEC_plusieurs_joueurs: {// Les pauvres joueurs qui sont au millieu du chemin de la flamme -> dead
				for( k=1; k<c_map->getBlock(x, y)->joueur->size(); k++ )
				{
					c_joueurs[c_map->getBlock(x, y)->joueur->at(k)-1].defArmements(0);
				}
				// On fait explos� la bombe
				c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].armements()->forceTimeOut(x, y);
			return 0;
		}
		case map::bombe_poser: {
			if( !c_map->getBlock(x, y)->joueur )
				stdErrorE("Probl�me de pointeur vide ! c_map->getBlock(%u, %u)->joueur = 0", x, y);
			if( !c_map->getBlock(x, y)->joueur->size() )
				stdErrorE("Vector vide � x=%u, y=%u", x, y);

			// On fait explos� la bombe
			c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].armements()->forceTimeOut(x, y);
			break;
			return 0;
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
			// On regarde si le block sur lequel on est n'est pas un block que l'on a d�truit
			// Si c'est le cas, on regarde si c'est nous qui l'avons d�truit
			//s_Coordonnees pos=coordonneeConvert( x, y );
			// On parcours la liste des blocks qui ont �t� d�truit
			/*
			for( k=0; k<c_listBlockDetruit.size(); k++ )
				if(	coordonneeEgal(c_listBlockDetruit.at(k).pos, pos) ) // Si on trouve notre block
					if( c_listBlockDetruit.at(k).joueur == joueur )// Si on est � l'origine de la destruction du block => STOP
						return -1;
					//else // On est pas � l'origine de la destruction du block => on lui passe dessus ^^ et on continue la progression mortelle
			*/
			break;
		}
		case map::bonus: {
			if( c_map->getBlock(x, y)->joueur )
				c_map->getBlock(x, y)->joueur->clear();
			break;
		}
		default: {// On eject les autres cas
			break;
		}
	}

	return 1;
}


/***************************************************************************//*!
* @fn const char* partie::packIt( uint32_t X, uint32_t Y, map::t_type type, uint32_t nb_MetaDonnee )
* @brief Empaquette les variables X, Y, type, nb_MetaDonnee pour les envoyer sur le r�seau
* @return Les variables format� dans une chaine de caract�re
*/
const char* partie::packIt( uint32_t X, uint32_t Y, map::t_type type, uint32_t nb_MetaDonnee )
{
	unsigned int PACK_size = snprintf(c_buffer, PACK_bufferSize, "%u,%u,%u,%u", X, Y, type, nb_MetaDonnee);
	if( PACK_size >= PACK_bufferSize )
		stdError("ATTENTION ! Perte de donnees ! %d donnees lues et %d donnees ecrites Maximum ( Ne pas oublier dans les calcules le \\0 ! )", PACK_size, PACK_bufferSize);

	return c_buffer;
}


/***************************************************************************//*!
* @fn const char* partie::packIt( std::vector<unsigned char>* list )
* @brief Empaquette tout ce que contient le tableau
* @return Les variables format� dans une chaine de caract�re
*/
const char* partie::packIt( std::vector<unsigned char>* list )
{
	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);

	unsigned int p=0;
	for( unsigned int i=0; i<list->size(); i++ ){
		if( p >= PACK_bufferSize )
			stdErrorE("D�passement de capacit� !");

		c_buffer[p] = list->at(i)+'0';
		p++;
		c_buffer[p] = ' ';
		p++;
	}
	c_buffer[p-1] = 0;

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

	strcpy(c_buffer, "1 0");
	c_buffer[2] += (int)t;

	return c_buffer;
}


/***************************************************************************//*!
* @fn void partie::unPackIt( uint32_t* X, uint32_t* Y, map::t_type* type, uint32_t* nb_MetaDonnee )
* @brief D�sempaquette le buffer et met les donn�es dans les variables X, Y, type et nb_MetaDonnee
*/
void partie::unPackIt( uint32_t* X, uint32_t* Y, map::t_type* type, uint32_t* nb_MetaDonnee )
{
	unsigned int i=0;
	bool firstLap = 1;

	stdError("UnPack this: %s", c_buffer);

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
	for( ; c_buffer[i] != ','; i++ )
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
	i++;// Pour virer la ,

	*nb_MetaDonnee = 0;
	firstLap = 1;
	for( ; c_buffer[i] != 0; i++ )
	{
		if( '0' <= c_buffer[i] && c_buffer[i] <= '9' ){// Pour contrer le bug SOH
			if( firstLap ){
				firstLap = 0;
			}else{
				*nb_MetaDonnee *= 10;
			}
			*nb_MetaDonnee += c_buffer[i]-'0';
		}
	}

	//sscanf(c_buffer, "%u,%u,%u,%u", X, Y, type, nb_MetaDonnee );
	stdError("UnPacked: %u,%u,%u,%u", *X, *Y, *type, *nb_MetaDonnee);
}


/***************************************************************************//*!
* @fn void partie::unPackIt( uint32_t X, uint32_t Y )
* @brief D�sempaquette le buffer et met les donn�es dans le tableau
*/
void partie::unPackIt( uint32_t X, uint32_t Y )
{
	stdError("UnPack this: %s at x=%u, y=%u", c_buffer, X, Y);

	for( unsigned int i=0; c_buffer[i]; i++ ){
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
	*t = (clavier::t_touche)(c_buffer[2]-'0');
}


/***************************************************************************//*!
* @fn void partie::ajouterNouvelleConnection( SOCKET s )
* @brief Permet d'ajouter un nouveau joueur � la partie
*
* <b>Protocole</b> (Envoie des donn�es)<br />
* Client: 0-1 0-1 (32bits ? Intel ?)<br />
* Server: 1-0 (Connection accord� ?)<br />
* Client: Nom Joueur<br />
* Server: 1-0 (Accord� ou non)<br />
* Server: {Envoie de la taille de la map}
* Server: {Envoie de la map} : "X, Y, type, nb_MetaDonnee"<br />
* Server: Si nb_MetaDonnee > 0 => Envoie des Meta-Donnees
* Server: "end" (Fin de transmission)
*<br />
* Si le perso du client bouge:<br />
* Client: 1 {action: haut, bas, droite, gauche, poser bombe, exploser} (note, 1=action)<br />
* Server: 0-1 (Accord� ou non)(Si accord�, alors le server fait l'action)<br />
* PAS d'envoie de nouvelle position, envoie effectu� lors du rafraichissement.
*/
void partie::ajouterNouvelleConnection( SOCKET s )
{
	int				l_isIntel=1,
					l_is32Bits=1;
	unsigned char	i,
					idJoueur = 0;

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);
	c_server->readClient( s, c_buffer, PACK_bufferSize );// On attend les donn�es du client
	sscanf(c_buffer, "%d %d", &l_is32Bits, &l_isIntel );// Utilisation d'un buffer pour eviter tout probl�me de lecture

	if( Is32Bits != l_is32Bits || l_isIntel != (int)baseClientServer::isLittleEndian() ){
		c_server->rmClient( s );// Connection refus�
		return ;
	}

	c_server->send_message( s, "1", 2);// Connection acc�pt�

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);
	c_server->readClient( s, c_buffer, PACK_bufferSize );// On attend le nom du client

	// On cherche un SLOT pour notre joueur
	// Et on v�rifit si le nom n'est pas dans la liste
	for( i=0; i<c_nb_joueurs; i++ ){
		if( c_joueurs[i].isLocal() ){
			if( *c_joueurs[i].nom() == c_buffer ){// Nom d�j� utilis�
				c_server->rmClient( s );// Nom refus� => D�co
				return ;
			}
		}else{// isLocal() == false
			if( c_joueurs[i].socket() == INVALID_SOCKET ){// Note, on est oblig� de trouver un slot libre, sinon le server n'aurait pas acc�pt� la connection
				idJoueur = i;
				c_joueurs[i].defSocket( s );
				c_joueurs[i].defNom( c_buffer );
				c_joueurs[i].defArmements( new bonus() );
				c_joueurs[i].armements()->param_Par_Defaut();
			}
		}
	}
	c_server->send_message( s, "1", 2);// Nom acc�pt�

	// Envoie des dim de la map
	c_server->send_message(s, packIt(c_map->X(), c_map->Y(), (map::t_type)0, 0), PACK_bufferSize);

	// Envoie de la map
	for( unsigned int x=0, y=0; y<c_map->Y(); y++ )
	{
		for( x=0; x<c_map->X(); x++ )
		{
			if( c_map->getBlock(x, y)->joueur && c_map->getBlock(x, y)->joueur->size() ){
				c_server->send_message(s, packIt(x, y, c_map->getBlock(x, y)->element, c_map->getBlock(x, y)->joueur->size()), PACK_bufferSize);
				c_server->send_message(s, packIt( c_map->getBlock(x, y)->joueur ), PACK_bufferSize);
			}else{
				c_server->send_message(s, packIt(x, y, c_map->getBlock(x, y)->element, 0), PACK_bufferSize);
			}
		}
	}

	c_server->send_message( s, "end", 4);// Fin de transmition
}
