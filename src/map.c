#include <stdlib.h>
#include <msvg.h>
#include <box2d/box2d.h>
#include <emmintrin.h>
#include "game.h"

static void find_map_data(MsvgElement *el, void *udata)
{
  map_t *map = udata;
  paths_t *paths = map->svg_paths;
  b2Segment line;
  switch (el->eid) {
    case EID_PATH:
      paths->path = MsvgDupElement(el, 1);
      paths->npath = calloc(1, sizeof(paths_t));
      udata = paths->npath;
      break;
    case EID_LINE: //finish
      line = (b2Segment){{el->plineattr->x1, el->plineattr->y1}, {el->plineattr->x2, el->plineattr->y2}};
      map->start = (b2Vec2){(line.point1.x + line.point2.x) / 2, (line.point1.y + line.point2.y) / 2};
      map->rotation = b2MakeRotFromUnitVector(b2Normalize((b2Vec2){line.point2.x - line.point1.x, line.point2.y - line.point1.y}));
      map->finish_id = create_finish_line(map->world_id, line);
      break;
    case EID_CIRCLE: //start
      map->start = (b2Vec2){el->pcircleattr->cx, el->pcircleattr->cy};
      map->rotation = b2MakeRot(el->pcircleattr->r);
      break;
    default:
      return;
  }
}

static b2BodyId paths2segment(b2WorldId world_id, paths_t *paths)
{
  b2BodyDef wall_body_def = b2DefaultBodyDef();
  wall_body_def.type = b2_staticBody;
  b2BodyId wall_body_id = b2CreateBody(world_id, &wall_body_def);
  while (paths->path) {
    MsvgElement *g = MsvgPathToPolyGroup(paths->path, PX_X_UNIT);
    if (!g)
      continue;
    MsvgElement *ply = g->fson;
    bool internal_wall = true;
    while (ply) {
      int pc = ply->ppolygonattr->npoints;
      float *fpoints = malloc(pc * 2 * sizeof(float));
      for (int i = 0; i < pc * 2; i += 2) {
        _mm_storel_pi((__m64*)(fpoints + i), _mm_cvtpd_ps(_mm_loadu_pd(ply->ppolygonattr->points + i)));
      }
      b2ChainId wall_id = create_wall(wall_body_id, (b2Vec2*)fpoints, pc, ply->eid == EID_POLYGON);
      if (internal_wall) {
        b2ChainId *pwall_id = malloc(sizeof(b2ChainId));
        *pwall_id = wall_id;
        b2Body_SetUserData(wall_body_id, pwall_id);
        internal_wall = false;
      }
      free(fpoints);
      ply = ply->nsibling;
    }
    MsvgDeleteElement(g);
    paths = paths->npath;
  }
  return wall_body_id;
}

int load_map(char path[], void *udata)
{
  map_t *map = udata;
  int err = 0;
  if (map->loaded) {
    fputs("unload map before loading\n", stderr);
    return -1;
  }
  MsvgElement *root = MsvgReadSvgFile(path, &err);
  if (!root)
    return err;
  MsvgRaw2CookedTree(root);
  map->svg_paths = calloc(1, sizeof(paths_t));
  MsvgWalkTree(root, find_map_data, map);
  MsvgDeleteElement(root);
  map->walls_id = paths2segment(map->world_id, map->svg_paths);
  b2ChainId *pwall_id = b2Body_GetUserData(map->walls_id);
  map->internal_wall_id = *pwall_id;
  free(pwall_id);
  map->loaded = true;
  return err;
}

void free_svg_paths(paths_t *path)
{
  if (path->npath)
    free_svg_paths(path->npath);
  if (path->path)
    MsvgDeleteElement(path->path);
  free(path);
}

void unload_map(map_t *map)
{
  if (!map->loaded)
    return;
  free_svg_paths(map->svg_paths);
  map->svg_paths = NULL;
  b2DestroyBody(map->walls_id);
  map->walls_id = b2_nullBodyId;
  b2DestroyBody(b2Shape_GetBody(map->finish_id));
  map->finish_id = b2_nullShapeId;
  map->loaded = false;
}
