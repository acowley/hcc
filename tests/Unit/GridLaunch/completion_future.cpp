// XFAIL: Linux
// RUN: %hc -lhc_am %s -o %t.out && %t.out

// FIXME: GridLaunch tests would hang HSA dGPU if executed in multi-thread
// environment. Need further invetigation


#include "grid_launch.h"
#include "hc_am.hpp"
#include <iostream>

#define GRID_SIZE 256
#define TILE_SIZE 16

const int SIZE = GRID_SIZE*TILE_SIZE;

__attribute__((hc_grid_launch)) void kernel1(grid_launch_parm lp, int *x) {
  int i = lp.threadId.x + lp.groupId.x*lp.groupDim.x;

  x[i] = i;
}

int main(void) {

  int *data1 = (int *)malloc(SIZE*sizeof(int));

  int* data1_d = (int*)hc::am_alloc(SIZE*sizeof(int), hc::accelerator(), 0);

  grid_launch_parm lp;
  grid_launch_init(&lp);

  lp.gridDim.x = GRID_SIZE;
  lp.groupDim.x = TILE_SIZE;

  hc::completion_future cf;
  lp.cf = &cf;
  kernel1(lp, data1_d);
  lp.cf->wait();

  hc::am_copy(data1, data1_d, SIZE*sizeof(int));

  bool ret = 0;
  for(int i = 0; i < SIZE; ++i) {
    if(data1[i] != i) {
      ret = 1;
      break;
    }
  }

  hc::am_free(data1_d);
  free(data1);

  return ret;
}
