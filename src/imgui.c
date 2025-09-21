#ifdef __unix__
#include <unistd.h>
#elif define(__WIN32)
#include <windows.h>
#endif
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include "render.h"
#include "imgui.h"
#include "game.h"

const char *XML_ErrorString(int code);

void ig_load_menu(int (*load_callback)(char *path, void *udata), void *udata)
{
  static int err = 0;
  static int lerrno = 0;
  static char path[512] = "";
  igInputText("Path", path, 512, ImGuiInputTextFlags_None, NULL, NULL);
  if ((igButton("Load", (ImVec2){0, 0}) || igIsKeyDown_Nil(ImGuiKey_Enter)) && *path != '\0') {
    err = load_callback(path, udata);
    lerrno = errno;
    if (!err)
      igCloseCurrentPopup();
  }
  igSameLine(0, 10);
  if (igButton("Current path", (ImVec2){0, 0})) {
#ifdef __unix__
    getcwd(path, 512);
#elif define(__WIN32)
    GetCurrentDirectory(512, path); 
#endif
  }
  if (err)
    igText("Error code: %d msg: %s", err, err > 0 ? XML_ErrorString(err) : strerror(lerrno));
}

void init_cimgui(SDL_Window *window, SDL_Renderer *renderer)
{
  igCreateContext(NULL);
  ImGuiIO *ioptr = igGetIO_Nil();
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ioptr->IniFilename = NULL;
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
  igStyleColorsDark(NULL);
  ImFontAtlas_AddFontDefault(ioptr->Fonts, NULL);
}

void ig_main_window(b2WorldId world_id)
{
  world_data_t *world_data = b2World_GetUserData(world_id);
  static int overdrive_start;
  if (!igBegin("main winodw", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
    return;
  igBeginMenuBar();
  if (igBeginMenu("Checkpoint", true)) {
    if (igMenuItem_Bool("Save best", NULL, false, false)) {
    }
    if (igBeginMenu("Import", false)) {
      ig_load_menu(load_checkpoint, NULL);
      igEndMenu();
    }
    igEndMenu();
  }
  if (igBeginMenu("Map", true)) {
    if (igMenuItem_Bool("Unload", NULL, false, !world_data->simulate))
      unload_map(&world_data->map);
    if (igBeginMenu("Import", true)) {
      ig_load_menu(load_map, &world_data->map);
      igEndMenu();
    }
    igEndMenu();
  }
  if (igBeginMenu("About", true)) {
    igTextLinkOpenURL("Github", "https://github.com/morryaland/brain2mars");
    igEndMenu();
  }
  igEndMenuBar();
  if (world_data->map.loaded) {
  if (world_data->simulate) {
    igText("Generation: %ld", world_data->generation);
    igText("Game timer: %.2f", world_data->game_timer);
  }
  igSliderFloat("Death timer", &world_data->death_timer, 0, 60.0f, "%.1f", ImGuiSliderFlags_None);
  igSliderFloat("Mutation rate", &world_data->mutation, 0.001, 1.0f, "%.3f", ImGuiSliderFlags_None);
  igSeparator();
  igBeginDisabled(world_data->simulate);
  igDragInt("Victor count", &world_data->victor_c, 1.0f, 1, 1000000, "%d", ImGuiSliderFlags_None);
  igBeginDisabled(true);
  igSliderInt("Inputs", &world_data->victor_ray_c, 4, 8, "%d", ImGuiSliderFlags_None);
  igEndDisabled();
  igSliderInt("Neurons", &world_data->neuron_c, 0, 40, "%d", ImGuiSliderFlags_None);
  igSliderInt("Layers", &world_data->hlayer_c, 0, 10, "%d", ImGuiSliderFlags_None);
  igEndDisabled();
  if (world_data->simulate) {
    if (igButton("Pause", (ImVec2){0, 0})) {
      pause_simulation(world_id);
    }
    if (igButton("Stop", (ImVec2){0, 0})) {
      stop_simulation(world_id);
    }
    if (!world_data->pause) {
    if (world_data->overdrive >= 0) {
      igSeparator();
      igDragInt("Generations", &world_data->overdrive, 0.05f, 1, INT_MAX, "%d", ImGuiSliderFlags_None);
      if (igButton("Overdrive", (ImVec2){0, 0})) {
        world_data->overdrive = -world_data->overdrive;
        overdrive_start = world_data->overdrive;
        igOpenPopup_Str("Overdrive", ImGuiPopupFlags_None);
      }
    }
    else {
      igBeginPopupModal("Overdrive", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
      char buff[32];
      sprintf(buff, "%d/%d", world_data->overdrive - overdrive_start, -overdrive_start);
      igProgressBar((float)(overdrive_start - world_data->overdrive) / overdrive_start, (ImVec2){0, 0}, buff);
      igSameLine(0, 10);
      if (igButton("Stop", (ImVec2){0, 0}))
        world_data->overdrive = 0;
      igEndPopup();
    }
    }
  } else if (igButton("Simulate", (ImVec2){0, 0})) {
    start_simulation(world_id);
  }
  }
  igEnd();
}
