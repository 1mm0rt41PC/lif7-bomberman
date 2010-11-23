#ifndef PARTIE_h
#define PARTIE_h

#include "options.h"
#include "perso.h"
#include "map.h"

// Vitesse de déflagration en milli-sec
#define VITESSE_DEFLAGRATION_FLAMME 500


class partie
{
	private:
		/*!
		* @struct s_Event
		* @brief Permet de gérer les event graphiques.
		*
		* Exemple: La duréer d'une animation
		*/
		typedef struct {
			bonus::t_Bonus type;//!< Type de bonus
			s_Coordonnees pos;//!< Position Originel de l'event
			unsigned char joueur;//!< Le joueur qui est la cause de l'event
			unsigned char Nb_Repetition;//!< Nombre de répétition actuel pour l'event
			unsigned char Nb_Repetition_MAX;//!< Nombre de répétition MAX pour l'event
			clock_t repetionSuivante;//!< Time de la prochaine répétion
		} s_Event;


		/*!
		* @struct s_BlockExploser
		* @brief Permet de gérer les block qui sont détruit lors des explostions
		*/
		typedef struct {
			unsigned char joueur;//!< Le joueur qui est la cause de l'explostion
			s_Coordonnees pos;//!< Position de l'explostion
		} s_BlockExploser;


	public:
		enum t_MODE {
			Combat_Libre,		// Free For All 	( F4A )
			Attrape_drapeau		// Capture the Flag	( CTF )
		};
		/*!
		* @typedef libAff
		* @brief Signature de la fonction a utiliser pour l'affichage de la map
		*
		* La fonction en question doit prendre en paramètre la partie actuel.<br />
		* Elle doit afficher la map.<br />
		* Puis elle doit renvoyer la touche actuellement appuyée.
		*
		* @see partie::main(libAff * afficherMapEtEvent);
		*/
		typedef SYS_CLAVIER(libAff)(const partie*);


	private:
		// struct {
			map*							c_map;// SIMPLE POINTEUR !
			unsigned char					c_nb_joueurs;
			unsigned char					c_nb_MAX_joueurs;
			perso*							c_joueurs;// Tableau
			t_MODE							c_mode;
			std::vector<s_Event>			c_listEvent;
			std::vector<s_BlockExploser>	c_listBlockDetruit;
		// }

		void deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur );
		void checkInternalEvent();
		char killPlayers( unsigned int x, unsigned int y, unsigned char joueur );
		bool estDansListBlockDetruit( s_Coordonnees& pos );


	public:
		partie();
		~partie();
		// Modificateurs
		void genMap();
		void def_nbJoueurs( unsigned char nb );
		void def_nbMAX_joueurs( unsigned char nb );
		void def_modeJeu( t_MODE m );


		// Accesseurs
		unsigned char nbJoueurs() const;
		unsigned char nbMAX_joueurs() const;
		t_MODE modeJeu() const;
		perso* joueur( unsigned int joueur_numero ) const;
		map* getMap() const;
		unsigned char nbJoueurVivant() const;

		// Autres
		void main(libAff * afficherMapEtEvent);
};

#endif
