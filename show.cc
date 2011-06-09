#include "arguments.hpp"
#include "show_args.hpp"
#include "graph.hpp"
#include "graph_serialization.hpp"
#include "matrixes.hpp"
#include "polynomial.hpp"
#include "utils.hpp"
#include "utils_ana.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <set>

#include <boost/foreach.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

using namespace boost::accumulators;

void
check_ll(const map<Edge, double> &ll, const Graph &g, ostream &os,
	 const show_args &args)
{
  if (args.overload_mean || args.load_mean ||
      args.overload_sdev || args.load_sdev)
    {
      accumulator_set<double, stats<tag::mean, tag::variance> > acc_overload;
      accumulator_set<double, stats<tag::mean, tag::variance> > acc_load;

      for(map<Edge, double>::const_iterator i = ll.begin(); i != ll.end(); ++i)
	{
	  double overload = 0;
	  double load = 100 * i->second;

	  if (load > 100)
	    {
	      if (args.show_others)
		os << "Link " << to_string(i->first, g) << ": "
		   << load << "%\n";

	      overload = load - 100;
	    }

	  acc_overload(overload);
	  acc_load(load);
	}

      if (args.show_others)
	os << "A link is above 100% on average by: ";
      if (args.overload_mean)
	os << mean(acc_overload) << std::endl;

      if (args.show_others)
	os << "With the standard deviation of: ";
      if (args.overload_sdev)
	os << sqrt(accumulators::variance(acc_overload)) << std::endl;

      if (args.show_others)
	os << "Average link load: ";
      if (args.load_mean)
	os << mean(acc_load) << std::endl;

      if (args.show_others)
	os << "With the standard deviation of: ";
      if (args.load_sdev)
	os << sqrt(accumulators::variance(acc_load)) << std::endl;
    }
}

void
check_plp(const dp_matrix &at, const dp_matrix &dt, const Graph &g,
	  ostream &os, const show_args &args)
{
  if (args.plp_mean || args.plp_sdev)
    {
      accumulator_set<double, stats<tag::mean, tag::variance> > acc_plp;

      // For each demand that started at node j and goes to node i.
      FOREACH_MATRIX_ELEMENT(dt, i, j, e, dp_matrix)
	{
	  double in_rate = ::sum(at.at(i, j)).mean(); 
	  double out_rate = ::sum(e).mean();

	  double plp = (in_rate - out_rate) / in_rate;
	  acc_plp(plp);
	}

      if (args.show_others)
	os << "Packet loss probability mean: ";
      if (args.plp_mean)
	os << mean(acc_plp) << std::endl;

      if (args.show_others)
	os << "With the standard deviation of: ";
      if (args.plp_sdev)
	os << sqrt(accumulators::variance(acc_plp)) << std::endl;
    }
}

void
check_thr(const dp_matrix &dt, ostream &os, const show_args &args)
{
  if (args.nth || args.dth_mean || args.dth_sdev)
    {
      double network_throughput = 0;

      accumulator_set<double, stats<tag::mean, tag::variance> > acc_thr;

      // For each demand that started at node j and goes to node i.
      FOREACH_MATRIX_ELEMENT(dt, i, j, e, dp_matrix)
	{
	  double out_rate = ::sum(e).mean();
	  acc_thr(out_rate);
	  network_throughput += out_rate;
	}

      if (args.show_others)
	os << "Network throughput: ";
      if (args.nth)
	os << network_throughput << std::endl;

      if (args.show_others)
	os << "Demand throughput mean: ";
      if (args.dth_mean)
	os << mean(acc_thr) << std::endl;

      if (args.show_others)
	os << "With the standard deviation of: ";
      if (args.dth_sdev)
	os << sqrt(accumulators::variance(acc_thr)) << std::endl;
    }
}

