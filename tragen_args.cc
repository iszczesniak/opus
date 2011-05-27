#include "tragen_args.hpp"
#include "utils.hpp"
#include <iostream>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

void
check(const tragen_args &args)
{
  if (!args.input_filename.first)
    {
      cerr << "You need to give me the input "
	   << "file name.\n";
      exit(1);
    }

  if (!args.poisson_mean.first)
    {
      cerr << "You need to give me the mean of "
	   << "the Poisson distribution.\n";
      exit(1);
    }

  // Conditions for the random traffic.
  if (args.tt.second == tragen_args::random)
    {
      if (!args.nr_demands.first)
	{
	  cerr << "You need to give me the number "
	       << "of demands to generate.\n";
	  exit(1);
	}
    }
}

tragen_args
process_tragen_args(int argc, char *argv[])
{
  tragen_args result;

  // The map of strings to enum values.
  map<string, tragen_args::traffic_type> ttm;
  ttm["random"] = tragen_args::random;
  ttm["uniform"] = tragen_args::uniform;

  try
    {
      po::options_description opts("Netgen options");
      opts.add_options()
        ("help,h", "produce help message")

        ("demands,d", po::value<int>(),
         "the number of demands to generate")

        ("poisson,p", po::value<double>(),
         "the mean of the Poisson distribution")

        ("input,i", po::value<string>(),
         "the name of the input file with topology")

        ("output,o", po::value<string>(),
         "the name of the output file with traffic matrix")

        ("seed,s", po::value<int>()->default_value(1),
         "the seed of the random number generator")

        ("nlimit,n", po::value<int>(),
         "the number of first nodes for which to generate traffic")

        ("type,t", po::value<string>()->default_value("random"),
         "the traffic type");

      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).options(opts).run(), vm);
      po::notify(vm);

      if (vm.count("help"))
        {
          cout << opts << "\n";
          exit(0);
        }

      if(vm.count("demands"))
        result.nr_demands = make_pair(true, vm["demands"].as<int>());

      if(vm.count("poisson"))
        result.poisson_mean = make_pair(true, vm["poisson"].as<double>());

      if(vm.count("input"))
        result.input_filename = make_pair(true, vm["input"].as<string>());

      if(vm.count("output"))
        result.output_filename = make_pair(true, vm["output"].as<string>());

      if(vm.count("nlimit"))
        result.nlimit = make_pair(true, vm["nlimit"].as<int>());

      // The seed for the random number generator.
      if(vm.count("seed"))
	result.seed = make_pair(true, vm["seed"].as<int>());

      // The traffic type.
      if (vm.count("type"))
        {
          tragen_args::traffic_type t;
          t = string_to_enum(vm["type"].as<string>(), ttm);
          result.tt = make_pair(true, t);
        }

      check(result);
    }
  catch(const std::exception& e)
    {
      cerr << e.what() << std::endl;
      exit(1);
    }

  return result;
}
