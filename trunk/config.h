// Define LIB : __LIB_ncurses__, __LIB_SDL__, __LIB_SFML__

//#define __LIB_SDL__ 1

#if defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	#define OS_WINDOWS 1
#else
#endif

/*******************************************************************************
* Lib SFML
*/
#ifdef __LIB_SFML__
	#include <SFML/...>
	#define SYS_CLAVIER


/*******************************************************************************
* Lib NCruses
*/
#elif __LIB_ncurses__
	#if defined(OS_WINDOWS)
		#include <Ncurses/curses.h>
	#else
		#include <ncurses.h>
	#endif
	#define SYS_CLAVIER int
	#define CLASS_TO_USE moteur_ncurses

	// Touches
	#define KEY_ESCAP 27
	#define KEY_ENTER_bis 10
	#define KEY_BACKSPACE_bis 8
	#define RETOUR_MENU_PRECEDENT KEY_ESCAP// Permet de faire la liaison pour quitter le menu principal

	#ifndef PADPLUS
		#define PADPLUS '+'
	#endif

	#ifndef PADMINUS
		#define PADMINUS '-'
	#endif


/*******************************************************************************
* Lib SDL
*/
#elif __LIB_SDL__
	#include <SDL/SDL.h>
	#define SYS_CLAVIER SDLKey //Uint8 || SDLKey
	#define CLASS_TO_USE moteur_sdl
	#define RETOUR_MENU_PRECEDENT SDLK_ESCAPE// Permet de faire la liaison pour quitter le menu principal


/*******************************************************************************
* BUG
*/
#else
	#error "Veuillez definir la lib d'affichage ! ( __LIB_ncurses__, __LIB_SDL__, __LIB_SFML__ ) !"
#endif
