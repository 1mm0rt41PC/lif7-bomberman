#include "map.h"
#include "debug.h"

typedef std::string string;


/***************************************************************************//*!
* @fn map::map()
* @brief Constructeur :: Initialise juste les variables. NE Cr�er PAS de map !
*
* Taille de la map = 0<br />
* Pas de points de d�part<br />
*/
map::map()
{
	c_block = 0;
	c_taille.x = 0;
	c_taille.y = 0;
	c_PointDeDepartJoueur = 0;
	c_nb_PointDeDepartJoueur = 0;
}


/***************************************************************************//*!
* @fn map::map( unsigned int tailleX, unsigned int tailleY )
* @brief Constructeur :: Cr�er une map vide !
*/
map::map( unsigned int tailleX, unsigned int tailleY )
{
	if( tailleX != tailleY )
		stdErrorE("Erreur ! La map doit �tre carr�e ! %u != %u", tailleX, tailleY);

	c_taille.x = tailleX;
	c_taille.y = tailleY;
	c_block = new s_Case[ tailleX * tailleY ];

	for( unsigned int i=0; i<tailleX*tailleY; i++ )
	{
		c_block[i].element = map::vide;
		c_block[i].joueur = 0;
	}

	c_PointDeDepartJoueur = 0;
	c_nb_PointDeDepartJoueur = 0;
}


/***************************************************************************//*!
* @fn map::~map()
* @brief D�sinitialise la class map
*
* - Supprime la map charg�
* - Supprime toutes les Info Joueurs (meta donn�es)
* - Supprime les points de d�part
* - Met toutes les variables � 0
*/
map::~map()
{
	// Suppression de la map si elle existe
	if( c_block ){
		// Suppression des meta donn�es
		for( unsigned int i=0; i<c_taille.x*c_taille.y; i++ )
		{
			if( c_block[i].joueur )
				delete c_block[i].joueur;
		}
		delete[] c_block;
	}

	// Suppression des points de d�part s'ils existent
	if( c_PointDeDepartJoueur )
		delete[] c_PointDeDepartJoueur;

	c_block = 0;
	c_taille.x = 0;
	c_taille.y = 0;
	c_PointDeDepartJoueur = 0;
	c_nb_PointDeDepartJoueur = 0;
}


/***************************************************************************//*!
* @fn void map::setBlock( unsigned int X, unsigned int Y, map::t_type what )
* @brief Modifie le block qui est � la position X, Y
*
* Ne modifie en aucun cas les Info joueurs ( meta donn�es )<br />
* <b>/!\\ATTENTION EXCEPTION: si what = vide => n�toyage de toutes les meta donn�es /!\\</b>
*/
void map::setBlock( unsigned int X, unsigned int Y, map::t_type what )
{
	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, what=%d", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, what);

	c_block[X * c_taille.x + Y].element = what;
	c_listDesChangement.push_back( coordonneeConvert(X,Y) );

	// N�toyage des m�ta donn�es si le block est vide
	if( what == vide && c_block[X * c_taille.x + Y].joueur )
		c_block[X * c_taille.x + Y].joueur->clear();
}


/***************************************************************************//*!
* @fn void map::ajouterInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierePosition )
* @brief Ajoute des meta donn�e ( ou info joueurs ) dans le block (X, Y)
* @param[in] X Coordonn�e
* @param[in] Y Coordonn�e
* @param[in] id_Joueur L'id du joueur a ajouter au block (meta-info)
* @param[in] premierePosition Placer l'id du joueur en 1�re position ?
*
* NOTE: Pour certain map::type comme bombe_poser, le 1er element repr�sente l'id du joueur qui a pos� l'objet
*/
void map::ajouterInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierePosition )
{
	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, id_Joueur=%d, premierePosition=%d", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, (int)id_Joueur, (int)premierePosition);

	s_Case* c = c_block + (X * c_taille.x + Y);

	if( !c->joueur )
		c->joueur = new std::vector<unsigned char>;

	if( premierePosition )
		c->joueur->insert( c->joueur->begin(), id_Joueur );
	else
		c->joueur->push_back( id_Joueur );

	if( c_listDesChangement.size() && (c_listDesChangement.back().x != X || c_listDesChangement.back().y != Y) )
		// Si les changement effectu�s ne sont pas d�j� mit dans la liste => on ajoute ces coordonn�es
		c_listDesChangement.push_back( coordonneeConvert(X,Y) );
}


