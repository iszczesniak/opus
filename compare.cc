#include "arguments.hpp"
#include "compare_args.hpp"
#include "graph.hpp"
#include "graph_serialization.hpp"
#include "matrixes.hpp"
#include "test.hpp"
#include "utils.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <set>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/serialization/string.hpp>

using namespace boost::accumulators;
using namespace boost::filesystem;

void
compare_ll(const pt_matrix &ptm1, const Graph &g1,
           const pt_matrix &ptm2, const Graph &g2,
           const compare_args &args)
{
  map<Edge, double> ll1, ll2;
  calculate_ll(ptm1, ll1, g1);
  calculate_ll(ptm2, ll2, g2);

  // These are the edges used in either or both maps.
  set<Edge> present;

  // Notice the edges in the first link load map.
  for(map<Edge, double>::const_iterator i = ll1.begin(); i != ll1.end(); ++i)
    present.insert(i->first);

  // Notice the edges in the second link load map.
  for(map<Edge, double>::const_iterator i = ll2.begin(); i != ll2.end(); ++i)
    present.insert(i->first);

  accumulator_set<double, stats<tag::mean> > acc_l1;
  accumulator_set<double, stats<tag::mean> > acc_l2;
  accumulator_set<double, stats<tag::mean, tag::variance> > acc_diff;

  for(set<Edge>::iterator i = present.begin(); i != present.end(); ++i)
    {
      Edge e = *i;

      bool present1 = ll1.find(e) != ll1.end();
      double load1 = present1 ? 100 * ll1[e] : 0;
      bool present2 = ll2.find(e) != ll2.end();
      double load2 = present2 ? 100 * ll2[e] : 0;

      double diff = abs(load1 - load2);

      // Push the sample to the accumulator.
      acc_l1(load1);
      acc_l2(load2);
      acc_diff(diff);

      if (args.show_others)
        {
          cout << "Link " << to_string(e, g1) << ": ";

          if (present1)
            cout << load1 << "%";
          else
            cout << "NULL";
          cout << " for input 1, ";

          if (present2)
            cout << load2 << "%";
          else
            cout << "NULL";
          cout << " for input 2; ";

          cout << diff << "% difference." << endl;
        }
    }

  if (args.show_others)
    {
      cout << "Mean link load for input 1: "
           << mean(acc_l1) << "%" << std::endl;
      cout << "Mean link load for input 2: "
           << mean(acc_l2) << "%" << std::endl;
    }

  // Print the mean error.
  if (args.show_others)
    cout << "Mean error: ";
  if (args.error_mean)
    cout << mean(acc_diff) << std::endl;

  // Print the standard deviation of error.
  if (args.show_others)
    cout << "Standard deviation of error: ";
  if (args.error_sdev)
    cout << sqrt(accumulators::variance(acc_diff)) << std::endl;
}

void
check_args(const arguments &a1, const arguments &a2)
{
  if (a1.DL != a2.DL)
    cerr << "Values of DL differ." << endl;

  if (a1.HL != a2.HL)
    cerr << "Values of HL differ." << endl;

  if (a1.graphviz_filename != a2.graphviz_filename)
    cerr << "Graph file names differ." << endl;

  if (a1.tm_filename != a2.tm_filename)
    cerr << "Traffic matrix file names differ." << endl;
}

bool
file_exists(const string &ifn)
{
  if (!exists(ifn))
    {
      std::cerr << "Can't find " << ifn << std::endl;
      return false;
    }

  return true;
}

bool
load_archive(const string &ifn, arguments &a, Graph &g, fp_matrix &tm,
             pt_matrix &ptm)
{
  ifstream ifs(ifn.c_str());

  if(!ifs)
    {
      cerr << "Can't open " << ifn <<  " \n";
      return false;
    }

  boost::archive::text_iarchive ia(ifs);
  ia >> a;
  ia >> g;
  p_graph = &g;
  ia >> tm;
  ia >> ptm;

  return true;
}

int main(int argc, char* argv[])
{
  compare_args args = process_compare_args(argc, argv);

  if (args.file_names.size() != 2)
    {
      cerr << "I need names of exactly two files to compare." << endl;
      return 1;
    }

  arguments a1, a2;
  Graph g1, g2;
  fp_matrix tm1, tm2;
  pt_matrix ptm1, ptm2;

  if (!file_exists(args.file_names[0]))
    return 1;

  if (!file_exists(args.file_names[1]))
    return 1;

  if (!load_archive(args.file_names[0], a1, g1, tm1, ptm1))
    return 1;

  if (!load_archive(args.file_names[1], a2, g2, tm2, ptm2))
    return 1;

  // Check the arguments loaded from the archives.
  check_args(a1, a2);
  TEST(test_graphs(g1, g2));

  compare_ll(ptm1, g1, ptm2, g2, args);

  return 0;
}
