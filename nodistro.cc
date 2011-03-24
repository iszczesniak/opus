#include "nodistro.hpp"

#include <cassert>
#include <iostream>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(nodistro);

nodistro *
nodistro::clone() const
{
  return new nodistro(*this);
}

bool
nodistro::exists_ith_max(int i) const
{
  return i == 0;
}

prob_pair
nodistro::get_ith_max(int i) const
{
  assert(i == 0);

  return make_pair(1.0, 0);
}

double
nodistro::get_prob(int i) const
{
  assert(i >= 0);

  return i == 0;
}

string
nodistro::to_tabular() const
{
  return string("0 1\n");
}

void
nodistro::output(ostream &out) const
{
  out << "nodistro";
}

bool
nodistro::operator == (const nodistro &) const
{
  // All objects of nodistro are equal.
  return true;
}

double
nodistro::mean() const
{
  return 0;
}
