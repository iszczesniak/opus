#include "rand.hpp"

#include <cassert>
#include <cstdlib>

void mysrand(unsigned int n)
{
  srand(n);
}

int myrand(int n)
{
  int ret = (int) (((float) n * (float) rand()) /
                   ((float) RAND_MAX + 1.0));
  
  assert(ret < n);
  return ret;
}
