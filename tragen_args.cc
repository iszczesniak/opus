#include "tragen_args.hpp"
#include <iostream>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

tragen_args
process_tragen_args(int argc, char *argv[])
{
  tragen_args result;

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
         "the seed of the random number generator");

      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).options(opts).run(), vm);
      po::notify(vm);

      if (vm.count("help"))
        {
          cout << opts << "\n";
          exit(0);
        }

      if(vm.count("demands"))
        result.nr_demands = vm["demands"].as<int>();
      else
        {
          cerr << "You need to give me the number "
               << "of demands to generate.\n";
          exit(1);
        }

      if(vm.count("poisson"))
        result.poisson_mean = vm["poisson"].as<double>();
      else
        {
          cerr << "You need to give me the mean of "
               << "the Poisson distribution.\n";
          exit(1);
        }

      if(vm.count("input"))
        result.input_filename = vm["input"].as<string>();
      else
        {
          cerr << "You need to give me the input "
               << "file name.\n";
          exit(1);
        }

      if(vm.count("output"))
        result.output_filename = vm["output"].as<string>();
      else
        {
          cerr << "You need to give me the output "
               << "file name.\n";
          exit(1);
        }

      // The seed for the random number generator.
      result.seed = vm["seed"].as<int>();
    }
  catch(const std::exception& e)
    {
      cerr << e.what() << std::endl;
      exit(1);
    }

  return result;
}
