#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "game.h"
#include "ga.h"
#include "render.h"

static layer_t *parent_brains[2] = {};

b2ShapeId create_finish_line(b2WorldId world_id, b2Segment line)
{
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
  return b2CreateSegmentShape(finish_body_id, &shapedef, &line);
}

b2ChainId create_wall(b2BodyId body_id, b2Vec2 *points, int count, bool closed)
{
  b2ChainDef chain_def = b2DefaultChainDef();
  chain_def.count = count;
  chain_def.points = points;
  chain_def.isLoop = closed;
  chain_def.materialCount = 1;
  b2SurfaceMaterial mat = b2DefaultSurfaceMaterial();
  mat.customColor = b2_colorWhite;
  chain_def.materials = (b2SurfaceMaterial[]){mat};
  return b2CreateChain(body_id, &chain_def);
}

void start_simulation(b2WorldId world_id)
{
  world_data_t *wd = b2World_GetUserData(world_id);
  wd->victors = create_victors(&wd->map);
  parent_brains[0] = create_mlp(wd->hlayer_c, wd->neuron_c, wd->victor_ray_c, 2);
  parent_brains[1] = create_mlp(wd->hlayer_c, wd->neuron_c, wd->victor_ray_c, 2);
  wd->death_timer = wd->cdeath_timer;
  wd->simulate = true;
}

void stop_simulation(b2WorldId world_id)
{
  world_data_t *wd = b2World_GetUserData(world_id);
  destroy_mlp(parent_brains[0], wd->hlayer_c + 1);
  destroy_mlp(parent_brains[1], wd->hlayer_c + 1);
  destroy_victors(wd->victors, wd->victor_c);
  wd->simulate = false;
  wd->pause = false;
  wd->game_timer = 0;
  wd->generation = 0;
  free(wd->score);
  wd->score = NULL;
  wd->max_score = 0;
}

void pause_simulation(b2WorldId world_id)
{
  world_data_t *wd = b2World_GetUserData(world_id);
  wd->death_timer = wd->cdeath_timer;
  wd->pause = !wd->pause;
}

void reset_victor(map_t *map, b2BodyId victor)
{
  victor_data_t *vd = b2Body_GetUserData(victor);
  vd->torque = 0;
  vd->acceleration = 0;
  vd->stun = 0;
  vd->score = 0;
  vd->cheater = true;
  b2Body_SetTransform(victor, map->start, map->rotation);
  b2Body_SetLinearVelocity(victor, b2Vec2_zero);
  b2Body_SetAngularVelocity(victor, 0);
}

b2BodyId *create_victors(map_t *map)
{
  world_data_t *wd = b2World_GetUserData(map->world_id);
  b2Hull victor_hull = b2ComputeHull((b2Vec2[]){{-0.5f, -0.75f}, {0.5f, -0.75f}, {0, 0.75f}}, 3);
  b2Polygon victor_polygon = b2MakePolygon(&victor_hull, 0);
  b2BodyDef victor_body_def = b2DefaultBodyDef();
  victor_body_def.type = b2_dynamicBody;
  victor_body_def.linearDamping = 0.5f;
  victor_body_def.angularDamping = 10.0f;

  b2ShapeDef victor_shape_def = b2DefaultShapeDef();
  victor_shape_def.density = 0.2f;
  victor_shape_def.material = b2DefaultSurfaceMaterial();
  victor_shape_def.filter.categoryBits = VICTOR_MASK;
  victor_shape_def.filter.maskBits = ~VICTOR_MASK;
  victor_shape_def.enableSensorEvents = true;
  victor_shape_def.enablePreSolveEvents = true;
  b2BodyId *victors = malloc(wd->victor_c * sizeof(b2BodyId));
  for (int i = 0; i < wd->victor_c; i++) {
    victors[i] = b2CreateBody(map->world_id, &victor_body_def);
    b2CreatePolygonShape(victors[i], &victor_shape_def, &victor_polygon);
    victor_data_t *vd = malloc(sizeof(victor_data_t));
    vd->rays = malloc((wd->victor_ray_c) * sizeof(b2RayResult));
    vd->layers = create_mlp(wd->hlayer_c, wd->neuron_c, wd->victor_ray_c, 2);
    b2Body_SetUserData(victors[i], vd);
    reset_victor(map, victors[i]);
  }
  return victors;
}

void destroy_victors(b2BodyId *victors, int victor_c)
{
  if (!victors)
    return;
  world_data_t *wd = b2World_GetUserData(b2Body_GetWorld(victors[0]));
  for (int i = 0; i < victor_c; i++) {
    victor_data_t *vd = b2Body_GetUserData(victors[i]);
    destroy_mlp(vd->layers, wd->hlayer_c + 1);
    free(vd->rays);
    free(vd);
    b2DestroyBody(victors[i]);
  }
  free(victors);
}

