#ifndef __RENDER_H__
#define __RENDER_H__

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <box2d/box2d.h>

#define DEFAULT_WINDOW_WIDTH  800
#define DEFAULT_WINDOW_HEIGHT 600

typedef struct cam_s {
  float f;
  float x;
  float y;
} cam_t;

extern cam_t g_cam;

void draw_segment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context);

void draw_solid_polygon(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius, b2HexColor color, void *context);

#endif
