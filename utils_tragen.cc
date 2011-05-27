#include "utils.hpp"
#include "utils_tragen.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/random.hpp>
#include <boost/random/linear_congruential.hpp>

#include <cassert>

using namespace std;
using namespace boost::random;

void
generate_tm(const Graph &g, fp_matrix &tm, const tragen_args &args)
{
  assert(args.poisson_mean.first);
  double p = args.poisson_mean.second;

  // The set of vertices between whom the demands will be created.
  set<Vertex> s;

  if (args.nlimit.first)
    {
      int n = args.nlimit.second;
      for(int i = 0; i < n; ++i)
	s.insert(i);
    }
  else
    {
      assert(check_components(g));
      s = *get_components(g).begin();
    }

  switch(args.tt.second)
    {
    case tragen_args::random:
      {
	assert(args.nr_demands.first);
	int d = args.nr_demands.second;
	generate_random_tm(g, tm, d, p, s);
	break;
      }

    case tragen_args::uniform:
      generate_uniform_tm(g, tm, p, s);
      break;

    default:
      assert(false);
    }
}

void
generate_random_tm(const Graph &g, fp_matrix &tm, int demands,
		   double poisson_mean, const set<Vertex> &st)
{
  minstd_rand gen;
  set<Vertex> s = st;

  for(int d = 0; d < demands; ++d)
    {
      Vertex src = get_random_element(s, gen);
      s.erase(src);
      Vertex dst = get_random_element(s, gen);
      s.insert(src);

      tm[dst][src] += poisson_mean;
    }
}

void
generate_uniform_tm(const Graph &g, fp_matrix &tm, double poisson_mean,
		    const set<Vertex> &s)
{
  BOOST_FOREACH(Vertex src, s)
    BOOST_FOREACH(Vertex dst, s)
      if (src != dst)
        tm[dst][src] = poisson_mean;
}
