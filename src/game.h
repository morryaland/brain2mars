#ifndef __GAME_H__
#define __GAME_H__

#include <box2d/box2d.h>
#include <msvg.h>

#define PX_X_UNIT 2

typedef struct paths_s {
  MsvgElement *path;
  struct paths_s *npath;
} paths_t;

extern paths_t *g_svg_paths;
extern b2WorldId g_world_id;

int load_map(char path[]);

void free_svg_paths(paths_t *path);

void unload_map();

void init_world();

#endif
