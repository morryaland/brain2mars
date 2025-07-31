#include "sdl.h"
#include "sdlcimgui.h"
#include "input.h"

int main(int argc, char **argv)
{
  init_sdl_gpu();
  init_cimgui_gpu();
  for (;;) {
    process_input();      
    //SDL_RenderClear(g_renderer);
    //SDL_RenderPresent(g_renderer);
  }
  return 0;
}
