#include "arr_queue.hpp"
#include "distro.hpp"
#include "nodistro.hpp"
#include "poisson.hpp"
#include "test.hpp"

#include <vector>

using namespace std;


int
main()
{
  // The test for arrangements of two nodistro groups.
  {
    vector<distro> distros;
    distros.push_back(distro(nodistro()));
    distros.push_back(distro(nodistro()));
    
    arr_queue q(distros);
    
    vector<int> arr;
    double prob;
    
    TEST(q.find_next(arr, prob));
    
    EXPECT(arr.size(), 2);
    EXPECT(arr[0], 0);
    EXPECT(arr[1], 0);
    EXPECT(prob, 1.0);
    
    TEST(!q.find_next(arr, prob));
  }

  // The test for arrangements of two poisson groups.
  {
    vector<distro> distros;
    distros.push_back(distro(poisson(0.3)));
    distros.push_back(distro(poisson(0.1)));
    
    arr_queue q(distros);
    
    vector<int> arr;
    double prob;
    
    TEST(q.find_next(arr, prob));
    EXPECT(arr.size(), 2);
    EXPECT(arr[0], 0);
    EXPECT(arr[1], 0);
    
    TEST(q.find_next(arr, prob));
    EXPECT(arr.size(), 2);
    EXPECT(arr[0], 1);
    EXPECT(arr[1], 0);
  }

  return 0;
}
