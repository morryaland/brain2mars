#include <SDL3/SDL_render.h>
#include "render.h"
#include "game.h"

SDL_Renderer *g_renderer = NULL;
cam_t g_cam = {10, 0, 0};

void setup_render()
{
  g_renderer = SDL_CreateRenderer(g_window, NULL);
}

static b2Vec2 projection(b2Vec2 a)
{
  return (b2Vec2){(a.x + g_cam.x) * g_cam.f, (a.y + g_cam.y) * g_cam.f};
}

static void set_draw_color(b2HexColor color)
{
  unsigned char red = (color >> 16) & 0xFF;
  unsigned char green = (color >> 8) & 0xFF;
  unsigned char blue = color & 0xFF;
  SDL_SetRenderDrawColor(g_renderer, red, green, blue, 255);
}

void draw_segment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context)
{
  p1 = projection(p1);
  p2 = projection(p2);
  set_draw_color(color);
  SDL_RenderLine(g_renderer, p1.x, p1.y, p2.x, p2.y);
  SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255); 
}

void draw_polygon(const b2Vec2 *p, int p_c, b2HexColor color, void *context)
{
  b2Vec2 pp[p_c] = {};
  for (int i = 0; i < p_c; i++) {
    pp[i] = projection(p[i]);
  }
  set_draw_color(color);
  SDL_RenderLines(g_renderer, (SDL_FPoint*)pp, p_c);
  SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255); 
}
