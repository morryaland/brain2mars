#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <stdio.h>
#include "sdl.h"

SDL_Window *g_window;
SDL_GPUDevice *g_gpu;

void init_sdl_gpu()
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  const SDL_DisplayMode *dm = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
  if (!dm) {
    puts(SDL_GetError());
  }
  g_window = SDL_CreateWindow("data wing", dm->w ?: DEFAULT_WINDOW_WIDTH, dm->h ?: DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN);
  SDL_ShowWindow(g_window);
  SDL_HideCursor();
  SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
  g_gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB, true, NULL);
  if (!g_gpu) {
    puts(SDL_GetError());
  }
  SDL_ClaimWindowForGPUDevice(g_gpu, g_window);
  SDL_SetGPUSwapchainParameters(g_gpu, g_window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);
}
