Bomberman en C++ réalisé avec les lib Ncurses, SDL ( et SFML en cours ) avec des parties en réseau possible.

# Comment compiler ? #
## Pour compiler le projet sous Windows ##
Avoir au préalable l'outil _sed_ dans le dossier **.tools** ( Le dossier **.tools** doit se trouver dans le dossier du projet !)
De plus il vous faudra:  make, g++ et la librairie de votre choix ( NCurses ou SDL OU SFML )
> Pour compiler le projet sous Unix/GNU avoir les outils
Il vous faudra: make, g++ et la librairie de votre choix ( NCurses ou SDL OU SFML )

### Les librairies: ###
> Sous Windows:
> > - **NCurses**
> > > pdcurses

> > - **SDL**
> > > ming32, SDLmain, SDL, SDL\_ttf, SDL\_image

> Sous GNU/UNIX:
> > - **NCurses**
> > > ncurses

> > - **SDL**
> > > SDL, SDL\_ttf, SDL\_image


Pour lancer la _compilation_ :

> - **Ncurses**
> > make ncurses

> - **SDL**
> > make sdl


Pour _compiler en mode debug avec gdb_ (niveau 3):

> - **Ncurses**
> > make ncurses debug

> - **SDL**
> > make sdl debug


Pour _compiler puis exécuter_ le projet:

> - **Ncurses**
> > make ncurses exec

> - **SDL**
> > make sdl exec


Pour compiler en mode debug avec gdb (niveau 3) puis lancer le projet avec gdb:

> - **Ncurses**
> > make ncurses debug exec

> - **SDL**
> > make sdl debug exec


Pour plus d'informations sur les commandes disponible:

> ~$**make help**



## Extensions des fichiers. ##
> Tous les fichiers **.inl** contiennent des fonctions inline


_Note:_ Par défaut, la lib utilisé est ncurses, vous n'êtes donc pas obligé de préciser ncurses. Un simple make compilera en NCurses.

_Note2:_ Par défaut, le Traçage mémoire n'est plus tout le temps activé, pour l'activer : **make memcheck** OU, dé-commentez la ligne DEBUG\_MEMORY = 1 dans MakeFile.org

_Note3:_ Le `MakeFile` a modifier est dans TOUT les cas `MakeFile.org`, le fichier _makefile_ est un simple "pointeur"

_Fun:_ make wc



## TODO ##
> Finir la partie SFML



### make help ###
```
/*
make ARG [no_clear]
        all:            Construit tout le projet.
        debug:          Construit tout le projet en debug (gdb).
        ncurses:        Construit tout le projet avec la lib Ncurses.
        sdl:            Construit tout le projet avec la lib SDL.
        sfml:           Construit tout le projet avec la lib SFML.
        clean:          Supprime les fichiers .o, et le fichier de
                        dépendances.
        clean_all:      Supprime les fichiers .o, l'exécutable et le fichier de
                        dépendances.
        clean_dep:      Supprime le fichier des dépendances.
        depend:         Construit le fichier des dépendances - Le fichier est :
                        supprime a chaque fin de compilations et est donc auto
                        régénéré a chaque compilation
        dep:            IDEM {make depend}.
        valgrind:       Compil le projet puis lance valgrind sur le projet.
        tar:            Archive les fichiers dans un fichier tar.bz2
        doxygen:        Exécute doxygen sur le projet.
        no_clear:       Annule tous les effacements d'ecran
        exec:           {make all} et exécute l'exécutable obtenu.
        memcheck:       {make all} avec ajout du système de debug mémoire,
                        exécute l'exécutable obtenu et retourne les résultats
                        mémoire.
        memchk:         Alias de memcheck
        exec:           {make all} et exécute l'exécutable obtenu.
        pfiles:         Affiche les fichiers utilises
        wc:             Affiche le nombre de lignes écrites dans ce projet
        help:           Affiche cette aide.

/!\ Si MAKE n'a pas d'ARG alors IDEM a {make all} avec la lib ncurses.
/!\ L'outil sed est utilise par ce makefile pour la génération des dépendances !
*/
```
