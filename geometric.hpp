#ifndef GEOMETRIC_HPP
#define GEOMETRIC_HPP

#include "counter.hpp"
#include "distro_base.hpp"

#include <utility>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

/**
 * Defines the ratio of the number of events for which the probability
 * is not printed.
 */
#define GEOMETRIC_PRINT_CUT_OFF 10e-5

using namespace std;

/**
 * Implements the geometric distribution.  The random variable starts
 * from 1, not from 0.
 */
class geometric : public distro_base, private counter<geometric>
{
  friend class boost::serialization::access;

  /**
   * The probability of success.
   */
  double p;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    boost::serialization::base_object<distro_base>(*this);
    ar & p;
  }

public:
  geometric();

  geometric(double p);

  geometric *clone() const;

  bool exists_ith_max(int i) const;

  prob_pair get_ith_max(int i) const;

  double get_prob(int i) const;

  string to_tabular() const;

  void output(ostream &out) const;

  double mean() const;

  using counter<geometric>::how_many;

  bool operator == (const geometric &) const;
};

#endif /* GEOMETRIC_HPP */