void after_step(b2WorldId world_id, float time_step)
{
  world_data_t *wd = b2World_GetUserData(world_id);
  float sum_score = 0;
  if ((sum_score = findlscore(world_id))) {
    next_generation(world_id, sum_score);
    return;
  }
  for (int i = 0; i < wd->victor_c; i++) {
    victor_data_t *vd = b2Body_GetUserData(wd->victors[i]);
    if (vd->stun > 0) {
      vd->stun -= time_step;
    }
    ray_cast(wd->victor_ray_c, world_id, wd->victors[i]);
    float *inp = malloc(wd->victor_ray_c * sizeof(float));
    inp[0] = vd->rays[0].hit ? vd->rays[0].fraction : -1;
    for (int i = 1; i < wd->victor_ray_c; i++) {
      inp[i] = vd->rays[i].fraction;
    }
    calc_mlp(vd->layers, wd->hlayer_c + 1, inp);
    free(inp);
    vd->acceleration = b2ClampFloat(vd->layers[wd->hlayer_c].neurons[0].o, 0, 1);
    vd->torque = tanhf(vd->layers[wd->hlayer_c].neurons[1].o);
    apply_force(wd->victors[i]);
  }
  wd->game_timer += time_step;
}

void next_generation(b2WorldId world_id, float sum_score)
{
  world_data_t *wd = b2World_GetUserData(world_id);
  float best_score = 0;
  for (int i = 0; i < wd->victor_c; i++) {
    victor_data_t *vd = b2Body_GetUserData(wd->victors[i]);
    if (best_score < vd->score)
      best_score = vd->score;
  }
  wd->score = realloc(wd->score, (wd->generation + 1) * sizeof(float));
  wd->score[wd->generation] = best_score;
  if (wd->max_score < best_score)
    wd->max_score = best_score;
  b2BodyId first_parent = b2_nullBodyId;
  for (int p = 0; p < 2; p++) {
    float r = SDL_randf() * sum_score;
    for (int i = 0; i < wd->victor_c; i++) {
      if (B2_ID_EQUALS(first_parent, wd->victors[i]))
        continue;
      victor_data_t *vd = b2Body_GetUserData(wd->victors[i]);
      if (r < vd->score) {
        copy_mlp(parent_brains[p], vd->layers, wd->hlayer_c + 1);
        first_parent = wd->victors[i];
        break;
      }
      r -= vd->score;
    }
  }
  for (int i = 0; i < wd->victor_c; i++) {
    reset_victor(&wd->map, wd->victors[i]);
    victor_data_t *vd = b2Body_GetUserData(wd->victors[i]);
    cross(vd->layers, wd->hlayer_c + 1, parent_brains);
    mutate(vd->layers, wd->hlayer_c + 1, wd->mutation);
  }
  wd->death_timer = wd->cdeath_timer;
  wd->game_timer = 0;
  wd->generation++;
  if (wd->overdrive < 0)
    wd->overdrive++;
}

float findlscore(b2WorldId world_id)
{
  b2BodyId winner_id = b2_nullBodyId;
  float sum_score = 0;
  float min_score = FLT_MAX;
  world_data_t *wd = b2World_GetUserData(world_id);
  b2SensorEvents se = b2World_GetSensorEvents(world_id);
  b2Segment s;
  if (se.beginCount || se.endCount) {
    b2ShapeId ssh = se.endCount ? se.endEvents[0].sensorShapeId : se.beginEvents[0].sensorShapeId;
    s = b2Shape_GetSegment(ssh);
    for (int i = 0; i < se.endCount; i++) {
      if (!b2Shape_IsValid(se.endEvents[i].visitorShapeId))
        continue;
      b2BodyId victor = b2Shape_GetBody(se.endEvents[i].visitorShapeId);
      victor_data_t *vd = b2Body_GetUserData(victor);
      b2Vec2 pos = b2Body_GetPosition(victor);
      vd->cheater = b2Cross(b2Sub(s.point2, s.point1), b2Sub(pos, s.point1)) < 0;
    }

    for (int i = 0; i < se.beginCount; i++) {
      b2BodyId victor = b2Shape_GetBody(se.beginEvents[i].visitorShapeId);
      victor_data_t *vd = b2Body_GetUserData(victor);
      b2Vec2 pos = b2Body_GetPosition(victor);
      if (b2Cross(b2Sub(s.point2, s.point1), b2Sub(pos, s.point1)) < 0) {
        if (!vd->cheater) {
          winner_id = victor;
          break;
        }
      } else {
        vd->cheater = true;
      }
    }
  }
  if ((wd->death_timer && wd->game_timer > wd->death_timer) || B2_IS_NON_NULL(winner_id)) {
    for (int i = 0; i < wd->victor_c; i++) {
      victor_data_t *vd = b2Body_GetUserData(wd->victors[i]);
      if (vd->cheater) {
        vd->score = 0;
        continue;
      }
      float dist = get_distance(&wd->map, wd->victors[i]);
      /* fitness function */
      vd->score = exp(3 * dist) - 1
        + (B2_ID_EQUALS(winner_id, wd->victors[i]) ? 10
        + (wd->death_timer ? 10 * (1.0f - wd->game_timer / wd->death_timer) : 0) : 0);
      if (min_score > vd->score) {
        min_score = vd->score;
      }
    }
    for (int i = 0; i < wd->victor_c; i++) {
      victor_data_t *vd = b2Body_GetUserData(wd->victors[i]);
      if (vd->score)
        sum_score += (vd->score -= min_score);
    }
    return sum_score;
  }
  return 0;
}

