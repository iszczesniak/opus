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
check_dad(const dp_matrix &da, ostream &os, const show_args &args)
{
  if (args.dad_mean || args.dad_sdev)
    {
      accumulator_set<double, stats<tag::mean, tag::variance> > acc_dad;

      // For each demand that started at node j and goes to node i.
      FOREACH_MATRIX_ELEMENT(da, i, j, e, dp_matrix)
	{
	  accumulator_set<double, stats<tag::weighted_mean>, double> acc_mean;
	  BOOST_FOREACH(const dist_poly::value_type &v, e)
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
check_dtd(const dp_matrix &dt, ostream &os, const show_args &args)
{
  if (args.dtd_mean || args.dtd_sdev)
    {
      accumulator_set<double, stats<tag::mean, tag::variance> > acc_dtd;

      // For each demand that started at node j and goes to node i.
      FOREACH_MATRIX_ELEMENT(dt, i, j, e, dp_matrix)
	{
	  accumulator_set<double, stats<tag::weighted_mean>, double> acc_mean;
	  BOOST_FOREACH(const dist_poly::value_type &v, e)
	    acc_mean(v.first, weight = v.second.mean());

	  double m = mean(acc_mean);
	  acc_dtd(m);
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

void
check_did(const dp_matrix &at, const dp_matrix &dt, ostream &os,
	  const show_args &args)
{
  if (args.did_mean || args.did_sdev)
    {
      accumulator_set<double, stats<tag::mean, tag::variance> > acc_did;

      // For each demand that started at node j and goes to node i.
      FOREACH_MATRIX_ELEMENT(dt, i, j, e, dp_matrix)
	{
	  double am, dm;

	  // Calculate the mean admission time.
	  {
	    const dist_poly &dp = at.at(i, j);
	    accumulator_set<double, stats<tag::weighted_mean>, double> acc_mean;
	    BOOST_FOREACH(const dist_poly::value_type &v, dp)
	      acc_mean(v.first, weight = v.second.mean());
	    am = mean(acc_mean);
	  }

	  // Calculate the mean delivery time.
	  {
	    accumulator_set<double, stats<tag::weighted_mean>, double> acc_mean;
	    BOOST_FOREACH(const dist_poly::value_type &v, e)
	      acc_mean(v.first, weight = v.second.mean());
	    dm = mean(acc_mean);
	  }

	  // The difference is the mean interconnection time.
	  acc_did(dm - am);
	}

      if (args.show_others)
	os << "Mean of the demand interconnection time: ";
      if (args.did_mean)
	os << mean(acc_did) << std::endl;

      if (args.show_others)
	os << "With the standard deviation of: ";
      if (args.did_sdev)
	os << sqrt(accumulators::variance(acc_did)) << std::endl;
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
  check_dad(at, cout, args);
  check_dtd(dt, cout, args);
  check_did(at, dt, cout, args);

  return 0;
}
