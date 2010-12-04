#ifndef PARTIE_h
#define PARTIE_h

#include "options.h"
#include "perso.h"
#include "map.h"


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
			std::vector<s_Coordonnees>	listBlockDetruit;//!< Contient la liste des block qui ont été détruit => Bonus a la clef ^^
			std::vector<s_Coordonnees>	deflagration;//!< Contient la position de tous block qui ont été touchés par la déflagration
			s_Coordonnees				pos;//!< Position Originel de l'event
			bonus::t_Bonus				type;//!< Type de bonus
			clock_t						repetionSuivante;//!< Time de la prochaine répétion
			unsigned char				joueur;//!< Le joueur qui est la cause de l'event
			unsigned char				Nb_Repetition;//!< Nombre de répétition actuel pour l'event
			unsigned char				Nb_Repetition_MAX;//!< Nombre de répétition MAX pour l'event
			bool						continue_X,
										continue_negativeX,
										continue_Y,
										continue_negativeY;
		} s_Event;


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
		//typedef SYS_CLAVIER(CLASS_TO_USE::*fctAff)(const partie*);
		typedef SYS_CLAVIER(libAff)(const partie*);


	private:
		// struct {
			std::vector<s_Event>			c_listEvent;
			map*							c_map;// SIMPLE POINTEUR !
			perso*							c_joueurs;// Tableau
			t_MODE							c_mode;//4
			unsigned char					c_nb_joueurs;//1
			unsigned char					c_nb_MAX_joueurs;//1
		// }

		void deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur );
		void checkInternalEvent();
		char actionSurLesElements( s_Event* e, unsigned int x, unsigned int y, unsigned int ValeurPositionOriginel, char direction );
		char killPlayers( s_Event* e, unsigned int x, unsigned int y );


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
		//void start( CLASS_TO_USE& moteur, fctAff afficherMapEtEvent );
		void main( libAff * afficherMapEtEvent );
};

#endif