void ray_cast(int ray_c, b2WorldId world_id, b2BodyId victor_id)
{
  b2Rot vr = b2Body_GetRotation(victor_id);
  b2Vec2 vo = b2Body_GetWorldPoint(victor_id, (b2Vec2){0, -0.75f});
  b2QueryFilter filter = b2DefaultQueryFilter();
  filter.maskBits = ~VICTOR_MASK & ~FINISH_MASK;
  victor_data_t *vd = b2Body_GetUserData(victor_id);
  vd->rays[0] = b2World_CastRayClosest(world_id, vo,
      b2RotateVector(vr, (b2Vec2){0, -1.5f}), filter);
  b2Vec2 rv = (b2Vec2){RAY_DIST, 0};
  rv = b2RotateVector(vr, rv);
  b2Rot at = b2MakeRot(B2_PI / (ray_c - 2));
  for (int i = 1; i < ray_c; i++) {
    vd->rays[i] = b2World_CastRayClosest(world_id, vo, rv, filter);
    rv = b2RotateVector(at, rv);
  }
}

void apply_force(b2BodyId victor_id)
{
  victor_data_t *vd = b2Body_GetUserData(victor_id);
  if (vd->stun > 0)
    return;
  float f = (vd->rays[0].hit ? 1/vd->rays[0].fraction : 1.0f) * vd->acceleration;
  b2Vec2 force = b2RotateVector(b2Body_GetRotation(victor_id), (b2Vec2){0, f});
  b2Body_ApplyForceToCenter(victor_id, force, false);
  b2Body_ApplyTorque(victor_id, vd->torque, false);
}

float get_distance(map_t *map, b2BodyId victor)
{
  b2Vec2 spos = b2Shape_GetSegment(map->finish_id).point1;
  b2Vec2 vpos = b2Body_GetPosition(victor);
  int wall_c = b2Chain_GetSegmentCount(map->internal_wall_id);
  b2ShapeId walls[wall_c];
  b2Chain_GetSegments(map->internal_wall_id, walls, wall_c);
  float wall_l = 0;
  b2Vec2 vcpm = b2Vec2_zero;
  float scpdm = FLT_MAX;
  float vcpdm = FLT_MAX;
  int ssi = 0;
  int vsi = 0;
  for (int i = 0; i < wall_c; i++) {
    b2Segment seg = b2Shape_GetChainSegment(walls[i]).segment;
    wall_l += b2Distance(seg.point1, seg.point2);
    b2Vec2 scp = b2Shape_GetClosestPoint(walls[i], spos);
    b2Vec2 vcp = b2Shape_GetClosestPoint(walls[i], vpos);
    float scpd = b2DistanceSquared(scp, spos);
    float vcpd = b2DistanceSquared(vcp, vpos);
    if (scpdm > scpd) {
      scpdm = scpd;
      ssi = i;
    }
    if (vcpdm > vcpd) {
      vcpdm = vcpd;
      vcpm = vcp;
      vsi = i;
    }
  }
  b2Segment seg = b2Shape_GetChainSegment(walls[ssi]).segment;
    float dfs = -b2Distance(spos, seg.point1);
    for (int i = 0; i < wall_c; i++) {
        if (ssi == vsi)
            break;
        seg = b2Shape_GetChainSegment(walls[ssi]).segment;
        dfs += b2Distance(seg.point1, seg.point2);
        ssi = (ssi + 1) % wall_c;
    }
    seg = b2Shape_GetChainSegment(walls[ssi]).segment;
    dfs += b2Distance(vcpm, seg.point1);

    while (dfs < 0.0f) dfs += wall_l;
    while (dfs >= wall_l) dfs -= wall_l;

    return dfs / wall_l;
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
