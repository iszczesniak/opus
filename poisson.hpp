#ifndef POISSON_HPP
#define POISSON_HPP

#include "counter.hpp"
#include "distro_base.hpp"

#include <deque>
#include <utility>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

/**
 * Defines the ratio of the number of events for which the probability
 * is not printed.
 */
#define POISSON_PRINT_CUT_OFF 10e-5

using namespace std;

class poisson : public distro_base, private counter<poisson>
{
  friend class boost::serialization::access;

  double lambda;

  /**
   * This is the cache of the known probabilities.
   */
  mutable deque<prob_pair> distro;

  /**
   * These are the smallest and largest values of k that will be
   * examined next.  The integer values between (k_left + 1) and
   * (k_right - 1) including them are already in the distro object.
   */
  mutable int k_left, k_right;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    boost::serialization::base_object<distro_base>(*this);
    ar & lambda;
  }

public:
  poisson();

  poisson(double lambda);

  poisson *clone() const;

  bool exists_ith_max(int i) const;

  prob_pair get_ith_max(int i) const;

  double get_prob(int i) const;

  string to_tabular() const;

  void output(ostream &out) const;

  double mean() const;

  using counter<poisson>::how_many;

  /**
   * Returns k for which poisson(k, lambda) is maximal.
   */
  static int mode(double lambda);

  bool operator == (const poisson &) const;

private:
  void figure_out_next() const;
};

#endif /* POISSON_HPP */
