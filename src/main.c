#include "render.h"
#include "imgui.h"
#include "input.h"
#include "game.h"

int main(int argc, char **argv)
{
  Uint64 nowf, lastf = 0;
  const float time_step = 1.0f/60.0f;
  const float time_step_overdrive = 1.0f/60.0f;

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

  world_data_t world_data = {0};
  world_data.mutation = 0.1f;
  world_data.hlayer_c = 2;
  world_data.neuron_c = 8;
  world_data.victor_c = 100;
  world_data.victor_ray_c = 4;

  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.enableSleep = false;
  world_def.gravity = (b2Vec2){0, 0};
  world_def.userData = &world_data;
  b2WorldId world_id = b2CreateWorld(&world_def);
  world_data.map.world_id = world_id;
  b2World_SetPreSolveCallback(world_id, PreSolveCallback, NULL);
  
  init_cimgui(window, renderer);
  SDL_ShowWindow(window);

  for (;;) {
    nowf = SDL_GetTicks();
    Uint64 deltaf = nowf - lastf;
    process_input(window);
    if (world_data.overdrive < 0) {
      b2World_Step(world_id, time_step_overdrive, 2);
    }
    else if (world_data.simulate && !world_data.pause && deltaf > 1000 * time_step) {
      g_cam.target = world_data.victors[0];
      b2World_Step(world_id, time_step, 4);
      get_distance(&world_data.map, world_data.victors[0]);
      for (int i = 0; i < world_data.victor_c; i++) {
        victor_data_t *vd = b2Body_GetUserData(world_data.victors[i]);
        if (vd->stun > 0) {
          vd->stun -= time_step;
        }
        ray_cast(world_data.victor_ray_c, world_id, world_data.victors[i]);
        apply_force(world_data.victors[i]);
      }
    }
    if (deltaf > 1000/60.0f) {
      lastf = nowf;
      ImGui_ImplSDLRenderer3_NewFrame();
      ImGui_ImplSDL3_NewFrame();
      igNewFrame();
      ig_main_window(world_id);
      igRender();
      SDL_RenderClear(renderer);
      if (world_data.overdrive >= 0) {
        b2World_Draw(world_id, &dd);
      }
      ImGui_ImplSDLRenderer3_RenderDrawData(igGetDrawData(), renderer);
      SDL_RenderPresent(renderer);
    }
    if (world_data.overdrive >= 0)
      SDL_Delay(SDL_max(1000/60.0f - deltaf, 0));
  }
  return 0;
}
