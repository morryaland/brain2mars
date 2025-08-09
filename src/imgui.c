#ifdef __unix__
#include <unistd.h>
#elifdef __WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include "render.h"
#include "imgui.h"
#include "game.h"

const char *XML_ErrorString(int code);

void ig_import_menu()
{
  static int err = 0;
  static char path[PATH_MAX] = {};
  igInputText("Path", path, PATH_MAX, ImGuiInputTextFlags_None, NULL, NULL);
  if ((igButton("Load", (ImVec2){0, 0}) || igIsKeyDown_Nil(ImGuiKey_Enter)) && *path != '\0') {
    if (g_svg_paths)
      unload_current_map();
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
    igText("Error code: %d msg: %s", err, err > 0 ? XML_ErrorString(err) : strerror(errno));
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
  if (igBeginMenu("File", true)) {
    if (igBeginMenu("Import map", true)) {
      ig_import_menu();
      igEndMenu();
    }
    igEndMenu();
  }
  if (igBeginMenu("Help", true)) {
    igEndMenu();
  }
  igEndMenuBar();
  igEnd();
}
