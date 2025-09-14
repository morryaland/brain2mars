#include <stdlib.h>
#include "game.h"
#include "render.h"
#include "imgui.h"

void process_input(SDL_Window *window)
{
  float dmx, dmy;
  static bool move = false;
  static float mbx = 0, mby = 0;
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    ImGui_ImplSDL3_ProcessEvent(&e);
    switch (e.type) {
      case SDL_EVENT_QUIT:
        SDL_DestroyWindow(window);
        exit(0);
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (e.button.button == SDL_BUTTON_RIGHT)
          move = true;
        g_cam.target = b2_nullBodyId;
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (e.button.button == SDL_BUTTON_RIGHT) {
          move = false;
          mbx = 0; 
          mby = 0; 
        }
        break;
      case SDL_EVENT_MOUSE_WHEEL:
        float f = g_cam.f;
        float scaleFactor = 1.0f + (0.1f*fabsf(e.wheel.y));
        if (e.wheel.y < 0) scaleFactor = 1.0f/scaleFactor;
        g_cam.f *= scaleFactor;
        if (g_cam.f < 1.0f) g_cam.f = 1;
        else if (g_cam.f > 40.0f) g_cam.f = 40;
        f = (f - g_cam.f) / (f * g_cam.f);
        g_cam.x += e.wheel.mouse_x * f;
        g_cam.y += e.wheel.mouse_y * f;
        break;
      default:
        break;
    }
  }
  if (b2Body_IsValid(g_cam.target)) {
    b2Vec2 body_pos = b2Body_GetPosition(g_cam.target);
    g_cam.f = 20;
    g_cam.x = (g_cam.f*1.77f)-body_pos.x;
    g_cam.y = (g_cam.f)-body_pos.y;
  //
    victor_data_t *vd = b2Body_GetUserData(g_cam.target);
    const bool *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_UP])
      vd->acceleration = 1;
    else
      vd->acceleration = 0;
    if (keys[SDL_SCANCODE_LEFT])
      vd->torque = -1;
    else if (keys[SDL_SCANCODE_RIGHT])
      vd->torque = 1;
    else
      vd->torque = 0;
  //
  }
  if (!move)
    return;
  dmx = mbx;
  dmy = mby;
  SDL_GetMouseState(&mbx, &mby);
  if (dmx)
    dmx -= mbx;
  if (dmy)
    dmy -= mby;
  g_cam.x -= dmx / g_cam.f;
  g_cam.y -= dmy / g_cam.f;
}
