#include "distro.hpp"
#include "geometric.hpp"
#include "test.hpp"

#include <gsl/gsl_randist.h>

int
main()
{
  {
    double p = 0.25;

    EXPECT(geometric::how_many(), 0);
    geometric dist(p);
    EXPECT(geometric::how_many(), 1);

    EXPECT(dist.get_prob(1), gsl_ran_geometric_pdf(1, p));
    EXPECT(dist.get_prob(10), gsl_ran_geometric_pdf(10, p));
  }

  // Test the cloning.
  {
    EXPECT(distro_base::how_many(), 0);
    EXPECT(geometric::how_many(), 0);
    distro_base *ptr = new geometric(1.0);
    EXPECT(distro_base::how_many(), 1);
    EXPECT(geometric::how_many(), 1);
    distro_base *ptr2 = ptr->clone();
    EXPECT(distro_base::how_many(), 2);
    EXPECT(geometric::how_many(), 2);
    delete ptr;
    delete ptr2;
    EXPECT(distro_base::how_many(), 0);
    EXPECT(geometric::how_many(), 0);
  }

  // Test how the geometric is enveloped.
  {
    distro d1 = geometric(2.0);
    EXPECT(distro::how_many(), 1);
    EXPECT(distro_base::how_many(), 1);
    EXPECT(geometric::how_many(), 1);
    distro d2 = d1;
    EXPECT(distro::how_many(), 2);
    EXPECT(distro_base::how_many(), 2);
    EXPECT(geometric::how_many(), 2);
  }
  EXPECT(distro::how_many(), 0);
  EXPECT(distro_base::how_many(), 0);
  EXPECT(geometric::how_many(), 0);

  return 0;
}
