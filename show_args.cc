#include "show_args.hpp"
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

show_args
process_show_args(int argc, char *argv[])
{
  show_args result;

  try
    {
      po::options_description opts("Show options");
      opts.add_options()
        ("help,h", "produce help message")

        ("overload-mean", "show the overload mean")

        ("overload-sdev", "show the overload standard deviation")

        ("load-mean", "show the load mean")

        ("load-sdev", "show the load standard deviation")

        ("plp-mean", "show the packet loss probability mean")

        ("plp-sdev", "show the packet loss probability standard deviation")

        ("file-name", po::value<string>(), "input file");

      po::positional_options_description p;
      p.add("file-name", -1);

      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).
                options(opts).positional(p).run(), vm);
      po::notify(vm);

      if (vm.count("help"))
        {
          cout << opts << "\n";
          exit(0);
        }

      // True if any of the below was set.
      bool any = false;

      // Overload
      any |= result.overload_mean = vm.count("overload-mean");
      any |= result.overload_sdev = vm.count("overload-sdev");

      // Load
      any |= result.load_mean = vm.count("load-mean");
      any |= result.load_sdev = vm.count("load-sdev");

      // PLP
      any |= result.plp_mean = vm.count("plp-mean");
      any |= result.plp_sdev = vm.count("plp-sdev");

      // Don't show others is any above was selected.
      result.show_others = !any;

      // Show all if nothing was selected.
      if (!any)
        {
          result.overload_mean = true;
          result.overload_sdev = true;
          result.load_mean = true;
          result.load_sdev = true;
          result.plp_mean = true;
          result.plp_sdev = true;
	}

      if (vm.count("file-name"))
        result.file_name = vm["file-name"].as<string>();
    }
  catch(const std::exception& e)
    {
      cerr << e.what() << std::endl;
      exit(1);
    }

  return result;
}
