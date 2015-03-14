#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>


/* Retourne les valeurs sur 8 bits des composantes rouges, vertes et
   bleues par rapport au format donn� */
#define REDVALUE(format, pixel) (Uint8) (((pixel & format->Rmask) << format->Rloss) \
        >> format->Rshift)
#define GREENVALUE(format, pixel) (Uint8) (((pixel & format->Gmask) << format->Gloss) \
        >> format->Gshift)
#define BLUEVALUE(format, pixel) (Uint8) (((pixel & format->Bmask) << format->Bloss) \
        >> format->Bshift)

/* Cr�e les masques correspondants pour les composantes de couleur
   donn�es sur 8 bits */
#define REDMASK(format, red) (Uint32) (((red << format->Rshift) \
        >> format->Rloss) & format->Rmask)
#define GREENMASK(format, green) (Uint32) (((green << format->Gshift) \
        >> format->Gloss) & format->Gmask)
#define BLUEMASK(format, blue) (Uint32) (((blue << format->Bshift) \
        >> format->Bloss) & format->Bmask)

/* Retourne la valeur d'un pixel selon ses 3 composantes de couleur,
   donn�es sur 8 bits */
#define MAKEPIXEL(format, red, green, blue) (Uint32) (REDMASK(format, red) \
        | GREENMASK(format, green) | BLUEMASK(format, blue))

/* Lit et affecte un pixel � partir d'un pointeur */
#define READPIXEL(pointer, pixel) { pixel = *(Uint16 *)(pointer) + \
        (*(Uint8 *)((pointer) + 2) << 16); }
#define STOREPIXEL(pointer, pixel) { *(Uint16 *)(pointer) = (Uint16)(pixel); \
        *(Uint8 *)((pointer) + 2) = (Uint8)((pixel) >> 16) & 0xff; }

/**
 * G�n�re un foyer gris sur la surface donn�e, dans les limites de
 * rect, en fonction des param�tres intensity, vivacity et r�sistance.
 * Attention: la surface doit �tre verrouill�e!
 */
void fireRoot(SDL_Surface * surface, SDL_Rect * rect, Uint8 intensity, Uint8 vivacity, \
        Uint8 resistance) {
  Uint32 i, j;

  /* Pour chaque ligne du foyer, la vivacit� d�termine combien de
     pixels mettre � jour */
  for (j = rect->y; j < rect->y + rect->h; j++)
    for (i = 0; i < vivacity; i++) {
      /* D�terminer la valeur de chaque composante en fonction de
         l'intensit� - elles auront les m�mes valeurs, ce qui nous
         fournira un gris */
      Uint8 color = (intensity + (rand() % (256 - intensity)));
      /* Choisir une position au hasard */
      Uint32 position = (rand() % rect->w) + rect->x;

      /* Et y affecter le pixel */
      STOREPIXEL(surface->pixels + j * surface->pitch + position *
                 surface->format->BytesPerPixel,
                 MAKEPIXEL(surface->format, color, color, color));
    }

  /* M�me chose avec la r�sistance - on �teint al�atoirement des
     pixels */
  for (j = rect->y; j < rect->y + rect->h; j++)
    for (i = 0; i < resistance; i++) {
      Uint32 position = (rand() % rect->w) + rect->x;

      STOREPIXEL(surface->pixels + j * surface->pitch + (position *
              surface->format->BytesPerPixel), 0);
    }
}

/**
 * Applique l'effet de feu sur la surface donn�e, dans les limites de
 * rect.  Attention: la surface doit �tre verrouill�e!
 */
void fire(SDL_Surface * surface, SDL_Rect * rect) {
  Uint32 i, j;

  /* Chaque pixel de la zone doit �tre mis � jour */
  for (j = rect->y; j < rect->y + rect->h; j++)
    for (i = rect->x; i < rect->x + rect->w; i++) {
      /* Pointeur vers le pixel courant */
      void * curpixel;
      /* Nouvelle valeur du pixel courant */
      Uint32 pixvalue = 0;
      /* Addition des valeurs des composantes rouges, vertes et bleues
        des pixels adjacents au pixel courant */
      Uint16 red = 0, green = 0, blue = 0;
      /* Le pixel � mettre � jour */
      curpixel = surface->pixels + (surface->pitch * j) +
              (i * surface->format->BytesPerPixel);
      /* Addition des composantes des 4 pixels adjacents */
      /* Inf�rieur gauche */
      if (i > 0) {
        READPIXEL(curpixel + surface->pitch - surface->format->BytesPerPixel,
                pixvalue);
        red += REDVALUE(surface->format, pixvalue);
        green += GREENVALUE(surface->format, pixvalue);
        blue += BLUEVALUE(surface->format, pixvalue);
      }

      /* Inf�rieur */
      READPIXEL(curpixel + surface->pitch, pixvalue);
      red += REDVALUE(surface->format, pixvalue);
      green += GREENVALUE(surface->format, pixvalue);
      blue += BLUEVALUE(surface->format, pixvalue);

      /* Deux lignes en dessous */
      READPIXEL(curpixel + surface->pitch * 2, pixvalue);
      red += REDVALUE(surface->format, pixvalue);
      green += GREENVALUE(surface->format, pixvalue);
      blue += BLUEVALUE(surface->format, pixvalue);

      /* Inf�rieur droit */
      if (i < surface->w - 1) {
        READPIXEL(curpixel + surface->pitch + surface->format->BytesPerPixel,
                pixvalue);
        red += REDVALUE(surface->format, pixvalue);
        green += GREENVALUE(surface->format, pixvalue);
        blue += BLUEVALUE(surface->format, pixvalue);
      }

      /* Alt�ration des composantes afin d'obtenir la teinte de la
        flamme */
      if (red > 2) red -= 2; else red = 0;
      if (green > 5) green -= 5; else green = 0;
      if (blue > 150) blue -= 150; else blue = 0;

      /* Division par quatre de l'addition des composantes */
      red /= 4; green /= 4; blue /= 4;

      /* Affectation de la nouvelle valeur du pixel */
      STOREPIXEL(curpixel, MAKEPIXEL(surface->format, red, green, blue));
    }
}


