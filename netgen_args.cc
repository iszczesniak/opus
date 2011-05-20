#include "netgen_args.hpp"
#include "utils.hpp"
#include <iostream>
#include <map>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

void
check(const netgen_args &args)
{
  // The number of nodes must be given for both the random and benes
  // networks.
  if (!args.nr_nodes.first)
    {
      cerr << "You need to give me the number "
	   << "of nodes to generate.\n";
      exit(1);
    }

  // The number of edges must be given for the random network.
  if (args.gt.second == netgen_args::random &&
      !args.nr_edges.first)
    {
      cerr << "You need to give me the number "
	   << "of edges to generate.\n";
      exit(1);
    }

  // The right number of nodes must be given for the Benes network.
  if (result.gt.second == netgen_args::benes &&
      pop_count(result.nr_nodes.second) != 1)
    {
      cerr << "The number of nodes for the Benes network"
	   << " must be the power of two.\n";
      exit(1);
    }
}

netgen_args
process_netgen_args(int argc, char *argv[])
{
  netgen_args result;

  // The maps of strings to enum values.
  typedef map<string, netgen_args::graph_type> gtm_type;
  gtm_type gtm;
  gtm["random"] = netgen_args::random;
  gtm["benes"] = netgen_args::benes;

  try
    {
      po::options_description opts("Netgen options");
      opts.add_options()
        ("help,h", "produce help message")

        ("nodes,n", po::value<int>(),
         "the number of nodes to generate")

        ("edges,e", po::value<int>(),
         "the number of edges to generate")

        ("output,o", po::value<string>(),
         "the name of the output Graphviz file")

        ("seed,s", po::value<int>()->default_value(1),
         "the seed of the random number generator")

        ("type,t", po::value<string>()->default_value("random"),
         "the type of the network");

      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).options(opts).run(), vm);
      po::notify(vm);

      if (vm.count("help"))
        {
          cout << opts << "\n";
          exit(0);
        }

      // The number of nodes.
      if(vm.count("nodes"))
        result.nr_nodes = make_pair(true, vm["nodes"].as<int>());

      // The number of edges.
      if(vm.count("edges"))
        result.nr_edges = make_pair(true, vm["edges"].as<int>());

      // The string describing the network type.
      string type = vm["type"].as<string>();

      if (gtm.find(type) == gtm.end())
	{
          cerr << "The graph type is invalid.  Use one of:\n";

	  BOOST_FOREACH(const gtm_type::value_type& p, gtm)
	    cerr << p.first << endl;

	  exit(1);
	}
      else
	result.gt = make_pair(true, gtm[type]);
	  
      // The seed for the random number generator.
      result.seed = make_pair(true, vm["seed"].as<int>());

      // The name of the output file.
      if(vm.count("output"))
        result.output_filename = make_pair(true, vm["output"].as<string>());

      check(result);
    }
  catch(const std::exception& e)
    {
      cerr << e.what() << std::endl;
      exit(1);
    }

  return result;
}
