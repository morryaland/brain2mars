#ifndef __SDL_H__
#define __SDL_H__

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#define DEFAULT_WINDOW_WIDTH  800
#define DEFAULT_WINDOW_HEIGHT 600

extern SDL_Window *g_window;
extern SDL_GPUDevice *g_gpu;

void init_sdl_gpu();

#endif
