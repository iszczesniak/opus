#ifndef DISTRO_BASE_HPP
#define DISTRO_BASE_HPP

#include "counter.hpp"

#include <string>
#include <utility>

using namespace std;

/**
 * The definition of the probability pair.  The first field is the
 * probability that the random variable will take the value of the
 * pair's second field.
 */
typedef pair<double, int> prob_pair;

/**
 * The base class for probability distributions.
 */
class distro_base : private counter<distro_base>
{
public:
  /**
   * We need to make it virtual, so that derived objects are destroyed
   * appropriately.
   */
  virtual ~distro_base() {}

  /**
   * Clones the distribution.
   */
  virtual distro_base *clone() const = 0;

  /**
   * Lets you know whether the distribution has the i-th most probable
   * value.  We start counting i from 0.
   *
   * @param i the number of the i-th most probable value
   *
   * @return true if the i-th most probable value exists
   */
  virtual bool exists_ith_max(int i) const = 0;

  /**
   * Returns i-th pair, where get(i).first is the i-th largest
   * probability, and get(i).second is the value of the random
   * variable for which we get this probability.  We start counting i
   * from 0.
   *
   * @param i the number of the i-th most probable value
   */
  virtual prob_pair get_ith_max(int i) const = 0;

  /**
   * Returns the probability for the given value of the random
   * variable.
   *
   * @param i the value of the random variable
   *
   * @return the probability for this value
   */
  virtual double get_prob(int i) const = 0;

  /**
   * See distro::to_tabular.
   */
  virtual string to_tabular() const = 0;

  /**
   * Returns the mean of the distribution.
   */
  virtual double mean() const = 0;

  /**
   * Outputs the distro info into a stream.
   */
  virtual void output(ostream &out) const = 0;

  /**
   * Returns the number of alive objects.
   */
  using counter<distro_base>::how_many;
};

#endif /* DISTRO_BASE_HPP */
