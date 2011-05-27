#include "utils.hpp"
#include "utils_tragen.hpp"

#include <boost/graph/random.hpp>
#include <boost/random/linear_congruential.hpp>

#include <cassert>

using namespace std;
using namespace boost::random;

void
generate_tm(const Graph &g, fp_matrix &tm, const tragen_args &args)
{
  if (!args.nlimit.first)
    generate_tm(g, tm, args.nr_demands, args.poisson_mean);
  else
    {
      set<Vertex> s;
      for(int i = 0; i < args.nlimit.second; ++i)
	s.insert(i);
      generate_tm(g, tm, args.nr_demands, args.poisson_mean, s);
    }
}

void
generate_tm(const Graph &g, fp_matrix &tm, int demands, double poisson_mean)
{
  assert(check_components(g));
  set<Vertex> s = *get_components(g).begin();
  generate_tm(g, tm, demands, poisson_mean, s);
}

void
generate_tm(const Graph &g, fp_matrix &tm, int demands, double poisson_mean,
	    set<Vertex> s)
{
  minstd_rand gen;

  for(int d = 0; d < demands; ++d)
    {
      Vertex src = get_random_element(s, gen);
      s.erase(src);
      Vertex dst = get_random_element(s, gen);
      s.insert(src);

      tm[dst][src] += poisson_mean;
    }
}
