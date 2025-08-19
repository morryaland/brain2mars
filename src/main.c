#include "render.h"
#include "imgui.h"
#include "input.h"
#include "game.h"

int main(int argc, char **argv)
{
  Uint64 nowf, lastf = 0;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  setup_window();
  setup_render();
  SDL_ShowWindow(g_window);
  init_cimgui();
  init_world();
  for (;;) {
    nowf = SDL_GetTicks();
    Uint64 deltaf = nowf - lastf;
    process_input();
    if (g_game_ctx.overdrive < 0) {
      //b2World_Step(g_game_ctx.world_id, 1.0f/10.0f, 2);
    }
    else if (deltaf > 1000/60.0f && g_game_ctx.simulate) {
      b2World_Step(g_game_ctx.world_id, 1.0f/60.0f, 4);
    }
    if (deltaf > 1000/60.0f) {
      lastf = nowf;
      ImGui_ImplSDLRenderer3_NewFrame();
      ImGui_ImplSDL3_NewFrame();
      igNewFrame();
      ig_main_window();
      igRender();
      SDL_RenderClear(g_renderer);
      if (g_game_ctx.overdrive >= 0) {
        b2World_Draw(g_game_ctx.world_id, &g_game_ctx.dd);
      }
      ImGui_ImplSDLRenderer3_RenderDrawData(igGetDrawData(), g_renderer);
      SDL_RenderPresent(g_renderer);
    }
  }
  return 0;
}
