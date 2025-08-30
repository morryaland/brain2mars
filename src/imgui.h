#ifndef __I_CORE_H__
#define __I_CORE_H__

#include <SDL3/SDL.h>
#include <cimgui.h>
#include <cimgui_impl.h>
#include <box2d/box2d.h>

void init_cimgui(SDL_Window *window, SDL_Renderer *renderer);

void ig_main_window(b2WorldId word_id);

#endif
