Le programme proposé est un bomberman réalisé en Ncurses et SDL avec des parties en ligne possible.

Comment compiler ?
> Pour compiler le projet sous Windows
Avoir au préalable l'outils sed dans le dossier .tools ( Le dossier .tools doit se trouver dans le dossier du projet !)
De plus il vous faudra:  make, g++ et la librairie de votre choix ( NCurses ou SDL )
> Pour compiler le projet sous Unix/GNU avoir les outils
Il vous faudra: make, g++ et la librairie de votre choix ( NCurses ou SDL )

Les librairies:
	Sous windows:
		- NCurses
			pdcurses
		- SDL
			ming32, SDLmain, SDL, SDL_ttf, SDL_image
	Sous Unix/GNU:
		- NCurses
			ncurses
		- SDL
			SDL, SDL_ttf, SDL_image


Pour lancer la compilation :
	- Ncurses
		make ncurses
	- SDL
		make sdl
		
		
Pour compiler en mode debug avec gdb (niveau 3):
	- Ncurses
		make ncurses debug
	- SDL
		make sdl debug
		
		
Pour compiler puis executer le projet:
	- Ncurses
		make ncurses exec
	- SDL
		make sdl exec
		
		
Pour compiler en mode debug avec gdb (niveau 3) puis lancer le projet avec gdb:
	- Ncurses
		make ncurses debug exec
	- SDL
		make sdl debug exec
		

Pour plus d'informations sur les commandes disponnible:
	make help
	
	
Extentions des fichiers.
	Tous les fichiers .inl contiennent des fonctions inline


Note: Par défaut, la lib utilisé est ncurses, vous n'êtes donc pas obligé de précisé ncurses. Un simple make compilera en NCurses.
Note2: Par défaut, le Tracage mémoire est tout le temps activé, pour le désactiver, commentez la ligne DEBUG_MEMORY = 1 dans MakeFile.org
Note3: Le MakeFile a modifier est dans TOUT les cas MakeFile.org, makefile est un simple "pointeur"
Fun: make wc


TODO
> Finir la partie SFML


--> make help <--
make ARG [no_clear]
        all:            Construit tout le projet.
        debug:          Construit tout le projet en debug (gdb).
        ncurses:        Construit tout le projet avec la lib Ncurses.
        sdl:            Construit tout le projet avec la lib SDL.
        sfml:           Construit tout le projet avec la lib SFML.
        clean:          Supprime les fichiers .o, et le fichier de
                        dependances.
        clean_all:      Supprime les fichiers .o, l'executable et le fichier de
                        dependances.
        clean_dep:      Supprime le fichier des dependances.
        depend:         Construit le fichier des dependances - Le fichier est :
                        supprime a chaque fin de compilations et est donc auto
                        regenere a chaque compilation
        dep:            IDEM {make depend}.
        valgrind:       Compil le projet puis lance valgrind sur le projet.
        tar:            Archive les fichiers dans un fichier tar.bz2
        doxygen:        Execute doxygen sur le projet.
        no_clear:       Annule tous les effacements d'ecran
        exec:           {make all} et execute l'executable obtenu.
        memcheck:       {make all} avec ajout du systeme de debug memoire,
                        execute l'executable obtenu et retourne les resultats
                        memoire.
        memchk:         Alias de memcheck
        exec:           {make all} et execute l'executable obtenu.
        pfiles:         Affiche les fichiers utilises
        wc:             Affiche le nombre de lignes ecrites dans ce projet
        help:           Affiche cette aide.

/!\ Si MAKE n'a pas d'ARG alors IDEM a {make all} avec la lib ncurses.
/!\ L'outil sed est utilise par ce makefile pour la generation des dependances !