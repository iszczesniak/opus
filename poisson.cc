#include "poisson.hpp"

#include <gsl/gsl_randist.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(poisson)

using namespace std;

poisson::poisson() : lambda(1)
{
}

poisson::poisson(double l) : lambda(l)
{
  int k = mode(lambda);
  double f = gsl_ran_poisson_pdf(k, lambda);

  // This is the maximum.  We can't get a bigger value than this one.
  // We can at most get an equal value.
  distro.push_back(make_pair(f, k));

  k_left = k - 1;
  k_right = k + 1;
}

poisson *
poisson::clone() const
{
  return new poisson(*this);
}

bool
poisson::exists_ith_max(int i) const
{
  return true;
}

prob_pair
poisson::get_ith_max(int i) const
{
  while(distro.size() <= i)
    figure_out_next();

  return distro[i];
}

double
poisson::get_prob(int i) const
{
  assert(i >= 0);

  return gsl_ran_poisson_pdf(i, lambda);
}

string
poisson::to_tabular() const
{
  map<int, double> val2prob;

  prob_pair pr = get_ith_max(0);
  double max_prob = pr.first;
  val2prob[pr.second] = pr.first;

  int i_max = 0;

  for(int i = 1;
      pr = get_ith_max(i), pr.first / max_prob > POISSON_PRINT_CUT_OFF; ++i)
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
poisson::output(ostream &out) const
{
  out << "poisson(" << lambda << ")";
}

double
poisson::mean() const
{
  return lambda;
}

int
poisson::mode(double lambda)
{
  assert(lambda > 0);

  // The Poisson distribution has the maximum between (lambda - 1) and
  // lambda, because poisson(x, lambda) = poisson(x + 1, lambda) gives
  // the result x = lambda - 1.  We look for two equal probability
  // values that are distant from one another by 1.  This give as a
  // pretty good clue where the maximum is.
  //
  // Since lambda doesn't have to be an integer, we have to consider
  // consider floor(lambda - 1), and ceil(lambda) as the possible
  // values for the maximum.  Also (floor(lambda - 1) + 1) can be the
  // maximum, so we consider this too.  It seems safe to assume that
  // there are three values for the maximum to consier:
  //
  // int(lambda - 1)
  // int(lambda - 1) + 1
  // int(lambda - 1) + 2
  //
  // But we need to make sure that (lambda - 1) is not negative.

  int k_ini = max(int(lambda - 1), 0);
  int mode = k_ini;
  double f_max = gsl_ran_poisson_pdf(mode, lambda);

  // We choose the max of k_ini, (k_ini + 1), and (k_ini + 2).
  for(int k = k_ini + 1; k <= k_ini + 2; ++k)
    {
      double f = gsl_ran_poisson_pdf(k, lambda);

      if (f > f_max)
        {
          f_max = f;
          mode = k;
        }
    }

  return mode;
}

void
poisson::figure_out_next() const
{
  int k;

  if (k_left < 0)
    {
      // This is the case when on the left of the Poisson distribution
      // there are no more numbers left.
      k = k_right;
      ++k_right;
    }
  else
    {
      double f_left = gsl_ran_poisson_pdf(k_left, lambda);
      double f_right = gsl_ran_poisson_pdf(k_right, lambda);

      if (f_left >= f_right)
        {
          k = k_left;
          --k_left;
        }
      else
        {
          k = k_right;
          ++k_right;
        }
    }

  double f = gsl_ran_poisson_pdf(k, lambda);
  distro.push_back(make_pair(f, k));
}

bool
poisson::operator == (const poisson &d) const
{
  return lambda == d.lambda;
}
