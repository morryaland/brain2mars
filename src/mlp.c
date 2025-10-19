#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mlp.h"

void create_layer(layer_t *layer, int neuron_c, int input_c)
{
  neuron_t *neurons = layer->neurons = malloc(neuron_c * sizeof(neuron_t));
  layer->neuron_c = neuron_c;
  layer->neuron_weight_c = input_c;
  for (int i = 0; i < neuron_c; i++) {
    float *w = neurons[i].w = malloc(input_c * sizeof(float));
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

void copy_mlp(layer_t *dest, const layer_t *src, int layer_c)
{
  for (int i = 0; i < layer_c; i++) {
    for (int j = 0; j < src[i].neuron_c; j++) {
      memcpy(dest[i].neurons[j].w, src[i].neurons[j].w, src[i].neuron_weight_c * sizeof(float));
    }
  }
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

float ReLU(float n)
{
  return SDL_max(n, 0);
}

void calc_layer(layer_t *layer, float *inputs, float (*f)(float))
{
  for (int i = 0; i < layer->neuron_c; i++) {
    layer->neurons[i].o = 0;
    for (int j = 0; j < layer->neuron_weight_c; j++) {
      layer->neurons[i].o += layer->neurons[i].w[j] * inputs[j];
    }
    if (f)
      layer->neurons[i].o = f(layer->neurons[i].o);
  }
}

void calc_mlp(layer_t *layers, int layer_c, float *inputs)
{
  calc_layer(layers, inputs, tanhf);
  inputs = malloc(layers[0].neuron_c * sizeof(float));
  for (int i = 1; i < layer_c; i++) {
    for (int j = 0; j < layers[i - 1].neuron_c; j++) {
      inputs[j] = layers[i - 1].neurons[j].o;
    }
    calc_layer(layers + i, inputs, NULL);
  }
  free(inputs);
}
