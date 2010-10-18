#define LIB_SFML 1
#define LIB_SDL 2
#define LIB_NCURSES 3
#define LIB_AFF LIB_NCURSES

#if LIB_AFF == LIB_SFML
	#include <SFML/...>
	#define SYS_CLAVIER 
	
#elif LIB_AFF == LIB_NCURSES
	#ifdef WIN32
		//#include <curses.h>
		#include <Ncurses/curses.h>
	#else
		#include <ncurses.h>
	#endif
	#define SYS_CLAVIER int
		
#elif LIB_AFF == LIB_SDL
	#include <SDL/SDL.h>
	#define SYS_CLAVIER SDL_keysym // || SDLKey
	
#else
	#error "Valeur INCORRECT pour LIB_AFF !"
#endif
