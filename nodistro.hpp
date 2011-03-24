#ifndef NODISTRO_HPP
#define NODISTRO_HPP

#include "counter.hpp"
#include "distro_base.hpp"

#include <deque>
#include <iostream>
#include <string>
#include <utility>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

using namespace std;

/**
 * This distribution returns probability 1 for value 0, and 0 for all
 * other values.  It's the null distribution.
 */
class nodistro : public distro_base, private counter<nodistro>
{
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    boost::serialization::base_object<distro_base>(*this);
  }

public:
  bool exists_ith_max(int i) const;

  nodistro *clone() const;

  prob_pair get_ith_max(int i) const;

  double get_prob(int i) const;

  string to_tabular() const;

  double mean() const;

  void output(ostream &out) const;

  bool operator == (const nodistro &) const;

  using counter<nodistro>::how_many;
};

#endif /* NODISTRO_HPP */
