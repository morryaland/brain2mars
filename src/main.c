#include "render.h"
#include "imgui.h"
#include "input.h"
#include "game.h"

int main(int argc, char **argv)
{
  Uint64 nowf, lastf = 0;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
  const SDL_DisplayMode *dm = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
  SDL_WindowFlags f = SDL_WINDOW_FULLSCREEN;
  SDL_Window *window = SDL_CreateWindow("data wing", dm->w ?: DEFAULT_WINDOW_WIDTH, dm->h ?: DEFAULT_WINDOW_HEIGHT, f);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  b2DebugDraw dd = b2DefaultDebugDraw();
  dd.context = renderer;
  dd.DrawSegmentFcn = draw_segment;
  dd.DrawSolidPolygonFcn = draw_solid_polygon;
  SDL_ShowWindow(window);
  init_cimgui(window, renderer);
  init_world();
  for (;;) {
    nowf = SDL_GetTicks();
    Uint64 deltaf = nowf - lastf;
    process_input(window);
    if (g_sim.overdrive < 0) {
      //b2World_Step(g_game_ctx.world_id, 1.0f/10.0f, 2);
    }
    else if (deltaf > 1000/60.0f && g_sim.simulate) {
      b2World_Step(g_sim.world_id, 1.0f/60.0f, 4);
    }
    if (deltaf > 1000/60.0f) {
      lastf = nowf;
      ImGui_ImplSDLRenderer3_NewFrame();
      ImGui_ImplSDL3_NewFrame();
      igNewFrame();
      ig_main_window();
      igRender();
      SDL_RenderClear(renderer);
      if (g_sim.overdrive >= 0) {
        b2World_Draw(g_sim.world_id, &dd);
      }
      ImGui_ImplSDLRenderer3_RenderDrawData(igGetDrawData(), renderer);
      SDL_RenderPresent(renderer);
    }
  }
  return 0;
}
