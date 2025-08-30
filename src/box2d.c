#include <stdlib.h>
#include "game.h"
#include "render.h"

b2BodyId create_finish_line(b2WorldId world_id, b2Vec2 p1, b2Vec2 p2)
{
   b2Segment finish = {p1, p2};
   b2BodyDef bodydef = b2DefaultBodyDef();
   bodydef.type = b2_staticBody;
   b2BodyId finish_body_id = b2CreateBody(world_id, &bodydef);
   b2ShapeDef shapedef = b2DefaultShapeDef();
   b2SurfaceMaterial surmat = b2DefaultSurfaceMaterial();
   surmat.customColor = b2_colorYellow;
   shapedef.material = surmat;
   shapedef.isSensor = true;
   shapedef.enableSensorEvents = true;
   b2CreateSegmentShape(finish_body_id, &shapedef, &finish);
   return finish_body_id;
}

b2BodyId *create_victors(map_t *map)
{
  world_data_t *world_data = b2World_GetUserData(map->world_id);
  b2Hull victor_hull = b2ComputeHull((b2Vec2[]){{-1, -1}, {1, -1}, {0, 1.75f}}, 3);
  b2Polygon victor_polygon = b2MakePolygon(&victor_hull, 0);
  b2BodyDef victor_body_def = b2DefaultBodyDef();
  victor_body_def.type = b2_dynamicBody;
  victor_body_def.position = map->start;
  victor_body_def.rotation = b2MakeRot(-90 * B2_PI / 180);
  victor_body_def.linearDamping = 0.5f;
  b2ShapeDef victor_shape_def = b2DefaultShapeDef();
  victor_shape_def.material = b2DefaultSurfaceMaterial();
  victor_shape_def.filter.groupIndex = -1;
  b2BodyId *victors = malloc(world_data->victor_c * sizeof(b2BodyId));
  for (int i = 0; i < world_data->victor_c; i++) {
    victors[i] = b2CreateBody(map->world_id, &victor_body_def);
    b2CreatePolygonShape(victors[i], &victor_shape_def, &victor_polygon);
  }
  return victors;
}

void destroy_victors(b2BodyId *victors, int victor_c)
{
  if (!victors)
    return;
  for (int i = 0; i < victor_c; i++) {
    free(b2Body_GetUserData(victors[i]));
    b2DestroyBody(victors[i]);
  }
  free(victors);
}
