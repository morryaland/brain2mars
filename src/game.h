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
  b2Vec2 start;
  b2WorldId world_id;
  b2BodyId finish_id;
  b2BodyId walls_id;
  bool loaded;
} map_t;

typedef struct checkpoint_s {
  uint64_t generation;
  uint32_t victor_inputs;
  uint16_t neuron_c;
  uint16_t hlayer_c;
} checkpoint_header_t;

typedef struct world_data_s {
  map_t map;
  float death_timer;
  float mutation;
  uint64_t generation;
  int overdrive;
  int victor_ray_c;
  int neuron_c;
  int hlayer_c;
  int victor_c;
  bool simulate;
} world_data_t;

int load_map(char path[], void *udata);

int load_checkpoint(char path[], void *udata);

void unload_map(map_t *map);

b2BodyId create_finish_line(b2WorldId world_id, b2Vec2 p1, b2Vec2 p2);

b2BodyId *create_victors(map_t *map);

void destroy_victors(b2BodyId *victors, int victor_c);

#endif
