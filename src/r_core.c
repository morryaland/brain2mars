#include <SDL3/SDL_render.h>
#include "render.h"
#include "game.h"

SDL_Renderer *g_renderer = NULL;
cam_t g_cam = {10, 0, 0};

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

static void projection(b2Vec2 *a)
{
  a->x = (a->x + g_cam.x) * g_cam.f;
  a->y = (a->y + g_cam.y) * g_cam.f;
}

void draw_segment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context)
{
  RGBA *sdlcolor = (RGBA*)&color;
  SDL_SetRenderDrawColor(g_renderer, sdlcolor->R, sdlcolor->G, sdlcolor->B, sdlcolor->A); 
  projection(&p1);
  projection(&p2);
  SDL_RenderLine(g_renderer, p1.x, p1.y, p2.x, p2.y);
  SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255); 
}
