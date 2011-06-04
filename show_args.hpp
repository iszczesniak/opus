#ifndef SHOW_ARGS_HPP
#define SHOW_ARGS_HPP

#include <string>

using namespace std;

/**
 * These are the compare program arguments.  In this single class we
 * store all information passed at the command line.
 */
struct show_args
{
  /// The file name.
  string file_name;

  /// Show others.
  bool show_others;

  /// Show the overload mean.
  bool overload_mean;

  /// Show the overload standard deviation.
  bool overload_sdev;

  /// Show the load mean.
  bool load_mean;

  /// Show the load standard deviation.
  bool load_sdev;

  /// Show the packet loss probability mean.
  bool plp_mean;

  /// Show the packet loss probability standard deviation.
  bool plp_sdev;

  /// Show the network throughput.
  bool nth;

  /// Show the demand throughput mean.
  bool dth_mean;

  /// Show the demand throughput standard deviation.
  bool dth_sdev;

  /// Show the mean of the demand admission time.
  bool dad_mean;

  /// Show the standard deviation of the demand admission time.
  bool dad_sdev;
};

/**
 * This function parses the command-line arguments.
 */
show_args
process_show_args(int argc, char* argv[]);

#endif /* SHOW_ARGS_HPP */