void
check_dad(const pp_matrix &ppm, ostream &os, const show_args &args)
{
  if (args.dad_mean || args.dad_sdev)
    {
      accumulator_set<double, stats<tag::mean, tag::variance> > acc_dad;

      // Iterate over every demand and check the admission properties
      // of the demand.  The element e is the packet trajectory for
      // the packet which started at node j and that goes to node i.
      FOREACH_MATRIX_ELEMENT(ppm, i, j, e, pp_matrix)
	{
	  packet_presence::const_iterator e0 = e.find(0);
	  assert(e0 != e.end());
	  map<Vertex, dist_poly>::const_iterator e0j = e0->second.find(j);
	  assert(e0j != e0->second.end());
	  const dist_poly &dp = e0j->second;

	  accumulator_set<double, stats<tag::weighted_mean>, double> acc_mean;
	  BOOST_FOREACH(const dist_poly::value_type &v, dp)
	    acc_mean(v.first, weight = v.second.mean());

	  double m = mean(acc_mean);
	  acc_dad(m);
	}

      if (args.show_others)
	os << "Mean of the demand admission delay: ";
      if (args.dad_mean)
	os << mean(acc_dad) << std::endl;

      if (args.show_others)
	os << "With the standard deviation of: ";
      if (args.dad_sdev)
	os << sqrt(accumulators::variance(acc_dad)) << std::endl;
    }
}

void
check_dtd(const pp_matrix &ppm, ostream &os, const show_args &args)
{
  if (args.dtd_mean || args.dtd_sdev)
    {
      accumulator_set<double, stats<tag::mean, tag::variance> > acc_dtd;

      // Iterate over every demand and check the admission properties
      // of the demand.  The element e is the packet trajectory for
      // the packet which started at node j and that goes to node i.
      FOREACH_MATRIX_ELEMENT(ppm, i, j, e, pp_matrix)
	// Iterate over the hops of a packet_presence.
	for(packet_presence::const_iterator t = e.begin(); t != e.end(); ++t)
	  {
	    map<Vertex, dist_poly>::const_iterator tj = t->second.find(j);
	    // Make sure the packet arrives at the node j after that
	    // hop.
	    if (tj != t->second.end())
	      {
		// This distribution polynomial describes
		const dist_poly &dp = tj->second;

		accumulator_set<double, stats<tag::weighted_mean>, double> acc_mean;
		BOOST_FOREACH(const dist_poly::value_type &v, dp)
		  acc_mean(v.first, weight = v.second.mean());

		double m = mean(acc_mean);
		acc_dtd(m);
	      }
	  }

      if (args.show_others)
	os << "Mean of the demand total time: ";
      if (args.dtd_mean)
	os << mean(acc_dtd) << std::endl;

      if (args.show_others)
	os << "With the standard deviation of: ";
      if (args.dtd_sdev)
	os << sqrt(accumulators::variance(acc_dtd)) << std::endl;
    }
}

int main(int argc, char* argv[])
{
  show_args args = process_show_args(argc, argv);

  if (args.file_name.empty())
    {
      cerr << "I need a name of the results file." << endl;
      return 1;
    }

  const char *ifn = args.file_name.c_str();

  arguments a;
  Graph g;
  fp_matrix tm;
  pp_matrix ppm;
  pt_matrix ptm;

  ifstream ifs(ifn);

  if(!ifs)
    {
      cerr << "Can't open " << ifn << endl;
      return 1;
    }

  boost::archive::text_iarchive ia(ifs);
  ia >> a;
  ia >> g;
  p_graph = &g;
  ia >> tm;
  ia >> ppm;
  ia >> ptm;

  if (args.show_others)
    print_input(g, tm, cout);

  map<Edge, double> ll;
  calculate_ll(ptm, ll, g);

  if (args.show_others)
    print_ll(ll, g, cout);

  if (args.show_others)
    {
      print_ppm(ppm, g, cout);
      print_ptm(ptm, g, cout);
    }

  check_ll(ll, g, cout, args);

  // Admitted traffic.
  dp_matrix at;
  // Delivered traffic.
  dp_matrix dt;

  calculate_at(ppm, at);
  calculate_dt(ppm, dt);

  check_plp(at, dt, g, cout, args);
  check_thr(dt, cout, args);
  check_dad(ppm, cout, args);
  check_dtd(ppm, cout, args);

  return 0;
}
