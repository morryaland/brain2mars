#ifndef __SDLCIMGUI_H__
#define __SDLCIMGUI_H__

#define CIMGUI_USE_SDL3 1
#define CIMGUI_USE_SDLGPU3 1
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 1
#include <SDL3/SDL_gpu.h>
#include <cimgui.h>
#include <cimgui_impl.h>

void init_cimgui_gpu();

#endif
