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
  ioptr->ConfigDpiScaleFonts = true;
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
  igStyleColorsDark(NULL);
  ImFontAtlas_AddFontDefault(ioptr->Fonts, NULL);
}

void ig_main_window(b2WorldId world_id)
{
  world_data_t *wd = b2World_GetUserData(world_id);
  static int overdrive_start;
  if (!igBegin("main winodw", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
    return;
  igBeginMenuBar();
  if (igBeginMenu("Brain", true)) {
    if (igMenuItem_Bool("Save best", NULL, false, false)) {
    }
    if (igBeginMenu("Import", false)) {
      ig_load_menu(load_checkpoint, NULL);
      igEndMenu();
    }
    igEndMenu();
  }
  if (igBeginMenu("Map", true)) {
    if (igMenuItem_Bool("Unload", NULL, false, !wd->simulate))
      unload_map(&wd->map);
    if (igBeginMenu("Import", true)) {
      ig_load_menu(load_map, &wd->map);
      igEndMenu();
    }
    igEndMenu();
  }
  if (igBeginMenu("About", true)) {
    igTextLinkOpenURL("Github", "https://github.com/morryaland/brain2mars");
    igEndMenu();
  }
  igEndMenuBar();
  if (wd->map.loaded) {
  if (wd->simulate) {
    igText("Generation: %ld", wd->generation);
    igText("Game timer: %.2f", wd->game_timer);
    igPlotLines_FloatPtr("Score", wd->score, 200, 0, NULL, 0, wd->max_score, (ImVec2){0,  80.0f}, sizeof(float));
  }
  igSliderFloat("Death timer", &wd->cdeath_timer, 0, 60.0f, "%.1f", ImGuiSliderFlags_None);
  igSliderFloat("Mutation chance", &wd->mutation, 0.001, 1.0f, "%.3f", ImGuiSliderFlags_None);
  igSeparator();
  igBeginDisabled(wd->simulate);
  igDragInt("Victor count", &wd->victor_c, 0.5f, 1, 10000, "%d", ImGuiSliderFlags_None);
  igDragInt("Parent count", &wd->parent_c, 0.5f, 2, 10, "%d", ImGuiSliderFlags_None);
  igSliderInt("Inputs", &wd->victor_ray_c, 4, 16, "%d", ImGuiSliderFlags_None);
  igSliderInt("Neurons", &wd->neuron_c, 3, 40, "%d", ImGuiSliderFlags_None);
  igSliderInt("Layers", &wd->hlayer_c, 1, 10, "%d", ImGuiSliderFlags_None);
  igEndDisabled();
  if (wd->simulate) {
    if (igButton("Pause", (ImVec2){0, 0})) {
      pause_simulation(world_id);
    }
    if (igButton("Stop", (ImVec2){0, 0})) {
      stop_simulation(world_id);
    }
    if (!wd->pause) {
    if (wd->overdrive >= 0) {
      igSeparator();
      igDragInt("Generations", &wd->overdrive, 0.05f, 1, INT_MAX, "%d", ImGuiSliderFlags_None);
      if (igButton("Overdrive", (ImVec2){0, 0})) {
        wd->overdrive = -wd->overdrive;
        overdrive_start = wd->overdrive;
        igOpenPopup_Str("Overdrive", ImGuiPopupFlags_None);
      }
    }
    else {
      igBeginPopupModal("Overdrive", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
      char buff[32];
      sprintf(buff, "%d/%d", wd->overdrive - overdrive_start, -overdrive_start);
      igProgressBar((float)(overdrive_start - wd->overdrive) / overdrive_start, (ImVec2){0, 0}, buff);
      igSameLine(0, 10);
      if (igButton("Stop", (ImVec2){0, 0}))
        wd->overdrive = 0;
      igEndPopup();
    }
    }
  } else if (igButton("Simulate", (ImVec2){0, 0})) {
    start_simulation(world_id);
  }
  }
  igEnd();
}
