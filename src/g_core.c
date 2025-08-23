#include <stdlib.h>
#include "game.h"
#include "render.h"

game_ctx_t g_game_ctx = {0};

void init_world()
{
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.enableSleep = false;
  world_def.gravity = (b2Vec2){0, 0};
  g_game_ctx.world_id = b2CreateWorld(&world_def);
  g_game_ctx.sim.death_timer = 0;
  g_game_ctx.sim.generation = 0;
  g_game_ctx.sim.mutation = 0.1;
  g_game_ctx.sim.hlayer_c = 2;
  g_game_ctx.sim.neuron_c = 8;
  g_game_ctx.sim.victor_c = 100;
  g_game_ctx.sim.victor_inputs = 3;
}
