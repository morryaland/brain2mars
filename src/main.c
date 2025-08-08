#include "render.h"
#include "imgui.h"
#include "input.h"
#include "game.h"

int main(int argc, char **argv)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  setup_window();
  setup_render();
  SDL_ShowWindow(g_window);
  init_cimgui();
  init_world();
  b2DebugDraw dd = b2DefaultDebugDraw();
  dd.DrawSegmentFcn = draw_segment;
  for (;;) {
    process_input();      
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    igNewFrame();
    ig_main_window();
    igRender();
    SDL_RenderClear(g_renderer);
    b2World_Draw(g_world_id, &dd);
    ImGui_ImplSDLRenderer3_RenderDrawData(igGetDrawData(), g_renderer);
    SDL_RenderPresent(g_renderer);
  }
  return 0;
}
