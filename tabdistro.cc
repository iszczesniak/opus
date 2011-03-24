#include "tabdistro.hpp"

#include <boost/foreach.hpp>

#include <cassert>
#include <iterator>
#include <map>
#include <set>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(tabdistro);

using namespace std;

tabdistro::tabdistro() {
  probs.insert(prob_pair(1.0, 0));
}

tabdistro *
tabdistro::clone() const
{
  return new tabdistro(*this);
}

size_t
tabdistro::size() const
{
  return probs.size();
}

bool
tabdistro::exists_ith_max(int i) const
{
  return i < probs.size();
}

prob_pair
tabdistro::get_ith_max(int i) const
{
  assert(i < probs.size());

  probs_cont::iterator ith_max = probs.begin();
  advance(ith_max, i);

  return *ith_max;
}

double
tabdistro::get_prob(int i) const
{
  assert(i >= 0);

  // Iterate over all elements of the probs object
  for(probs_cont::iterator ii = probs.begin(); ii != probs.end(); ++ii)
    if (ii->second == i)
      return ii->first;

  return 0;
}

bool
tabdistro::set_prob(const prob_pair &p)
{
  // Before we insert the pair, we need to see whether p.second
  // already isn't stored.
  BOOST_FOREACH(probs_cont::value_type i, probs)
    if (p.second == i.second)
      return false;

  if (!probs.insert(p).second)
    return false;

  sanitize();

  return true;
}

string
tabdistro::to_tabular() const
{
  ostringstream str;

  map<int, double> val2prob;

  int i_max = -1;

  BOOST_FOREACH(probs_cont::value_type p, probs)
    {
      val2prob[p.second] = p.first;
      i_max = max(i_max, p.second);
    }

  for(int i = 0; i <= i_max; ++i)
    str << i << " " << val2prob[i] << endl;

  return str.str();
}

void
tabdistro::output(ostream &out) const
{
  out << "tabdistro(" << mean() << ")";
}

double
tabdistro::mean() const
{
  double avg = 0;

  BOOST_FOREACH(probs_cont::value_type p, probs)
    avg += p.first * p.second;

  return avg;
}

tabdistro
operator + (const tabdistro& d1, const tabdistro& d2)
{
  tabdistro ret;

  map<int, double> m;

  for(int i1 = 0; d1.exists_ith_max(i1); ++i1)
    for(int i2 = 0; d2.exists_ith_max(i2); ++i2)
      {
        prob_pair p1 = d1.get_ith_max(i1);
        prob_pair p2 = d2.get_ith_max(i2);

        m[p1.second + p2.second] += p1.first * p2.first;
      }

  for(map<int, double>::iterator mi = m.begin(); mi != m.end(); ++mi)
    ret.set_prob(prob_pair(mi->second, mi->first));

  return ret;
}

tabdistro &
operator += (tabdistro &d1, const tabdistro &d2)
{
  d1 = d1 + d2;

  return d1;
}

bool
tabdistro::operator == (const tabdistro &d) const
{
  return probs == d.probs;
}

bool
tabdistro::operator != (const tabdistro &d) const
{
  return !operator == (d);
}

void
tabdistro::sanitize() const
{
  // Find the entry for the zero.
  probs_cont::iterator i = probs.begin();
  while(i != probs.end() && i->second != 0)
    ++i;

  // Remove the entry for the zero if it was found.
  if (i != probs.end())
    probs.erase(i);

  double sum = 0;
  BOOST_FOREACH(probs_cont::value_type p, probs)
    sum += p.first;

  assert(sum <= 1.01);

  // The probability of getting zero.
  double prob = 1 - sum;
  if (prob != 0.0)
    probs.insert(prob_pair(prob, 0));
}
