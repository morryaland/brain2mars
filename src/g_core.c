#include "game.h"
#include "render.h"

game_ctx_t g_game_ctx;

void init_world()
{
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.enableSleep = false;
  world_def.gravity = (b2Vec2){0, 0};
  g_game_ctx.world_id = b2CreateWorld(&world_def);
  g_game_ctx.dd = b2DefaultDebugDraw();
  g_game_ctx.dd.DrawSegmentFcn = draw_segment;
  g_game_ctx.dd.DrawSolidPolygonFcn = draw_solid_polygon;
}
