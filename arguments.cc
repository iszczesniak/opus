#include "arguments.hpp"
#include <iostream>

using namespace std;
namespace po = boost::program_options;

bool operator == (const arguments &a1, const arguments &a2)
{
  if (a1.graphviz_filename != a2.graphviz_filename)
    return false;
  if (a1.tm_filename != a2.tm_filename)
    return false;
  if (a1.output_filename != a2.output_filename)
    return false;
  if (a1.HL != a2.HL)
    return false;
  if (a1.DL != a2.DL)
    return false;
  if (a1.iters != a2.iters)
    return false;
  if (a1.method != a2.method)
    return false;
  if (a1.touch_random_seed != a2.touch_random_seed)
    return false;
  if (a1.AL != a2.AL)
    return false;
  if (a1.step != a2.step)
    return false;

  return true;
}

arguments
process_args(int argc, char * argv[])
{
  arguments result;

  try
    {
      po::options_description opts("OPUS options");
      opts.add_options()
        ("help,h", "produce help message")

        ("analysis,A", "solve with analysis")

        ("simulation,S", "solve with simulation")

        ("srand",
         "seed with time the random number generator for simulation")

        ("AL", po::value<int>(),
         "the average limit, default value is the hop limit")

        ("HL", po::value<int>(), "the hop limit")

        ("DL", po::value<int>(), "the distance limit")

        ("iters,i", po::value<int>()->default_value(10),
         "the number of iterations")

        ("step,s", po::value<int>()->default_value(0),
         "the step for saving results")

        ("topology,t", po::value<string>(),
         "name of the Graphviz file with topology")

        ("traffic_matrix,m", po::value<string>(),
         "name of the file with traffic matrix")

        ("output,o", po::value<string>(),
         "name of the output file");

      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).options(opts).run(), vm);
      po::notify(vm);

      if (vm.count("help"))
        {
          cout << opts << "\n";
          exit(0);
        }

      if(vm.count("topology"))
        result.graphviz_filename = vm["topology"].as<string>();
      else
        {
          cerr << "You need to give me a graphviz file.\n";
          exit(1);
        }

      if(vm.count("traffic_matrix"))
        result.tm_filename = vm["traffic_matrix"].as<string>();
      else
        {
          cerr << "You need to give me a file with traffic matrix.\n";
          exit(1);
        }

      if(vm.count("output"))
        result.output_filename = vm["output"].as<string>();

      if (vm.count("analysis") && vm.count("simulation"))
        {
          cerr << "You can choose only one of '-A' or '-S'.\n";
          exit(1);
        }

      if (!vm.count("analysis") && !vm.count("simulation"))
        {
          cerr << "You have to choose either '-A' or '-S'.\n";
          exit(1);
        }

      if (vm.count("analysis"))
        result.method = analysis;

      if (vm.count("simulation"))
        result.method = simulation;

      result.touch_random_seed = vm.count("srand");

      if (vm.count("HL"))
        result.HL = vm["HL"].as<int>();
      else
        {
          cerr << "You need to give me a HL.\n";
          exit(1);
        }

      // These are the arguments only for the analysis.
      if (result.method == analysis)
        {
          if (vm.count("AL"))
            result.AL = vm["AL"].as<int>();
          else
            {
              result.AL = result.HL;
              cerr << "Assuming the average limit to be the hop limit of "
                   << result.HL << ".\n";
            }

          result.iters = vm["iters"].as<int>();
          result.step = vm["step"].as<int>();
        }

      if (vm.count("DL"))
        result.DL = vm["DL"].as<int>();
      else
        {
          cerr << "You need to give me a DL.\n";
          exit(1);
        }

      if (result.HL <= 0)
        {
          cerr << "You gave me a wrong HL.  I need a positive HL.\n";
          exit(1);
        }

      if (result.DL <= 0)
        {
          cerr << "You gave me a wrong DL.  I need a positive DL.\n";
          exit(1);
        }
    }
  catch(const std::exception& e)
    {
      cerr << e.what() << std::endl;
      exit(1);
    }

  return result;
}
