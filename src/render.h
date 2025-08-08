#ifndef __RENDER_H__
#define __RENDER_H__

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <box2d/box2d.h>

#define DEFAULT_WINDOW_WIDTH  800
#define DEFAULT_WINDOW_HEIGHT 600

extern SDL_Window *g_window;
extern SDL_Renderer *g_renderer;

void setup_window();

void setup_render();

void draw_segment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context);

#endif
