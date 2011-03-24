#include "distro.hpp"
#include "poisson.hpp"
#include "test.hpp"

#include <algorithm>
#include <cassert>
#include <list>

#include <gsl/gsl_randist.h>

#define MAX_K 300

using namespace std;

// My simple function for finding the maximum of the Poisson
// distribution.
static int
my_poisson_mode(double lambda)
{
  assert(lambda > 0);

  int mode = -1;
  double f_max = -1;

  for (int k = 0; k < MAX_K; ++k)
    {
      double f = gsl_ran_poisson_pdf(k, lambda);

      if (f > f_max) 
        {
          f_max = f;
          mode = k;
        }
    }
  
  return mode;
}


deque<pair<double, int> >
my_poisson_distro(double lambda)
{
  deque<pair<double, int> > distro;

  for (int k = 0; k < MAX_K; ++k)
    {
      double f = gsl_ran_poisson_pdf(k, lambda);
      distro.push_back(make_pair(f, k));
    }
  
  sort(distro.begin(), distro.end());
  reverse(distro.begin(), distro.end());
  
  return distro;
}


int
main()
{
  // GSL random number generator
  const gsl_rng_type *T_rng;
  gsl_rng *rng;
  gsl_rng_env_setup();
  T_rng = gsl_rng_default;
  rng = gsl_rng_alloc(T_rng);
  gsl_rng_set (rng, 0);
  
  // Make sure that the function poisson_mode works OK.
  {
    list<double> lambdas;

    for (int i = 0; i < 100; ++i)
      {
        double num = gsl_rng_uniform(rng);
        if (num > 0.00001)
          {
            lambdas.push_back(num / 100);
            lambdas.push_back(num);
            lambdas.push_back(num * 100);
          }
      }

    for (int i = 1; i < 100; ++i)
      lambdas.push_back(i);

    while(!lambdas.empty())
      {
        double lambda = lambdas.back();

        int my_mode = my_poisson_mode(lambda);
        double my_p = gsl_ran_poisson_pdf(my_mode, lambda);
        
        int mode = poisson::mode(lambda);
        double p = gsl_ran_poisson_pdf(mode, lambda);

        TEST(my_p == p);

        lambdas.pop_back();
      }
  }

  // Make sure that the class poisson works fine.
  {
    list<double> lambdas;

    for (int i = 0; i < 100; ++i)
      {
        double num = gsl_rng_uniform(rng);
        if (num > 0.00001)
          {
            lambdas.push_back(num / 100);
            lambdas.push_back(num);
            lambdas.push_back(num * 100);
          }
      }
    
    while(!lambdas.empty())
      {
        double lambda = lambdas.back();

        deque<pair<double, int> > my_distro;
        my_distro = my_poisson_distro(lambda);
        
        EXPECT(poisson::how_many(), 0);
        poisson distro(lambda);
        EXPECT(poisson::how_many(), 1);
        
        for(int i = 0; i < my_distro.size(); ++i)
          {
            assert(distro.exists_ith_max(i));
            // We don't test the probs if they both equal 0.  This can
            // easily happen for low lambda and large k.
            if (my_distro[i].first != 0 || distro.get_ith_max(i).first != 0)
              TEST(my_distro[i] == distro.get_ith_max(i));
          }

        lambdas.pop_back();
      }
  }

  {
    double lambda = 2.5;
    
    EXPECT(poisson::how_many(), 0);
    poisson p(lambda);
    EXPECT(poisson::how_many(), 1);

    EXPECT(p.get_prob(1), gsl_ran_poisson_pdf(1, lambda));
    EXPECT(p.get_prob(10), gsl_ran_poisson_pdf(10, lambda));
  }

  // Test the cloning.
  {
    EXPECT(distro_base::how_many(), 0);
    EXPECT(poisson::how_many(), 0);
    distro_base *ptr = new poisson(1.0);
    EXPECT(distro_base::how_many(), 1);
    EXPECT(poisson::how_many(), 1);
    distro_base *ptr2 = ptr->clone();
    EXPECT(distro_base::how_many(), 2);
    EXPECT(poisson::how_many(), 2);
    delete ptr;
    delete ptr2;
    EXPECT(distro_base::how_many(), 0);
    EXPECT(poisson::how_many(), 0);
  }

  // Test how the poisson is enveloped.
  {
    distro d1 = poisson(2.0);
    EXPECT(distro::how_many(), 1);
    EXPECT(distro_base::how_many(), 1);
    EXPECT(poisson::how_many(), 1);
    distro d2 = d1;
    EXPECT(distro::how_many(), 2);
    EXPECT(distro_base::how_many(), 2);
    EXPECT(poisson::how_many(), 2);
  }
  EXPECT(distro::how_many(), 0);
  EXPECT(distro_base::how_many(), 0);
  EXPECT(poisson::how_many(), 0);

  gsl_rng_free(rng);
  
  return 0;
}
