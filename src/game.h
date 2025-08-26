#ifndef __GAME_H__
#define __GAME_H__

#include <box2d/box2d.h>
#include <msvg.h>

#define PX_X_UNIT 5

typedef struct paths_s {
  MsvgElement *path;
  struct paths_s *npath;
} paths_t;

typedef struct map_s {
  paths_t *svg_paths;
  b2BodyId walls_id;
  b2BodyId finish_id;
  b2Vec2 start;
} map_t;

typedef struct checkpoint_s {
  uint64_t generation;
  uint32_t victor_inputs;
  uint16_t neuron_c;
  uint16_t hlayer_c;
} checkpoint_header_t;

typedef struct simulation_s {
  float death_timer;
  float mutation;
  uint64_t generation;
  b2WorldId world_id;
  int victor_inputs;
  int overdrive;
  int neuron_c;
  int hlayer_c;
  int victor_c;
  bool simulate;
  b2BodyId *victors;
} simulation_t;

extern map_t *g_map;
extern simulation_t g_sim;

int load_map(char path[]);

int load_checkpoint(char path[]);

void free_svg_paths(paths_t *path);

void unload_map();

void init_world();

void create_victors();

void destroy_victors();

#endif
