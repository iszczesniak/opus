#ifndef TABDISTRO_HPP
#define TABDISTRO_HPP

#include "counter.hpp"
#include "distro_base.hpp"

#include <functional>
#include <iostream>
#include <set>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/utility.hpp>

using namespace std;

/**
 * This distribution is given by the user.  It's the tabular
 * distribution, i.e. it's given by a table.
 */

class tabdistro : public distro_base, private counter<tabdistro>
{
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    boost::serialization::base_object<distro_base>(*this);
    ar & probs;
  }

  // This is our sorting functor for prob_pair.
  class my_pair_order : binary_function<const prob_pair &,
                                        const prob_pair &, bool>
  {
  public:
    bool operator()(const prob_pair &p1, const prob_pair &p2)
    {
      if (p1.first == p2.first)
        return p1.second < p2.second;

      return p1.first > p2.first;
    }
  };

  typedef set<prob_pair, my_pair_order> probs_cont;

  mutable probs_cont probs;

public:
  tabdistro();

  tabdistro *clone() const;

  /**
   * Returns the number of values for the random variable.
   */
  size_t size() const;

  bool exists_ith_max(int i) const;

  prob_pair get_ith_max(int i) const;

  double get_prob(int i) const;

  /**
   * Inserts the pair to the distribution.
   *
   * @param p the pair to insert
   *
   * @return true if insertion successful
   */
  bool set_prob(const prob_pair &p);

  bool operator == (const tabdistro &) const;

  bool operator != (const tabdistro &) const;

  string to_tabular() const;

  void output(ostream &out) const;

  double mean() const;

  using counter<tabdistro>::how_many;

private:
  /**
   * This function is called by the set_prob function to make sure
   * that there's also an entry when the random variable is zero.
   */
  void sanitize() const;
};

/**
 * The operator for adding two tabdistro distributions.
 *
 * @return the distribution that is the convolution of the two
 * distributions.
 */
tabdistro
operator + (const tabdistro &d1, const tabdistro &d2);

tabdistro &
operator += (tabdistro &d1, const tabdistro &d2);

#endif /* TABDISTRO_HPP */
