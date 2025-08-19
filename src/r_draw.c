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

void draw_solid_polygon(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius, b2HexColor color, void *context)
{
  b2Vec2 pv[vertexCount + 1] = {};
  for (int i = 0; i < vertexCount; i++) {
    pv[i] = projection(b2Add(b2RotateVector(transform.q, vertices[i]), transform.p));
  }
  pv[vertexCount] = pv[0];
  set_draw_color(color);
  SDL_RenderLines(g_renderer, (SDL_FPoint*)pv, vertexCount + 1);
  SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255); 
}
