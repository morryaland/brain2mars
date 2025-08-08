#include <SDL3/SDL_video.h>
#include <SDL3/SDL_hints.h>
#include "render.h"

SDL_Window *g_window = NULL;

void setup_window()
{
  SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
  const SDL_DisplayMode *dm = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
  SDL_WindowFlags f = SDL_WINDOW_FULLSCREEN;
  g_window = SDL_CreateWindow("data wing", dm->w ?: DEFAULT_WINDOW_WIDTH, dm->h ?: DEFAULT_WINDOW_HEIGHT, f);
}
