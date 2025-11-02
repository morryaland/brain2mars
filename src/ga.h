#ifndef __GA_H__
#define __GA_H__

#include "mlp.h"

void cross(layer_t *victor_layers, int layer_c, layer_t **parent_brains, int parent_c);

void mutate(layer_t *victor_layers, int layer_c, float mutation);

#endif
