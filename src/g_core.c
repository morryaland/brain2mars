#include "game.h"

b2WorldId g_world_id;

void init_world()
{
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.enableSleep = false;
  g_world_id = b2CreateWorld(&world_def);
}
