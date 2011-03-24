#ifndef TRAGEN_ARGS_HPP
#define TRAGEN_ARGS_HPP

#include <string>

using namespace std;

/**
 * These are the program arguments.  In this single class we store all
 * information passed at the command line.
 */
struct tragen_args
{
  /// The name of the input graph file.
  string input_filename;

  /// The name of the output traffic matrix file.
  string output_filename;

  /// The number of demands to generate.
  int nr_demands;

  /// The mean of the poisson distribution.
  double poisson_mean;

  /// The seed of the random number generator.
  int seed;
};

/**
 * This function parses the command-line arguments.
 */
tragen_args
process_tragen_args(int argc, char* argv[]);

#endif /* TRAGEN_ARGS_HPP */
