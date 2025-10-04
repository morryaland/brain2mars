#include <SDL3/SDL.h>
#include <stdlib.h>
#include "mlp.h"

void create_layer(layer_t *layer, int neuron_c, int input_c)
{
  neuron_t *neurons = layer->neurons = malloc(neuron_c * sizeof(neuron_t));
  layer->neuron_c = neuron_c;
  for (int i = 0; i < neuron_c; i++) {
    float *w = neurons[i].w = malloc(input_c * sizeof(float));
    neurons[i].w_c = input_c;
    for (int j = 0; j < input_c; j++) {
      /* rand */
      w[j] = 1 - SDL_randf() * 2;
    }
  }
}

layer_t *create_mlp(int hlayer_c, int neuron_c, int input_c, int output_c)
{
  layer_t *layers = malloc((hlayer_c + 1) * sizeof(layer_t));
  create_layer(layers, neuron_c, input_c);
  for (int i = 1; i < hlayer_c; i++) {
    create_layer(layers + i, neuron_c, neuron_c);
  }
  create_layer(layers + hlayer_c, output_c, neuron_c);
  return layers;
}

void destroy_mlp(layer_t *layers, int layer_c)
{
  for (int i = 0; i < layer_c; i++) {
    neuron_t *n = layers[i].neurons;
    for (int j = 0; j < layers[i].neuron_c; j++) {
      free(n[j].w);
    }
    free(n);
  }
  free(layers);
}
