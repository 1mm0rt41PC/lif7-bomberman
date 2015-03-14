#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
 
int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) // Démarrage de la SDL. Si erreur alors...
    {
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Ecriture de l'erreur
        exit(EXIT_FAILURE); // On quitte le programme
    }
    
    SDL_EnableUNICODE(1);// On veux pas un clavier QWERTY
    
    SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("Ma super fenêtre SDL !", NULL);
    
    
    int continuer = 1;
    SDL_Event event;
 
    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
			case SDL_KEYDOWN:
				printf("sym: %d | Unicode: %d | SDL_GetKeyName: %s %c\n", event.key.keysym.sym, event.key.keysym.unicode, SDL_GetKeyName(event.key.keysym.sym), event.key.keysym.unicode);
				break;
        }
    }
 
 
    SDL_Quit();
 
    return EXIT_SUCCESS;
}