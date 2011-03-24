#include "distro.hpp"
#include "poisson.hpp"
#include "test.hpp"

#include <sstream>
#include <string>

using namespace std;


int
main()
{
  {
    // Make sure that a default distro is a nodistro.
    distro d;
    EXPECT(d.is_nodistro(), true);
  }

  {
    // Test the is_nodistro function.
    distro d = poisson(1.5);
    EXPECT(d.is_nodistro(), false);
    d = nodistro();
    EXPECT(d.is_nodistro(), true);
  }

  return 0;
}