/***************************************************************************//*!
* @fn void map::rmInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierEltInclu )
* @brief Supprime les info du joueur du block (X, Y)
* @param[in] X Coordonn�e
* @param[in] Y Coordonn�e
* @param[in] id_Joueur L'id du joueur a ajouter au block (meta-info)
* @param[in] premierEltInclu Si le premier element dela liste est l'id du joueur => SUPPRIMER ?
*
* NOTE: Pour certain map::t_type comme @e bombe_poser, le 1er element repr�sente l'id du joueur qui a pos� l'objet
*/
void map::rmInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierEltInclu )
{
	std::vector<unsigned char>* vct = NULL;

	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, id_Joueur=%u, premierEltInclu=%d", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, (unsigned int)id_Joueur, (int)premierEltInclu);

	if( id_Joueur == 0 )
		stdErrorE("ID de joueur incorrect ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, id_Joueur=%u, premierEltInclu=%d", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, (unsigned int)id_Joueur, (int)premierEltInclu);

	if( !(vct = c_block[X * c_taille.x + Y].joueur) )
		stdErrorE("Pas de meta donn�e pour le block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, id_Joueur=%u, premierEltInclu=%d", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, (unsigned int)id_Joueur, (int)premierEltInclu);

	for( unsigned int i=!premierEltInclu; i<vct->size(); i++ )
	{
		if(vct->at(i) == id_Joueur)
			vct->erase(vct->begin()+i);
	}
}


/***************************************************************************//*!
* @fn void map::rmInfoJoueur( unsigned int X, unsigned int Y )
* @brief Supprime toutes les meta-info du block (X, Y)
*
* NOTE: Pour certain map::t_type comme @e bombe_poser, le 1er element repr�sente l'id du joueur qui a pos� l'objet
*/
void map::rmInfoJoueur( unsigned int X, unsigned int Y )
{
	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y);

	if( c_block[X * c_taille.x + Y].joueur )
		c_block[X * c_taille.x + Y].joueur->clear();
}


/***************************************************************************//*!
* @fn void map::setTaille( unsigned int tailleX, unsigned int tailleY )
* @brief Modifie la taille de la map.
*
* Si une map existe d�j� => delete de l'ancienne et delete des points de d�part
*/
void map::setTaille( unsigned int tailleX, unsigned int tailleY )
{
	if( tailleX != tailleY )
		stdErrorE("Erreur ! La map doit �tre carr�e ! %u != %u", tailleX, tailleY);

	// Suppression de la map si elle existe
	if( c_block ){
		// Suppression des meta donn�es
		for( unsigned int i=0; i<c_taille.x*c_taille.y; i++ )
			delete c_block[i].joueur;
		delete[] c_block;
	}

	// Suppression des points de d�part s'ils existent
	if( c_PointDeDepartJoueur )
		delete[] c_PointDeDepartJoueur;

	c_taille.x = tailleX;
	c_taille.y = tailleY;
	c_block = new s_Case[ tailleX * tailleY ];

	for( unsigned int i=0; i<tailleX*tailleY; i++ )
	{
		c_block[i].element = map::vide;
		c_block[i].joueur = 0;
	}

	c_PointDeDepartJoueur = 0;
	c_nb_PointDeDepartJoueur = 0;
}


