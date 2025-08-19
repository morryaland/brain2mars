#ifndef __GAME_H__
#define __GAME_H__

#include <box2d/box2d.h>
#include <msvg.h>

#define PX_X_UNIT 5

typedef struct paths_s {
  MsvgElement *path;
  struct paths_s *npath;
} paths_t;

typedef struct game_ctx_s {
  float death_timer;
  float mutation;
  int overdrive;
  int generation;
  b2WorldId world_id;
  b2BodyId walls_id;
  b2BodyId finish_id;
  b2DebugDraw dd;
  bool simulate;
} game_ctx_t;

extern paths_t *g_svg_paths;
extern game_ctx_t g_game_ctx;

int load_map(char path[]);

void free_svg_paths(paths_t *path);

void unload_map();

void init_world();

#endif
