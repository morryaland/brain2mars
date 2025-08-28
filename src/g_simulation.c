#include <stdlib.h>
#include "game.h"

simulation_t g_sim = {
  .death_timer = 0,
  .generation = 0,
  .mutation = 0.1,
  .hlayer_c = 2,
  .neuron_c = 8,
  .victor_c = 100,
  .victor_inputs = 3
};

int load_checkpoint(char path[])
{
  return -1;
}

void create_victors()
{
  b2Hull victor_hull = b2ComputeHull((b2Vec2[]){{-1, -1}, {1, -1}, {0, 1.75f}}, 3);
  b2Polygon victor_polygon = b2MakePolygon(&victor_hull, 0);
  b2BodyDef victor_body_def = b2DefaultBodyDef();
  victor_body_def.type = b2_dynamicBody;
  victor_body_def.position = g_map->start;
  victor_body_def.rotation = b2MakeRot(-90 * B2_PI / 180);
  b2ShapeDef victor_shape_def = b2DefaultShapeDef();
  victor_shape_def.material = b2DefaultSurfaceMaterial();
  g_sim.victors = malloc(g_sim.victor_c * sizeof(b2BodyId));
  for (int i = 0; i < g_sim.victor_c; i++) {
    g_sim.victors[i] = b2CreateBody(g_sim.world_id, &victor_body_def);
    b2CreatePolygonShape(g_sim.victors[i], &victor_shape_def, &victor_polygon);
  }
}

void destroy_victors()
{
  if (!g_sim.victors)
    return;
  for (int i = 0; i < g_sim.victor_c; i++) {
    free(b2Body_GetUserData(g_sim.victors[i]));
    b2DestroyBody(g_sim.victors[i]);
  }
  free(g_sim.victors);
}
