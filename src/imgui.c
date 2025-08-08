#ifdef __unix__
#include <unistd.h>
#elifdef __WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <limits.h>
#include "render.h"
#include "imgui.h"
#include "game.h"

#define LOAD_DIALOG_ID 1

const char *XML_ErrorString(int code);

void ig_file_dialog()
{
  if (!igBeginPopupEx(LOAD_DIALOG_ID, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
    return;
  }
  static int err = 0;
  static char path[PATH_MAX] = {};
  igInputText("Path", path, PATH_MAX, ImGuiInputTextFlags_None, NULL, NULL);
  if ((igButton("Load", (ImVec2){0, 0}) || igIsKeyDown_Nil(ImGuiKey_Enter)) && *path != '\0') {
    err = load_map(path);
    if (!err)
      igCloseCurrentPopup();
  }
  igSameLine(0, 10);
  if (igButton("Current path", (ImVec2){0, 0})) {
#ifdef __unix__
    getcwd(path, PATH_MAX);
#elifdef __WIN32
    GetCurrentDirectory(PATH_MAX, path); 
#endif
  }
  if (err)
    igText("Error code: %d msg: %s", err, XML_ErrorString(err));
  igEndPopup();
}

void init_cimgui()
{
  igCreateContext(NULL);
  ImGuiIO *ioptr = igGetIO_Nil();
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ioptr->IniFilename = NULL;
  ImGui_ImplSDL3_InitForSDLRenderer(g_window, g_renderer);
  ImGui_ImplSDLRenderer3_Init(g_renderer);
  igStyleColorsDark(NULL);
  ImFontAtlas_AddFontDefault(ioptr->Fonts, NULL);
}

void ig_main_window()
{
  static bool open = true;
  if (!igBegin("main winodw", &open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar))
    return;
  igBeginMenuBar();
  if (igBeginMenu("file", true)) {
    if (igMenuItem_Bool("load map", NULL, false, true))
      igOpenPopup_ID(LOAD_DIALOG_ID, ImGuiPopupFlags_None);
    igEndMenu();
  }
  if (igBeginMenu("help", true)) {
    igEndMenu();
  }
  igEndMenuBar();
  ig_file_dialog();
  igEnd();
}
