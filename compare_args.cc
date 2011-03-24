#include "compare_args.hpp"
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

compare_args
process_compare_args(int argc, char *argv[])
{
  compare_args result;

  try{
    po::options_description opts("Compare options");
    opts.add_options()
      ("help,h", "produce help message")

      ("error-mean", "show only the error mean")

      ("error-sdev", "show only the error standard deviation")

      ("file-name", po::value<vector<string> >(), "input file");

    po::positional_options_description p;
    p.add("file-name", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(opts).positional(p).run(), vm);
    po::notify(vm);

    result.error_mean = vm.count("error-mean");
    result.error_sdev = vm.count("error-sdev");

    // Show all if nothing was selected.
    if (!result.error_mean && !result.error_sdev)
      {
	result.show_others = true;
	result.error_mean = true;
	result.error_sdev = true;
      }
    else
      result.show_others = false;

    if (vm.count("help"))
      {
        cout << opts << "\n";
        exit(0);
      }

    if (vm.count("file-name"))
      result.file_names = vm["file-name"].as<vector<string> >();
  }
  catch(const std::exception& e)
    {
      cerr << e.what() << std::endl;
    }

  return result;
}
