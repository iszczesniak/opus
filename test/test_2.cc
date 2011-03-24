#include "analysis.hpp"
#include "edge_probs.hpp"
#include "graph.hpp"
#include "utils.hpp"
#include "utils_netgen.hpp"
#include "test.hpp"

#include <fstream>
#include <iostream>
#include <list>
#include <map>

#include <boost/graph/random.hpp>
#include <boost/random/linear_congruential.hpp>

using namespace std;

int
main (int argc, char* argv[])
{
  int HL = 5;
  int DL = 1000;

  dist_poly::set_S(DL + 1);

  minstd_rand gen;

  int id = 1;

  for (int nnodes = 2; nnodes < 10; ++nnodes)
    for (int nedges = 2; nedges < 10; ++nedges)
      for (int ndemands = 1; ndemands < 10; ++ndemands)
        for (float load = 0.25; load < 2.1; load += 0.25)
          for (int test = 1; test <= 10; ++test)
            {
              cout << "Testing graph:" << endl;
              cout << "\tid = " << id << endl;
              cout << "\tnodes = " << nnodes << endl;
              cout << "\tedges = " << nedges << endl;
              cout << "\tdemands = " << ndemands << endl;
              cout << "\tload = " << load << endl;
              cout << "\ttest = " << test << endl;

              // Create a graph.  The source and the destination nodes
              // of an edge are chosen randomly, but they are not the
              // same.  No parallel edges are allowed.
              Graph g;
              generate_random_graph(g, nnodes, nedges, gen, false);
              assert(test_parallel(g));

              // Name the vertices.
              name_vertices(g);

              // The distances are from 50 to 200 km included.
              set_distances(g, 50, 200);

              // The lambdas are from 1 to 40 included.
              set_lambdas(g, 1, 40);

              complete_graph(g);

              fp_matrix tm;
              generate_tm(g, tm, ndemands, load);

              // Stores the resulting packet presence matrix.
              list<pp_matrix> ppm_list;

              // Stores the resulting packet trajectory matrix.
              list<pt_matrix> ptm_list;

              map<Edge, double> pll;

              pp_matrix ppm;
              pt_matrix ptm;

              for(int iter = 1; iter <= 20; ++iter)
                {
                  cerr << "Iteration #" << iter << ": " << endl;
                  tie(ppm, ptm) = ana_iteration(g, tm, HL, ptm_list, cerr);
                  ppm_list.push_back(ppm);
                  ptm_list.push_back(ptm);

                  map<Edge, double> ll;
                  calculate_ll(ptm, ll, g);

                  if (iter >= 10)
                    BGL_FORALL_EDGES(e, g, Graph)
                      {
                        if (ll.count(e) && !pll.count(e) ||
                            pll.count(e) && !ll.count(e))
                          {
                            float l = ll.count(e) ? ll[e] : pll[e];

                            if (l > 0.3)
                              cout << "PROBLEM: edge " << e
                                   << " comes and goes: " << l << endl;
                          }

                        if (ll.count(e) && pll.count(e))
                          if (pll[e] != 0 && ll[e] / pll[e] > 1.5 ||
                              ll[e] != 0 && pll[e] / ll[e] > 1.5)
                            cout << "PROBLEM: undulating edge " << e
                                 << " " << ll[e] << ", " << pll[e] << endl;

                        if (ll.count(e) && ll[e] > 1.0)
                          cout << "PROBLEM: overutilized edge " << e
                               << " " << ll[e] << ", " << pll[e] << endl;
                      }

                  pll = ll;
                }

              dynamic_properties dp;

              dp.property("node_id", get(vertex_name, g));
              dp.property("distance", get(edge_weight, g));
              dp.property("lambdas", get(edge_weight2, g));

              ostringstream id_stream;
              id_stream << id;
              string dot_filename = id_stream.str() + ".dot";
              string tm_filename = id_stream.str() + ".tm";
              string A_filename = id_stream.str() + ".A";

              ofstream dot_file(dot_filename.c_str());
              write_graphviz(dot_file, g, dp);
              dot_file.close();

              ofstream tm_file(tm_filename.c_str());
              print_tm(g, tm, tm_file);
              tm_file.close();

              ofstream A_file(A_filename.c_str());
              print_output(ppm, ptm, g, A_file);
              A_file.close();

              ++id;
            }

  return 0;
}
