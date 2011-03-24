#ifndef ARR_QUEUE_HPP
#define ARR_QUEUE_HPP

#include "config.hpp"
#include "distro.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <vector>

/**
 * The queue of the most probable arrangements.
 */
class arr_queue
{
  friend ostream & operator << (ostream &out, const arr_queue &q);

  /**
   * Keeps track of the size of the queue.
   */
  int size;

  /**
   * The probability of the most probable arrangement.
   */
  double max_prob;

  /**
   * The vector of distributions for each group.
   */
  vector<distro> distros;

  /**
   * This is the queue of waiting arrangements.
   *
   * The key of the map is the probability of an arrangement, and the
   * value is the set of arrangements with that probability.  We need
   * a set, because there can be various arrangements with the same
   * probability.
   *
   * We sort the map with decreasing probabilities, and that's why we
   * use the "greater" class.
   */
  typedef map<double, set<vector<int> >, greater<double> > queue_type;

  /**
   * The queue of arrangements from which the most probable
   * arrangement is chosen.
   */
  queue_type q;

  /**
   * When we consider packet arrangements, there is one the most
   * probable.  We don't considering an arrangement for which the
   * ratio of its probability and the maximal probability is below
   * this value.
   */
  double ratio;

  /**
   * The default ratio.
   */
  const static double default_ratio = 1e-2;

public:

  /**
   * The constructor only needs the distributions.
   *
   * @param d the vector of distributions for each group
   */
  arr_queue(const vector<distro> &d);

  /**
   * This function tries to find the next most probable arrangement of
   * packets.  Not only it returns the most probable arrangement, but
   * also replenishes the queue with new arrangements.
   *
   * @param next_arr keeps the next arrangement
   *
   * @param prob keeps the probability of the next arrangement
   *
   * @return false if there is no other arrangement
   */
  bool
  find_next(vector<int> &next_arr, double &prob);

  /**
   * Sets the ratio for which the further arrangements are not
   * returned.
   */
  void
  set_ratio(double ratio);

  /**
   * Returns the current size of the queue.
   */
  int
  get_size() const;

private:

  /**
   * Pushes the arrangement into the queue.
   *
   * @param the arrangement to push
   */
  void
  push(const vector<int> &arr);

  /**
   * Pops an arrangement from the queue.
   *
   * @param next_arr keeps the arrangement popped
   */
  void
  pop(vector<int> &next_arr);

  /**
   * This function calculates the probability of the given arrangement
   * of packets of k groups.  The probability distribution of each
   * group is given by the distros vector of size k.  The specific
   * arrangement is given by the arr vector also of size k.
   *
   * @param arr describes the arrangement; arr[i] says how many
   * packets there are that belong to group i, which are described by
   * the distros[i] probability distribution
   *
   * @return probability of arrangement
   */
  double
  calc_arr_prob(const vector<int> &arr);
};

/**
 * Prints the queue to stream.
 */
ostream &
operator << (ostream &out, const arr_queue &q);

#endif /* ARR_QUEUE_HPP */
