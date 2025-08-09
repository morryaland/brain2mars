#include <SDL3/SDL_render.h>
#include "render.h"
#include "game.h"

SDL_Renderer *g_renderer = NULL;

typedef struct _RGBA {
  unsigned int R: 8;
  unsigned int G: 8;
  unsigned int B: 8;
  unsigned int A: 8;
} RGBA;

void setup_render()
{
  g_renderer = SDL_CreateRenderer(g_window, NULL);
}

void draw_segment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context)
{
  RGBA *sdlcolor = (RGBA*)&color;
  SDL_SetRenderDrawColor(g_renderer, sdlcolor->R, sdlcolor->G, sdlcolor->B, sdlcolor->A); 
  SDL_RenderLine(g_renderer, p1.x * 10, p1.y * 10, p2.x * 10, p2.y * 10);
  SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255); 
}
