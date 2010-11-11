// Define LIB : __LIB_ncurses__, __LIB_SDL__, __LIB_SFML__

#define __LIB_ncurses__ 1

#ifdef __LIB_SFML__
	#include <SFML/...>
	#define SYS_CLAVIER

#elif __LIB_ncurses__
	#if defined(WIN32) || defined(WIN64)
		//#include <curses.h>
		#include <Ncurses/curses.h>
	#else
		#include <ncurses.h>
	#endif
	#define SYS_CLAVIER int

	// Touches
	#define KEY_ESCAP 27
	#define KEY_ENTER_bis 10
	#define KEY_BACKSPACE_bis 8

	#ifndef PADPLUS
	#define PADPLUS '+'
	#endif

	#ifndef PADMINUS
	#define PADMINUS '-'
	#endif

#elif __LIB_SDL__
	#include <SDL/SDL.h>
	#define SYS_CLAVIER SDL_keysym // || SDLKey

#else
	#error "Veuillez definir la lib d'affichage ! ( __LIB_ncurses__, __LIB_SDL__, __LIB_SFML__ ) !"
#endif
