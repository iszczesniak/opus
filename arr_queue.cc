#include "arr_queue.hpp"
#include "utils.hpp"

#include <cassert>
#include <sstream>

using namespace std;

arr_queue::arr_queue(const vector<distro> &d) :
  distros(d), ratio(default_ratio), size(0)
{
  // Initialize the queue with the most probable arrangement.
  vector<int> first_arr(d.size(), 0);
  max_prob = calc_arr_prob(first_arr);
  push(first_arr);
}

bool
arr_queue::find_next(vector<int> &arr, double &prob)
{
  // Return with false if the queue is empty.
  if (q.empty())
    return false;

  // Pop the most probable arrangement and calculate its probability.
  pop(arr);
  prob = calc_arr_prob(arr);

  // Return no more arrangements, when this one is already below the
  // cut off ratio.
  double ratio = prob / max_prob;

  if (ratio < ratio)
    return false;

  for(int i = 0; i < arr.size(); ++i)
    {
      // This is the new arrangement.
      vector<int> n = arr;
      ++n[i];

      // We push the new arrangement into the queue only if the
      // distribution distros[i] has n[i]-th largest value.
      if (distros[i].exists_ith_max(n[i]))
        push(n);
    }

  return true;
}

void
arr_queue::push(const vector<int> &arr)
{
  assert(arr.size() == distros.size());
  double prob = calc_arr_prob(arr);
  q[prob].insert(arr);
  ++size;
}

void
arr_queue::pop(vector<int> &arr)
{
  assert(!q.empty());

  // This is the pair which has the largest value.
  queue_type::iterator c = q.begin();

  // The set of arrangements cannot be empty.
  assert(c->second.size());

  // This is the first arrangement in this set of arrangements.
  set<vector<int> >::iterator ai = c->second.begin();

  // This is the arrangement that we return.
  arr = (*ai);

  // Remove the processed arrangement.
  c->second.erase(ai);

  // Erase the value if its set is empty.
  if (c->second.empty())
    q.erase(c);

  --size;
}

double
arr_queue::calc_arr_prob(const vector<int> &arr)
{
  assert(distros.size() == arr.size());

  double prob = 1;

  for(unsigned i = 0; i < arr.size(); ++i)
    prob *= distros[i].get_ith_max(arr[i]).first;

  return prob;
}

void
arr_queue::set_ratio(double ratio)
{
  arr_queue::ratio = ratio;
}

int
arr_queue::get_size() const
{
  return size;
}

ostream &
operator << (ostream &out, const arr_queue &q)
{
  for(arr_queue::queue_type::const_iterator
        i = q.q.begin(); i != q.q.end(); ++i)
    {
      out << "Arrangements with probability " << i->first << ": ";
      for(arr_queue::queue_type::mapped_type::const_iterator
            j = i->second.begin(); j != i->second.end(); ++j)
        out << "<" << *j << ">" << ", ";
      out << endl;
    }

  return out;
}
