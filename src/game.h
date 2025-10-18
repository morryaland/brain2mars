#ifndef __GAME_H__
#define __GAME_H__

#include <box2d/box2d.h>
#include <msvg.h>
#include "mlp.h"

#define PX_X_UNIT 5
#define VICTOR_MASK 2
#define FINISH_MASK 4
#define RAY_DIST 100

typedef struct paths_s {
  MsvgElement *path;
  struct paths_s *npath;
} paths_t;

typedef struct map_s {
  paths_t *svg_paths;
  b2Vec2 start;
  b2Rot rotation;
  b2WorldId world_id;
  b2ShapeId finish_id;
  b2BodyId walls_id;
  b2ChainId internal_wall_id;
  bool loaded;
} map_t;

typedef struct victor_data_s {
  b2RayResult *rays;
  layer_t *layers;
  float torque; // [-1 1]
  float acceleration; // [0 1]
  float stun;
  float score;
  bool cheater;
} victor_data_t;

typedef struct world_data_s {
  b2BodyId *victors;
  float *score;
  float max_score;
  map_t map;
  float cdeath_timer;
  float death_timer;
  float game_timer;
  float mutation;
  uint64_t generation;
  int overdrive;
  int victor_ray_c;
  int neuron_c;
  int hlayer_c;
  int victor_c;
  bool simulate;
  bool pause;
} world_data_t;

int load_map(char path[], void *udata);

int load_checkpoint(char path[], void *udata);

void unload_map(map_t *map);

b2ShapeId create_finish_line(b2WorldId world_id, b2Segment line);

b2ChainId create_wall(b2BodyId body_id, b2Vec2 *points, int count, bool closed);

void start_simulation(b2WorldId world_id);

void stop_simulation(b2WorldId world_id);

void pause_simulation(b2WorldId world_id);

void reset_victor(map_t *map, b2BodyId victor);

b2BodyId *create_victors(map_t *map);

void destroy_victors(b2BodyId *victors, int victor_c);

void after_step(b2WorldId world_id, float time_step);

void next_generation(b2WorldId world_id, float sum_score);

float findlscore(b2WorldId world_id);

void ray_cast(int ray_c, b2WorldId world_id, b2BodyId victor_id);

void apply_force(b2BodyId victor_id);

float get_distance(map_t *map, b2BodyId victor);

bool PreSolveCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal, void* context);

#endif
