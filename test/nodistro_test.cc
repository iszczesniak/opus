#include "distro.hpp"
#include "nodistro.hpp"
#include "test.hpp"


int
main()
{
  {
    EXPECT(nodistro::how_many(), 0);
    nodistro d;
    EXPECT(nodistro::how_many(), 1);
    
    // The most probable event.
    EXPECT(d.exists_ith_max(0), true);
    EXPECT(d.get_ith_max(0).first, 1.0);
    EXPECT(d.get_ith_max(0).second, 0);
    
    // The next most probable event.
    EXPECT(d.exists_ith_max(1), false);
    
    // The probability of 0.
    EXPECT(d.get_prob(0), 1.0);
    
    // The probability of something else then 0.
    EXPECT(d.get_prob(1), 0.0);
  }

  // Test the cloning.
  {
    EXPECT(distro_base::how_many(), 0);
    EXPECT(nodistro::how_many(), 0);
    distro_base *ptr = new nodistro();
    EXPECT(distro_base::how_many(), 1);
    EXPECT(nodistro::how_many(), 1);
    distro_base *ptr2 = ptr->clone();
    EXPECT(distro_base::how_many(), 2);
    EXPECT(nodistro::how_many(), 2);
    delete ptr;
    delete ptr2;
    EXPECT(distro_base::how_many(), 0);
    EXPECT(nodistro::how_many(), 0);
  }

  return 0;
}