/***************************************************************************//*!
* @fn int map::chargerMap( const char fichier[] )
* @brief Charge une map en m�moire
* @param[in] fichier	Si = 0 => map al�atoire
* @return Renvoie le nombre de joueur maximum pour cette carte
*
* Si la map existe d�j� => ancienne map out<br />
* Un fichier de map a une forme :<br />
* X Y NB_Joueur\n<br />
* #...#\n<br />
*/
int map::chargerMap( const char fichier[] )
{
	char *fichierUtiliser = (char*)fichier;
	FILE* fp=0;
	char c=0;
	std::vector<string> fichiers;

	// Si pas de nom de fichier donn�e => map al�atoire
	if( !fichier ){
		readDir( &fichiers );

		// Ajout du pr�fix ./map
		int r = myRand(0, fichiers.size()-1);
		fichiers.at(r).insert(0, "./map/");
		//fichiers.at(r) = "lvl1.map";// A VIRER EN FINAL ( MODE DEBUG )
		fichierUtiliser = (char *)fichiers[r].c_str();
	}

	if( !(fp=fopen(fichierUtiliser, "r")) )
		stdErrorE("Erreur lors de l'ouverture de la map <%s>", fichierUtiliser);

	if( fscanf(fp, "%2u %2u %2u\n", &c_taille.x, &c_taille.y, (unsigned int*)&c_nb_PointDeDepartJoueur) != 3 ){
		fclose(fp);
		stdErrorE("Erreur lors de la lecture du fichier <%s>", fichierUtiliser);
	}

	// Bonne dimension ?
	if( !c_taille.x || !c_taille.y || c_taille.x != c_taille.y )
		stdErrorE("Les dimensions de la map sont incorrect ! c_taille=(%u,%u) La map doit �tre carr�e !", c_taille.x, c_taille.y);

	// Nombre de joueur correct
	if( !c_nb_PointDeDepartJoueur )
		stdErrorE("Le nombre de joueur pour la map ( ou nombre de point de d�part ) est incorrect ! c_nb_PointDeDepartJoueur=%u", c_nb_PointDeDepartJoueur);

	// Allocation de la map
	if( !(c_block = new s_Case[ c_taille.x * c_taille.y ]) )
		stdErrorE("Erreur lors de l'allocation de la m�moire ! c_taille=(%u,%u)", c_taille.x, c_taille.y);

	// Allocation des points de d�part
	if( !(c_PointDeDepartJoueur = new s_Coordonnees[ c_nb_PointDeDepartJoueur ]) )
		stdErrorE("Erreur lors de l'allocation de la m�moire ! c_taille=(%u,%u)", c_taille.x, c_taille.y);


	for( unsigned int y=0,x; y<c_taille.y; y++ )
	{
		for( x=0; x<c_taille.x; x++ )
		{
			switch( c=fgetc(fp) )
			{
				case EOF:{
					stdErrorE("Fin de fichier pr�matur� ! x=%u, y=%u, c_taille=(%u,%u), <%s>", x, y, c_taille.x, c_taille.y, fichierUtiliser);
					break;
				}
				case '1':
				case '2':
				case '3':
				case '4':{
					c=c-'0';// Convertion en nombre manipulable

					// On v�rif que l'on a pas un joueur qui soit en dehors de ce que l'on a r�serv�
					if( c > c_nb_PointDeDepartJoueur )
						stdErrorE("Joueur inconnu dans la map <%s> ! x=%u, y=%u, joueur=%d", fichierUtiliser, x, y, (int)c);

					c_PointDeDepartJoueur[c-1].x = x;
					c_PointDeDepartJoueur[c-1].y = y;
					c_block[x * c_taille.x + y].element = vide;
					break;
				}
				case ' ':{
					c_block[x * c_taille.x + y].element = vide;
					break;
				}
				case '#':{
					c_block[x * c_taille.x + y].element = Mur_INdestructible;
					break;
				}
				case '%':{
					c_block[x * c_taille.x + y].element = Mur_destructible;
					break;
				}
				default:{
					stdErrorE("Caract�re inatendu dans le fichier <%s> : (code)%d, (aff)%c, x=%u, y=%u", fichierUtiliser, (int)c, c, x, y);
					break;
				}
			}
			c_block[x * c_taille.x + y].joueur = 0;
		}
		fgetc(fp);// Pour virer le \n de la fin
	}

	fclose(fp);

	return c_nb_PointDeDepartJoueur;
}


/***************************************************************************//*!
* @fn void map::readDir( std::vector<std::string>* files )
* @brief Lit le dossier ./map et retourne ( par files ) les fichiers (.map) qu'il contient
* @param[in,out] files Dans cette variable sera stock� les nom des fichiers pr�sent dans le dossier ./map
* @attention Aucun fichier n'est pr�fix� du dossier !
*/
void map::readDir( std::vector<std::string>* files )
{
	DIR* dp;
	dirent* dirp=0;

	if( (dp = opendir("./map")) == NULL )
		stdErrorE("Erreur lors de l'ourverture du dossier ./map");

	string tmp;
	while( (dirp = readdir(dp)) != NULL )
	{
		tmp = dirp->d_name;
		if( tmp[tmp.size()-4] == '.' &&
			tmp[tmp.size()-3] == 'm' &&
			tmp[tmp.size()-2] == 'a' &&
			tmp[tmp.size()-1] == 'p'
		)
			files->push_back(tmp);
	}
	closedir(dp);
}



