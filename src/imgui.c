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

void ig_load_menu(int (*load_callback)(char *path))
{
  static int err = 0;
  static int lerrno = 0;
  static char path[512] = "";
  igInputText("Path", path, 512, ImGuiInputTextFlags_None, NULL, NULL);
  if ((igButton("Load", (ImVec2){0, 0}) || igIsKeyDown_Nil(ImGuiKey_Enter)) && *path != '\0') {
    err = load_callback(path);
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

void ig_main_window()
{
  static bool open = true;
  static int overdrive_start;
  if (!igBegin("main winodw", &open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
    return;
  igBeginMenuBar();
  if (igBeginMenu("Checkpoint", true)) {
    if (igMenuItem_Bool("Save best", NULL, false, false)) {
    }
    if (igBeginMenu("Import", true)) {
      ig_load_menu(load_checkpoint);
      igEndMenu();
    }
    igEndMenu();
  }
  if (igBeginMenu("Map", true)) {
    if (igMenuItem_Bool("Unload", NULL, false, true))
      unload_map();
    if (igBeginMenu("Import", true)) {
      ig_load_menu(load_map);
      igEndMenu();
    }
    igEndMenu();
  }
  if (igBeginMenu("About", true)) {
    igTextLinkOpenURL("Github", "https://github.com/morryaland/brain2mars");
    igEndMenu();
  }
  igEndMenuBar();
  if (g_game_ctx.curr_map) {
  if (g_game_ctx.simulate) {
    igText("Generation: %ld", g_game_ctx.sim.generation);
  }
  igSliderFloat("Death timer", &g_game_ctx.sim.death_timer, 0, 60.0f, "%.1f", ImGuiSliderFlags_None);
  igSliderFloat("Mutation rate", &g_game_ctx.sim.mutation, 0.001, 1.0f, "%.3f", ImGuiSliderFlags_None);
  igSeparator();
  igBeginDisabled(g_game_ctx.simulate);
  igDragInt("Victor count", &g_game_ctx.sim.victor_c, 1.0f, 1, 1000000, "%d", ImGuiSliderFlags_None);
  igSliderInt("Inputs", &g_game_ctx.sim.victor_inputs, 1, 9, "%d", ImGuiSliderFlags_None);
  igSliderInt("Neurons", &g_game_ctx.sim.neuron_c, 0, 40, "%d", ImGuiSliderFlags_None);
  igSliderInt("Layers", &g_game_ctx.sim.hlayer_c, 0, 10, "%d", ImGuiSliderFlags_None);
  if (igButton("Simulate", (ImVec2){0, 0})) {
    g_game_ctx.simulate = true;
  }
  igEndDisabled();
  if (g_game_ctx.simulate) {
    if (igButton("Stop", (ImVec2){0, 0})) {
      g_game_ctx.simulate = false;
    }
    if (g_game_ctx.overdrive >= 0) {
      igSeparator();
      igDragInt("Generations", &g_game_ctx.overdrive, 0.05f, 1, INT_MAX, "%d", ImGuiSliderFlags_None);
      if (igButton("Overdrive", (ImVec2){0, 0})) {
        g_game_ctx.overdrive = -g_game_ctx.overdrive;
        overdrive_start = g_game_ctx.overdrive;
        igOpenPopup_Str("Overdrive", ImGuiPopupFlags_None);
      }
    }
    else {
      igBeginPopupModal("Overdrive", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
      char buff[32];
      sprintf(buff, "%d/%d", g_game_ctx.overdrive - overdrive_start, -overdrive_start);
      igProgressBar((float)(overdrive_start - g_game_ctx.overdrive) / overdrive_start, (ImVec2){0, 0}, buff);
      igSameLine(0, 10);
      if (igButton("Stop", (ImVec2){0, 0}))
        g_game_ctx.overdrive = 0;
      igEndPopup();
    }
  }
  }
  igEnd();
}
