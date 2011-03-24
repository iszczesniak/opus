#ifndef COMPARE_ARGS_HPP
#define COMPARE_ARGS_HPP

#include <string>
#include <vector>

using namespace std;

/**
 * These are the compare program arguments.  In this single class we
 * store all information passed at the command line.
 */
struct compare_args
{
  /// The file names.
  vector<string> file_names;

  /// Show others.
  bool show_others;

  /// Show the error mean.
  bool error_mean;

  /// Show the error standard deviation.
  bool error_sdev;
};

/**
 * This function parses the command-line arguments.
 */
compare_args
process_compare_args(int argc, char* argv[]);

#endif /* COMPARE_ARGS_HPP */
