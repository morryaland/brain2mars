#include <stdlib.h>
#include <msvg.h>
#include <box2d/box2d.h>
#include <emmintrin.h>
#include "game.h"

paths_t *g_svg_paths;

static b2BodyId g_walls;

static void find_map_data(MsvgElement *el, void *udata)
{
  paths_t *paths = udata;
  switch (el->eid) {
    case EID_PATH:
      paths->path = MsvgDupElement(el, 1);
      paths->npath = calloc(1, sizeof(paths_t));
      udata = paths->npath;
      break;
    case EID_LINE: //finish
      break;
    case EID_CIRCLE: //start
      break;
    default:
      return;
  }
}

static b2BodyId paths2segment(paths_t *paths)
{
  b2BodyDef body_def = b2DefaultBodyDef();
  body_def.type = b2_staticBody;
  b2BodyId body_id = b2CreateBody(g_world_id, &body_def);
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
      //chain_def.enableSensorEvents = true;
      b2CreateChain(body_id, &chain_def);
      ply = ply->nsibling;
    }
    MsvgDeleteElement(g);
    paths = paths->npath;
  }
  return body_id;
}

int load_map(char path[])
{
  int err = 0;
  MsvgElement *root = MsvgReadSvgFile(path, &err);
  if (!root)
    return err;
  MsvgRaw2CookedTree(root);
  g_svg_paths = calloc(1, sizeof(paths_t));
  MsvgWalkTree(root, find_map_data, g_svg_paths);
  MsvgDeleteElement(root);
  g_walls = paths2segment(g_svg_paths);
  return 0;
}

void free_svg_paths(paths_t *path)
{
  if (path->npath)
    free_svg_paths(path->npath);
  if (path->path)
    MsvgDeleteElement(path->path);
  free(path);
}

void unload_current_map()
{
  free_svg_paths(g_svg_paths);
  b2DestroyBody(g_walls);
  g_walls = b2_nullBodyId;
}
