#ifndef DISTRO_HPP
#define DISTRO_HPP

#include "counter.hpp"
#include "distro_base.hpp"
#include "nodistro.hpp"
#include "tabdistro.hpp"

#include <iostream>
#include <string>
#include <utility>

#include <boost/serialization/access.hpp>

using namespace std;

/**
 * The envelope class for probability distributions.
 *
 * We define it all inline to make it faster, because it's small and
 * frequently used.
 */
class distro : private counter<distro>
{
  friend class boost::serialization::access;
  friend ostream &operator << (ostream &o, const distro &b);

  /**
   * Pointer to the implementation.
   */
  distro_base *db;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & db;
  }

public:
  /**
   * The default constructor created a nodistro object.
   */
  distro()
  {
    db = new nodistro();
  }

  /**
   * Pass the distribution that you want the dist to have.  This
   * distribution is copied, and you should make sure that you destroy
   * it.
   *
   * @param db the distribution
   */
  distro(const distro_base &db)
  {
    distro::db = db.clone();
  }

  /**
   * Copy constructor.
   */
  distro(const distro &d)
  {
    distro::db = d.db->clone();
  }

  ~distro()
  {
    delete db;
  }

  /**
   * The assignment operator destroys the object currently stored, and
   * clones the right-hand side object for itself.
   */
  distro& operator=(const distro &d)
  {
    distro_base *new_db = d.db->clone();
    delete db;
    db = new_db;
    return *this;
  }

  /**
   * Lets you know whether the distribution has the i-th most probable
   * value.  We start counting i from 0.
   *
   * @param i the number of the i-th most probable value
   *
   * @return true if the i-th most probable value exists
   */
  bool exists_ith_max(int i)
  {
    return db->exists_ith_max(i);
  }

  /**
   * Returns i-th pair, where get(i).first is the i-th largest
   * probability, and get(i).second is the value of the random
   * variable for which we get this probability.  We start counting i
   * from 0.
   *
   * @param i the number of the i-th most probable value
   */
  prob_pair get_ith_max(int i)
  {
    return db->get_ith_max(i);
  }

  /**
   * Returns the probability for the given value of the random
   * variable.
   *
   * @param i the value of the random variable
   *
   * @return the probability for this value
   */
  double get_prob(int i)
  {
    return db->get_prob(i);
  }

  /**
   * Returns the string that describes the distribution in a table.
   * In each line we print "x P(x)", where x is the value of the
   * random variable, and P(x) is the probability of getting this
   * value.  For each next printed line, the x is greater.
   */
  string to_tabular() const
  {
    return db->to_tabular();
  }

  /**
   * Returns the mean of the distribution.
   */
  double mean() const
  {
    return db->mean();
  }

  /**
   * Returns true if the distribution is nodistro.
   */
  bool
  is_nodistro() const
  {
    return dynamic_cast<nodistro *>(db) != NULL;
  }

  /**
   * Returns true if the distribution is tabdistro.
   */
  bool
  is_tabdistro() const
  {
    return dynamic_cast<tabdistro *>(db) != NULL;
  }

  /**
   * Adding distros.  This function is more complicated, so we leave
   * the implementation for a separate file.
   */
  distro &operator += (const distro &d);

  /**
   * The operator for getting a new distribution of packets if packets
   * are let through with probability p.
   */
  distro &operator *= (double p);

  /**
   * The comparison operator.
   */
  bool operator == (const distro &) const;

  /**
   * Returns the number of alive objects.
   */
  using counter<distro>::how_many;
};

/**
 * The operator for getting a new distribution of packets if packets
 * are let through with probability p.
 */
distro
operator * (double p, const distro &d);

/**
 * The output operator for the distro.  It describes the distro
 * in a way that can be printed in a single line.  This is used
 * to print one polynomial in single line.
 */
ostream &
operator << (ostream &out, const distro &d);

#endif /* DISTRO_HPP */
