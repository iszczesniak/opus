#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include <boost/program_options.hpp>
#include <string>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

using namespace std;

enum method_type {analysis, simulation};

/**
 * These are the program arguments.  In this single class we store all
 * information passed at the command line.
 */
class arguments
{
  friend class boost::serialization::access;
  friend bool operator == (const arguments &, const arguments &);

  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & graphviz_filename;
    ar & tm_filename;
    ar & output_filename;
    ar & HL;
    ar & DL;
    ar & iters;
    ar & method;
    ar & touch_random_seed;
    ar & AL;
  }

public:
  /// The name of the graph file.
  string graphviz_filename;

  /// The name of the traffic matrix file.
  string tm_filename;

  /// The name of the output file.
  string output_filename;

  /// Hop limit
  int HL;

  /// Distance limit
  int DL;

  /// The number of iterations
  int iters;

  /// The number of iterations after which to cyclicaly save the
  /// results.
  int step;

  /// Either analysis or simulation.
  method_type method;

  /// If true, touch the random seed.
  bool touch_random_seed;

  /// Average limit;
  int AL;
};

bool operator == (const arguments &, const arguments &);

/**
 * This function parses the command-line arguments.
 */
arguments
process_args(int argc, char* argv[]);

#endif /* ARGUMENTS_HPP */
