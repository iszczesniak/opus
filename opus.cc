#include "analysis.hpp"
#include "arguments.hpp"
#include "graph.hpp"
#include "graph_serialization.hpp"
#include "matrixes.hpp"
#include "poisson.hpp"
#include "serializer.hpp"
#include "rand.hpp"
#include "simulation.hpp"
#include "utils.hpp"

#include <ctime>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>

using namespace std;

int
main (int argc, char* argv[])
{
  arguments args = process_args(argc, argv);

  // Here we create the graph.
  Graph g;
  if (!read_graphviz_filename(args.graphviz_filename.c_str(), g))
    {
      cerr << "Error loading the graphviz file.\n";
      return 1;
    }

  // Here we create the traffic matrix.
  fp_matrix tm;
  if (!load_tm_file(args.tm_filename.c_str(), get_vertex_names(g), tm))
    {
      cerr << "Error loading the traffic matrix file.\n";
      return 1;
    }

  // Stores the results: the packet presence matrix.
  pp_matrix ppm;

  // Stores the results: the packet trajectory matrix.
  pt_matrix ptm;

  if (args.touch_random_seed)
    {
      time_t ourTime;
      time(&ourTime);
      mysrand((unsigned int)ourTime);
    }

  // The serializer for saving results.
  serializer s(args, g, tm);

  if (args.method == analysis)
    tie(ppm, ptm) = ana_solution(g, tm, args.HL, args.DL, args.iters,
                                 args.AL, cerr, s);
  else
    tie(ppm, ptm) = sim_solution(g, tm, args.HL, args.DL, cerr);

  // Save the final ppm and ptm.
  s(ppm, ptm);

  return 0;
}
