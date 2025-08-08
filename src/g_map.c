#include <stdlib.h>
#include <msvg.h>
#include <box2d/box2d.h>
#include <emmintrin.h>
#include "game.h"

paths_t *g_svg_paths;

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

static void paths2segment(paths_t *paths)
{
  b2BodyDef body_def = b2DefaultBodyDef();
  body_def.type = b2_staticBody;
  b2BodyId body_id = b2CreateBody(g_world_id, &body_def);
  b2SurfaceMaterial mat = b2DefaultSurfaceMaterial();
  mat.customColor = b2_colorWhite;
  while (paths->path) {
    MsvgElement *g = MsvgPathToPolyGroup(paths->path, 5);
    if (!g)
      continue;
    MsvgElement *ply = g->fson;
    while (ply) {
      MsvgPrintCookedElement(stdout, ply);
      b2ChainDef chain_def = b2DefaultChainDef();
      chain_def.count = ply->ppolygonattr->npoints;
      float *fpoints = malloc(chain_def.count * 2 * sizeof(float));
      for (int i = 0; i < chain_def.count * 2; i += 2) {
        _mm_storel_pi((__m64*)(fpoints + i), _mm_cvtpd_ps(_mm_loadu_pd(ply->ppolygonattr->points + i)));
      }
      chain_def.points = (b2Vec2*)fpoints;
      chain_def.isLoop = true;
      chain_def.materialCount = 1;
      chain_def.materials = &mat;
      //chain_def.enableSensorEvents = true;
      b2CreateChain(body_id, &chain_def);
      ply = ply->nsibling;
    }
    MsvgDeleteElement(g);
    paths = paths->npath;
  }
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
  paths2segment(g_svg_paths);
  return 0;
}
