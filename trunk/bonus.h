#ifndef BONUS_h
#define BONUS_h

#include "debug.h"
#include <time.h>

/*!
* @class bonus
* @brief G�re les bonus.
*
* Cette classe poss�de 2 couches de fonctionnement:
* -# La couche g�n�ral, qui regroupe les bonus disponibles pour la partie.<br />
* (En commum avec toutes les instances de cette class)
* <b>A initialiser en 1er !via bonus::bonusProp()</b>
* -# La couche instanciable, unique a chaque instance.
*
* @todo Meilleur syst�me de gestion des bonus
*/
class bonus
{
	public:
		/*!
		* @enum t_Bonus
		* @brief Les types de bonus possible dans le jeu
		*/
		enum t_Bonus{
			bombe=0,
			puissance_flamme,
			declancheur,
			vitesse,
			vie
		};

		/*!
		* @struct s_bonus_proprieter
		* @brief Cette structure accueil la propri�t� des bonus
		* @var probabiliter_pop	Pourcentage q'un bonus pop (valeur: entre 0 et 100 au max)
		* @var probabiliter_pop	Pourcentage q'un bonus pop (valeur: entre 0 et 100 au max)
		*/
		typedef struct {
			//t_Bonus type; // <- Le type est stock� dans le num�ro du tableau [0] -> [t_Bonus::bombe]
			unsigned char probabiliter_pop;
			unsigned char quantite_MAX_Ramassable;
			/*
			* La dur�e peut avoir 3 status
			* -1 => L'objet est utilis� sans limite de temps (Dur�e Illimit�)
			*  0 => L'objet est utilis� directement (Dur�e instantan�)
			* >0 => L'objet est utilis� au bout de "XXX" (valeur dans la variable)
			*/
			int duree;
		} s_bonus_proprieter;

	private:
		// Cette sturcture accueil un bonus d'un joueur ( un tableau de cette structure forme l'ensemble des bonus d'un joueur )
		/*!
		* @struct s_bonus
		* @brief Cette sturcture accueil un bonus d'un joueur.
		*
		* ( un tableau de cette structure forme l'ensemble des bonus d'un joueur )
		* quantite_utilisable: Nombre d'objet poss�d� actuellement et utilisable
		* quantite_MAX_en_stock: Quantit� Maxi d'un objet ( ex: 5 bonus de type bombe => 5 Bombes max en stock ! )
		*/
		typedef struct {
			t_Bonus type;
			unsigned char quantite_utilisable;// Nombre d'objet poss�d� actuellement et utilisable
			unsigned char quantite_MAX_en_stock;// Quantit� Maxi d'un objet
		} s_bonus;

		static s_bonus_proprieter *C_bonusProp;// Liste des bonus charg� et utilisables sur la map ( cette var n'est remplis q'une fois ! (static) )
		s_bonus* c_liste;// Liste des bonus attrap� par un player. ( bonus utilisable )
		unsigned char c_nb;// Nombre de bonus obtenu par un player

		static clock_t calcFinEvent();


	public:
		bonus();// Constructeur
		~bonus();// Destructeur
		static s_bonus_proprieter* bonusProp();// Constructeur pour C_bonusProp
		static void unInitBonusProp();// Destructeur pour inclut C_bonusProp

		// Accesseurs
		bool aDeja( t_Bonus b ) const;// Permet de savoir si un perso a un bonus ( existe et non null )
		int est_Dans_La_Liste( t_Bonus b ) const;// Permet de savoir si un bonus est d�j� dans la liste
		unsigned char quantiteUtilisable( t_Bonus b ) const;
		unsigned char quantiteMAX( t_Bonus b ) const;
		unsigned char quantiteMAX_Ramassable( t_Bonus b ) const;
		unsigned char probabiliter( t_Bonus b ) const;

		// Modificateurs
		bool incQuantiteUtilisable( t_Bonus b );// Incr�menter quantite_utilisable
		bool incQuantiteMAX_en_stock( t_Bonus b );// Incr�menter quantite_MAX_en_stock
		bool decQuantiteUtilisable( t_Bonus b );// D�cr�menter quantite_utilisable
		bool decQuantiteMAX_en_stock( t_Bonus b );// D�cr�menter quantite_MAX_en_stock
		void param_Par_Defaut();
		void defQuantiteUtilisable( t_Bonus b, unsigned char quantite_utilisable );
		void defQuantiteMAX( t_Bonus b, unsigned char quantite_MAX_en_stock );
		void defQuantiteMAX_Ramassable( t_Bonus b, unsigned char quantite_MAX_Ramassable );
		void defProbabiliter( t_Bonus b, unsigned char probabiliter_pop );
		void defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock );
		void defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop );
};

#endif
