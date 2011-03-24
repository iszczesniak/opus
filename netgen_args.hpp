#ifndef NETGEN_ARGS_HPP
#define NETGEN_ARGS_HPP

#include <string>

using namespace std;

/**
 * These are the program arguments.  In this single class we store all
 * information passed at the command line.
 */
struct netgen_args
{
  /// The number of nodes in the graph.
  int nr_nodes;

  /// The number of edges in the graph.
  int nr_edges;

  /// The name of the output file.
  string output_filename;

  /// The seed of the random number generator.
  int seed;
};

/**
 * This function parses the command-line arguments.
 */
netgen_args
process_netgen_args(int argc, char* argv[]);

#endif /* NETGEN_ARGS_HPP */
