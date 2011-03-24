#include "graph.hpp"
#include "tragen_args.hpp"
#include "utils.hpp"
#include "utils_tragen.hpp"

#include <boost/graph/graphviz.hpp>
#include <boost/graph/random.hpp>
#include <boost/random/linear_congruential.hpp>

using namespace boost::random;
using namespace std;

int main(int argc, char* argv[])
{
  tragen_args args = process_tragen_args(argc, argv);

  minstd_rand gen;
  gen.seed(args.seed);

  // Here we create the graph.
  Graph g;
  if (!read_graphviz_filename(args.input_filename.c_str(), g))
    {
      cerr << "Error loading the graphviz file.\n";
      return 1;
    }

  // The traffic matrix.
  fp_matrix tm;
  generate_tm(g, tm, args.nr_demands, args.poisson_mean);
  ofstream tm_file(args.output_filename.c_str());
  print_tm(g, tm, tm_file);
  tm_file.close();

  return 0;
}
