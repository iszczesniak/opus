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
  /// Type of graph.
  enum graph_type {random, benes};

  /// Remembers the graph type.
  pair<bool, graph_type> gt;

  /// The number of nodes in the graph.
  pair<bool, int> nr_nodes;

  /// The number of edges in the graph.
  pair<bool, int> nr_edges;

  /// The name of the output file.
  pair<bool, string> output_filename;

  /// The seed of the random number generator.
  pair<bool, int> seed;
};

/**
 * This function parses the command-line arguments.
 */
netgen_args
process_netgen_args(int argc, char* argv[]);

#endif /* NETGEN_ARGS_HPP */
