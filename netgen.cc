#include "graph.hpp"
#include "netgen_args.hpp"
#include "utils.hpp"
#include "utils_netgen.hpp"
#include "test.hpp"

#include <boost/graph/graphviz.hpp>
#include <boost/random/linear_congruential.hpp>

using namespace boost::random;
using namespace std;

int main(int argc, char* argv[])
{
  netgen_args args = process_netgen_args(argc, argv);

  minstd_rand gen;
  gen.seed(args.seed);

  // Create a graph.  The source and the destination nodes of an edge
  // are chosen randomly, but they are not the same.  No parallel
  // edges are allowed.
  Graph g;
  generate_graph(g, args.nr_nodes, args.nr_edges, gen);
  assert(test_parallel(g));

  // Name the vertices.
  name_vertices(g);

  // The distances are from 50 to 200 km included.
  set_distances(g, 50, 200, gen);

  // The lambdas are from 1 to 40 included.
  set_lambdas(g, 1, 40, gen);

  dynamic_properties dp;

  dp.property("node_id", get(vertex_name, g));
  dp.property("distance", get(edge_weight, g));
  dp.property("lambdas", get(edge_weight2, g));

  ofstream dot_file(args.output_filename.c_str());
  write_graphviz(dot_file, g, dp);
  dot_file.close();

  return 0;
}
