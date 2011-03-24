#include "geometric.hpp"

#include <gsl/gsl_randist.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(geometric)

using namespace std;

geometric::geometric() : p(0)
{
}

geometric::geometric(double p) : p(p)
{
}

geometric *
geometric::clone() const
{
  return new geometric(*this);
}

bool
geometric::exists_ith_max(int i) const
{
  return true;
}

prob_pair
geometric::get_ith_max(int i) const
{
  prob_pair p(get_prob(i + 1), i + 1);
  return p;
}

double
geometric::get_prob(int i) const
{
  assert(i >= 1);

  return gsl_ran_geometric_pdf(i, p);
}

string
geometric::to_tabular() const
{
  map<int, double> val2prob;

  prob_pair pr = get_ith_max(0);
  double max_prob = pr.first;
  val2prob[pr.second] = pr.first;

  int i_max = 0;

  for(int i = 1;
      pr = get_ith_max(i), pr.first / max_prob > GEOMETRIC_PRINT_CUT_OFF; ++i)
    {
      val2prob[pr.second] = pr.first;
      i_max = max(i_max, pr.second);
    }

  // Here we produce the string.
  std::ostringstream str;
  for(int i = 0; i <= i_max; ++i)
    str << i << " " << val2prob[i] << endl;

  return str.str();
}

void
geometric::output(ostream &out) const
{
  out << "geometric(" << p << ")";
}

double
geometric::mean() const
{
  return p;
}

bool
geometric::operator == (const geometric &d) const
{
  return p == d.p;
}
