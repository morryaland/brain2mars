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
   shapedef.filter.categoryBits = FINISH_MASK;
   b2CreateSegmentShape(finish_body_id, &shapedef, &finish);
   return finish_body_id;
}

void create_walls(b2BodyId body_id, b2Vec2 *points, int count)
{
  b2ChainDef chain_def = b2DefaultChainDef();
  chain_def.count = count;
  chain_def.points = points;
  chain_def.isLoop = true;
  chain_def.materialCount = 1;
  b2SurfaceMaterial mat = b2DefaultSurfaceMaterial();
  mat.customColor = b2_colorWhite;
  chain_def.materials = (b2SurfaceMaterial[]){mat};
  b2CreateChain(body_id, &chain_def);
}

b2BodyId *create_victors(map_t *map)
{
  world_data_t *world_data = b2World_GetUserData(map->world_id);
  b2Hull victor_hull = b2ComputeHull((b2Vec2[]){{-0.5f, -0.75f}, {0.5f, -0.75f}, {0, 0.75f}}, 3);
  b2Polygon victor_polygon = b2MakePolygon(&victor_hull, 0);
  b2BodyDef victor_body_def = b2DefaultBodyDef();
  victor_body_def.type = b2_dynamicBody;
  victor_body_def.position = map->start;
  victor_body_def.rotation = b2MakeRot(-90 * B2_PI / 180);
  victor_body_def.linearDamping = 0.5f;
  victor_body_def.angularDamping = 10.0f;

  b2ShapeDef victor_shape_def = b2DefaultShapeDef();
  victor_shape_def.density = 0.2f;
  victor_shape_def.material = b2DefaultSurfaceMaterial();
  victor_shape_def.filter.categoryBits = VICTOR_MASK;
  victor_shape_def.filter.maskBits = ~VICTOR_MASK;
  victor_shape_def.enableSensorEvents = true;
  victor_shape_def.enablePreSolveEvents = true;
  b2BodyId *victors = malloc(world_data->victor_c * sizeof(b2BodyId));
  for (int i = 0; i < world_data->victor_c; i++) {
    victors[i] = b2CreateBody(map->world_id, &victor_body_def);
    b2CreatePolygonShape(victors[i], &victor_shape_def, &victor_polygon);
    victor_data_t *victor_data = malloc(sizeof(victor_data_t));
    victor_data->rays = malloc(world_data->victor_ray_c * sizeof(b2RayResult));
    victor_data->torque = 0;
    victor_data->acceleration = 0;
    victor_data->stun = 0;
    b2Body_SetUserData(victors[i], victor_data);
  }
  return victors;
}

void destroy_victors(b2BodyId *victors, int victor_c)
{
  if (!victors)
    return;
  for (int i = 0; i < victor_c; i++) {
    victor_data_t *victor_data = b2Body_GetUserData(victors[i]);
    free(victor_data->rays);
    free(victor_data);
    b2DestroyBody(victors[i]);
  }
  free(victors);
}

void ray_cast(int ray_c, b2WorldId world_id, b2BodyId victor_id)
{
  b2Rot vr = b2Body_GetRotation(victor_id);
  b2QueryFilter filter = b2DefaultQueryFilter();
  filter.maskBits = ~VICTOR_MASK & ~FINISH_MASK;
  victor_data_t *victor_data = b2Body_GetUserData(victor_id);
  victor_data->rays[0] = b2World_CastRayClosest(world_id, b2Body_GetWorldPoint(victor_id, (b2Vec2){0, -0.75f}),
      b2RotateVector(vr, (b2Vec2){0, -1.5f}), filter);
  victor_data->rays[1] = b2World_CastRayClosest(world_id, b2Body_GetWorldPoint(victor_id, (b2Vec2){0, 0}),
      b2RotateVector(vr, (b2Vec2){0, RAY_DIST}), filter);
  victor_data->rays[2] = b2World_CastRayClosest(world_id, b2Body_GetWorldPoint(victor_id, (b2Vec2){0, -0.75f}),
      b2RotateVector(vr, (b2Vec2){RAY_DIST, 0}), filter);
  victor_data->rays[3] = b2World_CastRayClosest(world_id, b2Body_GetWorldPoint(victor_id, (b2Vec2){0, -0.75f}),
      b2RotateVector(vr, (b2Vec2){-RAY_DIST, 0}), filter);
}

void apply_force(b2BodyId victor_id)
{
  victor_data_t *vd = b2Body_GetUserData(victor_id);
  float f = (vd->rays[0].hit ? 1/vd->rays[0].fraction : 1.0f) * vd->acceleration;
  b2Vec2 force = b2RotateVector(b2Body_GetRotation(victor_id), (b2Vec2){0, f});
  b2Body_ApplyForceToCenter(victor_id, force, false);
  b2Body_ApplyTorque(victor_id, vd->torque, false);
}

bool PreSolveCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal, void* context)
{
  if (b2Shape_GetType(shapeIdA) == b2_polygonShape && b2Shape_GetType(shapeIdB) == b2_chainSegmentShape) {
    b2Segment seg = b2Shape_GetChainSegment(shapeIdB).segment;
    normal = b2Normalize((b2Vec2){seg.point2.y - seg.point1.y, -(seg.point2.x - seg.point1.x)});
  }
  else if (b2Shape_GetType(shapeIdB) == b2_polygonShape && b2Shape_GetType(shapeIdA) == b2_chainSegmentShape) {
    b2ShapeId tmp = shapeIdA;
    shapeIdA = shapeIdB;
    shapeIdB = tmp;
  }
  else {
    return true;
  }
  b2BodyId victor = b2Shape_GetBody(shapeIdA);
  victor_data_t *vd = b2Body_GetUserData(victor);
  b2Body_SetAngularVelocity(victor, 0);
  float vvl;
  b2Vec2 vvn = b2GetLengthAndNormalize(&vvl, b2Body_GetLinearVelocity(victor));
  vd->stun = b2AbsFloat(b2Dot(vvn, normal));
  float invspeed = SDL_clamp(1/vvl, 0.1f, 0.5f);
  b2Body_ApplyLinearImpulseToCenter(victor, b2MulSV(invspeed, normal), false);
  return true;
}
