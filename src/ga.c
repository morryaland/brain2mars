#include <SDL3/SDL.h>
#include "mlp.h"

void cross(layer_t *victor_layers, int layer_c, layer_t **parent_brains, int parent_brain_c)
{
  for (int i = 0; i < layer_c; i++) {
    for (int j = 0; j < victor_layers[i].neuron_c; j++) {
      for (int k = 0; k < victor_layers[i].neuron_weight_c; k++) {
        victor_layers[i].neurons[j].w[k] = parent_brains[SDL_rand(parent_brain_c)][i].neurons[j].w[k];
      }
    }
  }
}
