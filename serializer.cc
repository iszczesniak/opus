#include "serializer.hpp"
#include <fstream>
#include <sstream>

#include <boost/archive/text_oarchive.hpp>

serializer::serializer(const arguments &args,
                       const Graph &g,
                       const fp_matrix &tm) :
  args(args), g(g), tm(tm)
{
}

void
serializer::operator()(const pt_matrix &ptm) const
{
  if (!args.output_filename.empty())
    {
      ofstream out(args.output_filename.c_str());

      boost::archive::text_oarchive oa(out);
      oa << args;
      oa << g;
      oa << tm;
      oa << ptm;
    }
}

void
serializer::operator()(const pt_matrix &ptm, int iter) const
{
  if (!args.output_filename.empty())
    {
      if (args.step && !(iter % args.step))
        {
          ostringstream name;

          name << args.output_filename;
          name << "_" << iter;

          ofstream out(name.str().c_str());

          boost::archive::text_oarchive oa(out);
          oa << args;
          oa << g;
          oa << tm;
          oa << ptm;
        }
    }
}
