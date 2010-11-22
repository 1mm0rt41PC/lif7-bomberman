#ifndef MAP_h
#define MAP_h

#include "debug.h"
#include "coordonnees.h"
#include <stdlib.h>// Pour fopen, fread
#include <vector>
#include <string>
#include <dirent.h>

/*!
* @class map
* @brief Gère une map.
* @todo Coder les fonctions enleverAuBlock(), ajouterAuBlock()<br />
* - PASSER std::vector<unsigned char>* joueur; -> std::vector<unsigned char> joueur; ???<br />
* - PROBLEME : c'est la map où la partie qui defini quand on meure ?<br />
* Une partie = map + play dessus + style de jeu<br />
* MAIS<br />
* une map c'est un ensemble de block avec des infos dont celle des joueurs<br />
* Surcharger la map ? map(virtual)+map_F4A || map(virtual)+map_CTF
*/
class map
{
	public:
		/*!
		* @enum t_type
		* @brief Les types de d'objet sur la map
		*
		* Aller voir la def dans le .h
		*/
		// A quoi correspond un block
		enum t_type {
			inconnu,// BUG
			vide,
			// MUR
			Mur_destructible,
			Mur_INdestructible,
			// Flammes
			flamme_horizontal,
			flamme_vertical,
			flamme_origine,
			flamme_pointe_haut,
			flamme_pointe_bas,
			flamme_pointe_droite,
			flamme_pointe_gauche,
			// Armes
			bombe_poser,
			// Bonus
			gain_bombe,
			gain_puissance_flamme,
			gain_declancheur_manuel,
			// Player
			UN_joueur,
			plusieurs_joueurs,
			// Combinaison
			bombe_poser_AVEC_UN_joueur,
			bombe_poser_AVEC_plusieurs_joueurs,
			// Points de départ
			point_de_depart_j1,
			point_de_depart_j2,
			point_de_depart_j3,
			point_de_depart_j4
		};

		/*!
		* @struct s_Case
		* Exemple:<br />
		* - flamme à QUI ? => La liste contient : le num du joueur qui a provoqué ces flammes<br />
		* - bombe_poser à QUI ? => La liste contient : le num du joueur qui a posé la bombe<br />
		* - UN_joueur QUI ? => La liste contient : le num du joueur qui est à cette endroit<br />
		* - plusieurs_joueurs QUI ... ? => La liste contient : les num des joueurs qui sont à cette endroit<br />
		* - bombe_poser_AVEC_UN_joueur QUI et QUI ? => La liste contient : (dans l'ordre !)<br />
		*		1) Le num du joueur a qui appartient la bombe.<br />
		*		2) Le num du joueur qui est à cette position<br />
		* - bombe_poser_AVEC_plusieurs_joueurs QUI et QUI ... ? => La liste contient : (dans l'ordre !)<br />
		*		1) Le num du joueur a qui appartient la bombe.<br />
		*		2 à ...) Les num des joueurs qui sont à cette position.<br />
		*/
		typedef struct {
			t_type element;//!< type d'element.
			std::vector<unsigned char>* joueur;//!< La liste est ici pour obtenir des info sur le perso à l'origine du type t.
		} s_Case;


	// Variables
	private:
		// struct map {
		s_Case *c_block;//!< Tableau de case => la map en entère
		s_Coordonnees c_taille;//!< Taille de la map
		s_Coordonnees *c_PointDeDepartJoueur;//!< Liste des points de départ des joueurs ({point_de_depart_jXXX} numJ)
		unsigned char c_nb_PointDeDepartJoueur;//!< Le nombre de points de départ => le nombre maximum de joueur sur une map
		//}


	// Fonctions
	public:
		// Modificateurs
		map();
		map( unsigned int tailleX, unsigned int tailleY );
		~map();
		void setBlock( unsigned int X, unsigned int Y, map::t_type what );
		void ajouterInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierePosition=0 );
		void rmInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierEltInclu );
		void setTaille( unsigned int tailleX, unsigned int tailleY );
		int chargerMap( const char fichier[]=0 );
		s_Coordonnees mettreJoueurA_sa_PositionInitial( unsigned char joueur );

		// Accesseurs
		const s_Case* getBlock( unsigned int X, unsigned int Y ) const;
		const s_Case* getBlock( s_Coordonnees pos ) const;
		unsigned int X() const;
		unsigned int Y() const;
		s_Coordonnees positionInitialJoueur( unsigned char joueur ) const;
		unsigned char nb_PointDeDepartJoueur() const;
		inline unsigned char nb_MAX_Joueur() const { return nb_PointDeDepartJoueur(); }
		unsigned int nb_InfoJoueur( unsigned int X, unsigned int Y ) const;

		// Autres
		static int myRand(int a, int b);
		static void readDir( std::vector<std::string>* files );
};

#endif

