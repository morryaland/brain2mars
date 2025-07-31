#include "sdl.h"
#include "sdlcimgui.h"

void init_cimgui_gpu()
{
  igCreateContext(NULL);
  ImGuiIO *ioptr = igGetIO_Nil();
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
#ifdef IMGUI_HAS_DOCK
  ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
  igStyleColorsDark(NULL);
  ImGui_ImplSDL3_InitForSDLGPU(g_window);
  ImGui_ImplSDLGPU3_InitInfo info = {
    .Device = g_gpu,
    .ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(g_gpu, g_window),
    .MSAASamples = SDL_GPU_SAMPLECOUNT_1
  };
  ImGui_ImplSDLGPU3_Init(&info);
  ImFontAtlas_AddFontDefault(ioptr->Fonts, NULL);
}
