#include "analysis.hpp"
#include "edge_probs.hpp"
#include "graph.hpp"
#include "utils.hpp"
#include "utils_netgen.hpp"
#include "test.hpp"

#include <fstream>
#include <iostream>
#include <map>

#include <boost/graph/random.hpp>
#include <boost/random/linear_congruential.hpp>

using namespace std;

int
main (int argc, char* argv[])
{
  int HL = 5;
  int DL = 1000;

  minstd_rand gen;

  int id = 1;

  for (int nodes = 2; nodes < 10; ++nodes)
    for (int edges = 2; edges < 10; ++edges)
      for (int demands = 2; demands < 10; ++demands)
        for (float load = 0.25; load < 2.1; load += 0.25)
          for (int test = 1; test <= 10; ++test)
            {
              cout << "Testing graph:" << endl;
              cout << "\tid = " << id << endl;
              cout << "\tnodes = " << nodes << endl;
              cout << "\tedges = " << edges << endl;
              cout << "\tdemands = " << demands << endl;
              cout << "\tload = " << load << endl;
              cout << "\ttest = " << test << endl;

              // Create a graph.  The source and the destination nodes
              // of an edge are chosen randomly, but they are not the
              // same.  No parallel edges are allowed.
              Graph g;
              generate_random_graph(g, nodes, edges, gen, false);
              assert(test_parallel(g));

              // Name the vertices.
              name_vertices(g);

              // The distances are from 50 to 200 km included.
              set_distances(g, 50, 200);

              // The lambdas are from 1 to 40 included.
              set_lambdas(g, 1, 40);

              complete_graph(g);

              fp_matrix tm;
              generate_tm(g, tm, demands, load);

              // Stores the resulting packet presence matrix.
              pp_matrix ppm;

              // Stores the resulting packet trajectory matrix.
              pt_matrix ptm;

              tie(ppm, ptm) = ana_solution(g, tm, HL, DL, 10, 10, cerr);

              map<Edge, double> ll;
              calculate_ll(ptm, ll, g);

              test_ll(ll, g, cout);

              dynamic_properties dp;

              dp.property("node_id", get(vertex_name, g));
              dp.property("distance", get(edge_weight, g));
              dp.property("lambdas", get(edge_weight2, g));

              ostringstream id_stream;
              id_stream << id;
              string dot_filename = id_stream.str() + ".dot";
              string tm_filename = id_stream.str() + ".tm";

              ofstream dot_file(dot_filename.c_str());
              write_graphviz(dot_file, g, dp);
              dot_file.close();

              ofstream tm_file(tm_filename.c_str());
              print_tm(g, tm, tm_file);
              tm_file.close();

              ++id;
            }

  return 0;
}
