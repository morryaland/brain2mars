#include "game.h"

simulation_t g_sim = {
  .death_timer = 0,
  .generation = 0,
  .mutation = 0.1,
  .hlayer_c = 2,
  .neuron_c = 8,
  .victor_c = 100,
  .victor_inputs = 3
};

int load_checkpoint(char path[])
{

}
