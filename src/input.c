#include <stdio.h>
#include <stdlib.h>
#include "sdl.h"
#include "sdlcimgui.h"

void process_input()
{
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    ImGui_ImplSDL3_ProcessEvent(&e);
    switch (e.type) {
      case SDL_EVENT_QUIT:
        SDL_DestroyWindow(g_window);
        exit(0);
        break;
      default:
        break;
    }
  }
}
