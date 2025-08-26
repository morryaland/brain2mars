#include <stdlib.h>
#include "game.h"
#include "render.h"

void init_world()
{
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.enableSleep = false;
  world_def.gravity = (b2Vec2){0, 0};
  g_sim.world_id = b2CreateWorld(&world_def);
}
