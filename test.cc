#include "test.hpp"

#include <boost/graph/iteration_macros.hpp>

void
test_ll(const map<Edge, double> &ll,
	const Graph &g, std::ostream &os)
{
  for(map<Edge, double>::const_iterator i = ll.begin();
      i != ll.end(); ++i)
    if (!(0 <= i->second && i->second <= 1.0))
      os << "Load of " << i->first
	 << " is wrong: " << i->second << endl;
}

bool
test_parallel(const Graph &g)
{
  BGL_FORALL_EDGES(e, g, Graph)
    BGL_FORALL_EDGES(f, g, Graph)
    if (e != f &&
	source(e, g) == source(f, g) &&
	target(e, g) == target(f, g))
      return false;

  return true;
}

bool
test_graphs(const Graph &g1, const Graph &g2)
{
  // Check the number of vertices.
  if (!(num_vertices(g1) == num_vertices(g2)))
    return false;

  // Check the number of edges.
  if (!(num_edges(g1) == num_edges(g2)))
    return false;

  // Check the vertex properties.
  if (!test_property<vertex_name_t>(g1, g2))
    return false;
  if (!test_property<vertex_distance_t>(g1, g2))
    return false;
  if (!test_property<vertex_predecessor_t>(g1, g2))
    return false;
  if (!test_property<vertex_bundle_t>(g1, g2))
    return false;

  // Check the vertex properties.
  if (!test_property<edge_weight_t>(g1, g2))
    return false;
  if (!test_property<edge_weight2_t>(g1, g2))
    return false;

  return true;
}
