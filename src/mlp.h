#ifndef __MLP_H__
#define __MLP_H__

typedef struct neuron_s {
  float *w;
  float  o;
} neuron_t;

typedef struct layer_s {
  neuron_t *neurons;
  int neuron_c;
  int neuron_weight_c;
} layer_t;

void create_layer(layer_t *layer, int neuron_c, int input_c);

layer_t *create_mlp(int hlayer_c, int neuron_c, int input_c, int output_c);

void destroy_mlp(layer_t *layers, int layer_c);

float ReLU(float n);

void calc_layer(layer_t *layer, float *inputs, float (*f)(float));

void calc_mlp(layer_t *layers, int layer_c, float *inputs);

#endif