/* Variables d'intensit�s, de vivacit� et de r�sistance */
#define DEFAULT_INTENSITY 150
#define DEFAULT_VIVACITY 128
#define DEFAULT_RESISTANCE 75

static Uint8 intensity = DEFAULT_INTENSITY;
static Uint8 vivacity = DEFAULT_VIVACITY;
static Uint8 resistance = DEFAULT_RESISTANCE;

static Sint8 intensity_delta = 0;
static Sint8 vivacity_delta = 0;
static Sint8 resistance_delta = 0;

/* Mise � 1 s'il faut quitter */
Uint8 letsexit = 0;

SDL_Surface * screen, * image;

/* Gestion des entr�es clavier */
void input_update() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
        /* Quitter */
      case SDLK_ESCAPE:
        letsexit = 1;
        break;
        /* Enflammement du logo SDL - il suffit de le dessiner
         et de laisser l'effet de feu faire le reste */
      case SDLK_RETURN:
        {
          SDL_Rect blitRect = { 0, 0, 0, 0 };
          blitRect.x = (rand() % (screen->w - image->w));
          blitRect.y = (rand() % (screen->h - image->h));

          SDL_BlitSurface(image, NULL, screen, &blitRect);
          break;
        }
        /* Augmenter intensit� */
      case SDLK_UP:
        intensity_delta = 1;
        break;
        /* Diminuer intensit� */
      case SDLK_DOWN:
        intensity_delta = -1;
        break;
        /* Augmenter r�sistance */
      case SDLK_RIGHT:
        resistance_delta = 1;
        break;
        /* Diminuer r�sistance */
      case SDLK_LEFT:
        resistance_delta = -1;
        break;
        /* Augmenter vivacit� */
      case SDLK_PAGEUP:
        vivacity_delta = 1;
        break;
        /* Diminuer vivacit� */
      case SDLK_PAGEDOWN:
        vivacity_delta = -1;
        break;
      default:
        break;
      }
      break;
      /* Remise � z�ro du delta des variables de param�tre du foyer si
       une touche leur correspondant est rel�ch�e */
    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      case SDLK_UP:
      case SDLK_DOWN:
        intensity_delta = 0;
        break;
      case SDLK_RIGHT:
      case SDLK_LEFT:
        resistance_delta = 0;
        break;
      case SDLK_PAGEUP:
      case SDLK_PAGEDOWN:
        vivacity_delta = 0;
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
}

int main(int argc, char * argv[]) {
  /* Rectangles dans lesquels vont s'appliquer le foyer et l'effet de
     feu */
  SDL_Rect rootRect, fireRect;
  /* Surface temporaire pour charger l'image */
  SDL_Surface * tmp;

  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    printf("Erreur lors de l'initialisation de SDL!\n");
    return 1;
  }

  screen = SDL_SetVideoMode(320, 240, 24,
                            SDL_SWSURFACE);

  /* Le foyer est appliqu� aux deux derni�res lignes */
  rootRect.x = 0;
  rootRect.y = screen->h - 2;
  rootRect.w = screen->w;
  rootRect.h = 2;

  /* L'effet de feu s'applique sur tout l'�cran, � l'exception
     des deux derni�res */
  fireRect.x = 0;
  fireRect.y = 0;
  fireRect.w = screen->w;
  fireRect.h = screen->h - 2;

  /* Cr�ation de l'image du logo SDL */
  tmp = SDL_LoadBMP("image.bmp");
  image = SDL_DisplayFormat(tmp);
  SDL_FreeSurface(tmp);
  SDL_SetColorKey(image, SDL_SRCCOLORKEY, SDL_MapRGB(image->format, 0xff, 0x00, 0xff));

  /* Boucle principale */
  while (!letsexit) {
    /* Mise � jour des entr�es */
    input_update();

    /* Mise � jour des param�tre du foyer */
    if ((intensity + intensity_delta) < 0) intensity = 0;
    else if ((intensity + intensity_delta) > 255) intensity = 255;
    else intensity += intensity_delta;
    if (intensity_delta != 0) printf("Intensit�: %d\n", intensity);

    if ((vivacity + vivacity_delta) < 0) vivacity = 0;
    else if ((vivacity + vivacity_delta) > 255) vivacity = 255;
    else vivacity += vivacity_delta;
    if (vivacity_delta != 0) printf("Vivacit�: %d\n", vivacity);

    if ((resistance + resistance_delta) < 0) resistance = 0;
    else if ((resistance + resistance_delta) > 255) resistance = 255;
    else resistance += resistance_delta;
    if (resistance_delta != 0) printf("R�sistance: %d\n", resistance);

    /* Ne pas oublier de verrouiller la surface que l'on va directement
       modifier! */
    SDL_LockSurface(screen);
    fireRoot(screen, &rootRect, intensity, vivacity, resistance);
    fire(screen, &fireRect);
    SDL_UnlockSurface(screen);

    /* Mise � jour de l'�cran */
    SDL_Flip(screen);
  }

  SDL_Quit();
  return 0;
}