#include "render.h"
#include "imgui.h"
#include "input.h"
#include "game.h"

int main(int argc, char **argv)
{
  Uint64 nowf, lastf = 0;
  const float time_step = 1.0f/60;
  const float time_step_overdrive = 1.0f/30;

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

  world_data_t wd = {0};
  wd.mutation = 0.1f;
  wd.hlayer_c = 2;
  wd.neuron_c = 8;
  wd.victor_c = 100;
  wd.victor_ray_c = 4;

  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.enableSleep = false;
  world_def.gravity = (b2Vec2){0, 0};
  world_def.userData = &wd;
  b2WorldId world_id = b2CreateWorld(&world_def);
  wd.map.world_id = world_id;
  b2World_SetPreSolveCallback(world_id, PreSolveCallback, NULL);
  
  init_cimgui(window, renderer);
  SDL_ShowWindow(window);

  for (;;) {
    nowf = SDL_GetTicks();
    Uint64 deltaf = nowf - lastf;
    process_input(window);
    if (wd.overdrive < 0) {
      b2World_Step(world_id, time_step_overdrive, 3);
        after_step(world_id, time_step_overdrive);
    }
    else if (wd.simulate && !wd.pause && deltaf > 1000 * time_step) {
      b2World_Step(world_id, time_step, 4);
        after_step(world_id, time_step);
    }
    if (deltaf > 1000 * time_step) {
      lastf = nowf;
      ImGui_ImplSDLRenderer3_NewFrame();
      ImGui_ImplSDL3_NewFrame();
      igNewFrame();
      ig_main_window(world_id);
      igRender();
      SDL_RenderClear(renderer);
      if (wd.overdrive >= 0) {
        b2World_Draw(world_id, &dd);
      }
      ImGui_ImplSDLRenderer3_RenderDrawData(igGetDrawData(), renderer);
      SDL_RenderPresent(renderer);
    }
    if (wd.overdrive >= 0)
      SDL_Delay(SDL_max(1000 * time_step - deltaf, 0));
  }
  return 0;
}