/***************************************************************************//*!
* @fn s_Coordonnees map::mettreJoueurA_sa_PositionInitial( unsigned char joueur )
* @brief Met le perso {joueur} a sa place initial
* @param[in] joueur	Joueur que l'on veut d�placer � sa position initial ( un nombre entre 1 et 255 )
* @return Renvoie la position initial du joueur.
*
* <b>Vous devez vous m�me supprimer le perso de son ancienne position, si il y en avait une</b><br />
* Si le joueur n'est pas trouv� => Erreur FATAL !
* @warning Le positionnement ne tiens pas compte des obstacles, comme le feu, une bombe, ...
*/
s_Coordonnees map::mettreJoueurA_sa_PositionInitial( unsigned char joueur )
{
	if( joueur == 0 || joueur > c_nb_PointDeDepartJoueur )
		stdErrorE("Le num�ro de joueur Doit �tre entre 1 et %d ! mettreJoueurA_sa_PositionInitial( %d )", (int)c_nb_PointDeDepartJoueur, (int)joueur);

	if( !c_block || c_PointDeDepartJoueur[joueur-1].x >= c_taille.x || c_PointDeDepartJoueur[joueur-1].y >= c_taille.y )
		stdErrorE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, Joueur=%u", (unsigned int)c_block, c_taille.x, c_taille.y, c_PointDeDepartJoueur[joueur-1].x, c_PointDeDepartJoueur[joueur-1].y, (unsigned int)joueur);

	s_Case* c = &c_block[ c_PointDeDepartJoueur[joueur-1].x * c_taille.x + c_PointDeDepartJoueur[joueur-1].y ];
	c->element = UN_joueur;
	if( !c->joueur )
		c->joueur = new std::vector<unsigned char>;
	c->joueur->push_back( joueur );

	return c_PointDeDepartJoueur[joueur-1];
}


/***************************************************************************//*!
* @fn const map::s_Case* map::getBlock( unsigned int X, unsigned int Y ) const
* @brief Retourn le block qui est � la position X, Y
*/
const map::s_Case* map::getBlock( unsigned int X, unsigned int Y ) const
{
	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y);

	return c_block+(X * c_taille.x + Y);
}


/***************************************************************************//*!
* @fn s_Coordonnees map::positionInitialJoueur( unsigned char joueur ) const
* @brief Renvoie la position initial du joueur.
* @param[in] joueur	Joueur dont on veut avoir sa position initial ( un nombre entre 1 et 255 )
*
* Si une position initial n'est pas trouv� pour le joueur => Erreur FATAL !
*/
s_Coordonnees map::positionInitialJoueur( unsigned char joueur ) const
{
	if( joueur == 0 )
		stdErrorE("positionInitialJoueur( 0 ) INTERDIT !");

	if( !c_PointDeDepartJoueur )
		stdErrorE("Charger une map correct avant d'appeler positionInitialJoueur() ! Il n'existe aucun point de d�part !");

	if( joueur > c_nb_PointDeDepartJoueur )
		stdErrorE("Le joueur demand� ne peut �tre positionn� sur cette map ! c_nb_PointDeDepartJoueur=%u, joueur=%u", (unsigned int)c_nb_PointDeDepartJoueur, (unsigned int)joueur);

	return c_PointDeDepartJoueur[joueur-1];
}


/***************************************************************************//*!
* @fn unsigned int map::nb_InfoJoueur( unsigned int X, unsigned int Y ) const
* @brief Renvoie le nombre d'info joueurs dans le block (X, Y) (meta donn�es)
*/
unsigned int map::nb_InfoJoueur( unsigned int X, unsigned int Y ) const
{
	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y);

	s_Case* c = c_block+(X * c_taille.x + Y);

	if( !c->joueur )
		return 0;

	return c->joueur->size();
}


/***************************************************************************//*!
* @fn bool map::getModifications( s_Coordonnees& pos )
* @brief Retourne les positions X,Y des case modifi�es depuis le dernier cycle
* @param[out] pos La variables o� sera stock�e les donn�es de retour
* @return True s'il y a eu des modifications
*
* Utilisation:
* @code
* map myMap;
* s_Coordonnees pos={0,0};
* // Charger une map ici + traitement divers
* // ...
* while( myMap.getModifications(pos) )
* {
*	// pos contient les coodonn�es d'un block modifi�
*	printf("Le block a l'adresse: X=%u, Y=%u a �t� modifi�.\n", pos.x, pos.y);
* }
* // Fin des block modifi�
* @endcode
* @note A chaque appel de la fonction l'element qui est envoy� a pos est enlev� de la liste des modification (First In Last Out)
*/
bool map::getModifications( s_Coordonnees& pos )
{
	if( !c_listDesChangement.size() )
		return false;

	pos = c_listDesChangement.back();// On envoie le dernier element
	c_listDesChangement.pop_back();// On le supprime de la liste

	return true;
}
