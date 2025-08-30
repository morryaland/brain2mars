#include <stdlib.h>
#include <msvg.h>
#include <box2d/box2d.h>
#include <emmintrin.h>
#include "game.h"

static void find_map_data(MsvgElement *el, void *udata)
{
  map_t *map = udata;
  paths_t *paths = map->svg_paths;
  switch (el->eid) {
    case EID_PATH:
      paths->path = MsvgDupElement(el, 1);
      paths->npath = calloc(1, sizeof(paths_t));
      udata = paths->npath;
      break;
    case EID_LINE: //finish
      map->start = (b2Vec2){(el->plineattr->x1 + el->plineattr->x2) / 2, (el->plineattr->y1 + el->plineattr->y2) / 2};
      map->finish_id = create_finish_line(map->world_id, (b2Vec2){el->plineattr->x1, el->plineattr->y1},
                         (b2Vec2){el->plineattr->x2, el->plineattr->y2});
      break;
    case EID_CIRCLE: //start
      map->start = (b2Vec2){el->pcircleattr->cx, el->pcircleattr->cy};
      break;
    default:
      return;
  }
}

static b2BodyId paths2segment(b2WorldId world_id, paths_t *paths)
{
  b2BodyDef body_def = b2DefaultBodyDef();
  body_def.type = b2_staticBody;
  b2BodyId body_id = b2CreateBody(world_id, &body_def);
  b2SurfaceMaterial mat = b2DefaultSurfaceMaterial();
  mat.customColor = b2_colorWhite;
  while (paths->path) {
    MsvgElement *g = MsvgPathToPolyGroup(paths->path, PX_X_UNIT);
    if (!g)
      continue;
    MsvgElement *ply = g->fson;
    while (ply) {
      b2ChainDef chain_def = b2DefaultChainDef();
      chain_def.count = ply->ppolygonattr->npoints;
      float *fpoints = malloc(chain_def.count * 2 * sizeof(float));
      for (int i = 0; i < chain_def.count * 2; i += 2) {
        _mm_storel_pi((__m64*)(fpoints + i), _mm_cvtpd_ps(_mm_loadu_pd(ply->ppolygonattr->points + i)));
      }
      chain_def.points = (b2Vec2*)fpoints;
      chain_def.isLoop = true;
      chain_def.materialCount = 1;
      chain_def.materials = (b2SurfaceMaterial[]){mat};
      b2CreateChain(body_id, &chain_def);
      free(fpoints);
      ply = ply->nsibling;
    }
    MsvgDeleteElement(g);
    paths = paths->npath;
  }
  return body_id;
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
  free_svg_paths(map->svg_paths);
  map->svg_paths = NULL;
  b2DestroyBody(map->walls_id);
  map->walls_id = b2_nullBodyId;
  b2DestroyBody(map->finish_id);
  map->finish_id = b2_nullBodyId;
  map->loaded = false;
}
