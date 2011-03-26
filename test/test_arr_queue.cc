#include "arr_queue.hpp"
#include "distro.hpp"
#include "nodistro.hpp"
#include "geometric.hpp"
#include "test.hpp"
#include "utils.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/error_of_mean.hpp>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include <boost/timer.hpp>

#include <boost/tuple/tuple.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace boost;
using namespace accumulators;

/**
 * These are results for a single test.  The first is the aggregate
 * probability, the second is the number of considered arrangements,
 * the third is the time taken, and the fourth is the queue size.
 */
typedef tuple<double, int, double, int> test_results;

/**
 * These are average results for a sample.
 */
class sample_results
{
public:
  /**
   * Cumulative probability.
   */
  pair<double, double> probability;
  /**
   * Number of considered arrangements.
   */
  pair<double, double> arrangements;
  /**
   * Time taken.
   */
  pair<double, double> time;
  /**
   * Queue size.
   */
  pair<double, double> size;
};

class condition
{
public:
  virtual string to_string() const = 0;
};

/**
 * Gives the limit on the number of considered arrangements.
 */
class arra_condition : public condition
{
  int number;

public:
  arra_condition(int number) : number(number) {}

  double get_number() const
  {
    return number;
  }

  string to_string() const
  {
    ostringstream out;
    out << "number of arrangements of " << number;
    return out.str();
  }
};

class rati_condition : public condition
{
  double ratio;

public:
  rati_condition(double ratio) : ratio(ratio) {}

  double get_ratio() const
  {
    return ratio;
  }

  string to_string() const
  {
    ostringstream out;
    out << "ratio of " << ratio;
    return out.str();
  }
};

class time_condition : public condition
{
  double time;

public:
  time_condition(double time) : time(time) {}

  double get_time() const
  {
    return time;
  }

  string to_string() const
  {
    ostringstream out;
    out << "maximal time of " << time << " seconds";
    return out.str();
  }
};

class prob_condition: public condition
{
  double prob;

public:
  prob_condition(double prob) : prob(prob) {}

  double get_prob() const
  {
    return prob;
  }

  string to_string() const
  {
    ostringstream out;
    out << "cumulative probability of at least " << prob;
    return out.str();
  }
};

/**
 * This is a problem, because the queue size might not reach that
 * limit, and the code just doesn't stop.
 */
class size_condition: public condition
{
  int size;

public:
  size_condition(double size) : size(size) {}

  double get_size() const
  {
    return size;
  }

  string to_string() const
  {
    ostringstream out;
    out << "queue size limit of " << size;
    return out.str();
  }
};

class randis
{
public:
  virtual distro pick() const = 0;

  virtual string to_string() const = 0;
};

template<typename T>
class random_poisson : public randis
{
  T &gen;
  double min, max;

public:
  random_poisson(T &gen, double min, double max) : gen(gen), min(min), max(max) {}

  distro pick() const
  {
    boost::uniform_real<> dist(min, max);
    boost::variate_generator<minstd_rand &, uniform_real<> > g(gen, dist);
    double rate = g();
    return poisson(rate);
  }

  string to_string() const
  {
    ostringstream out;
    out << "random poisson with min = " << min << ", max = " << max;
    return out.str();
  }
};

template<typename T>
class random_geometric : public randis
{
  T &gen;
  double min, max;

public:
  random_geometric(T &gen, double min, double max) : gen(gen), min(min), max(max) {}

  distro pick() const
  {
    boost::uniform_real<> dist(min, max);
    boost::variate_generator<minstd_rand &, uniform_real<> > g(gen, dist);
    double rate = g();
    return geometric(rate);
  }

  string to_string() const
  {
    ostringstream out;
    out << "random geometric with min = " << min << ", max = " << max;
    return out.str();    
  }
};

/**
 * Calculates the cummulative probability of all considered
 * arrangements, and the number of considered arrangements.
 */
test_results
for_a_test(const vector<distro> &distros, const condition &sc)
{
  test_results tr;

  arr_queue q(distros);

  const arra_condition *acp = dynamic_cast<const arra_condition *>(&sc);
  const prob_condition *pcp = dynamic_cast<const prob_condition *>(&sc);
  const rati_condition *rcp = dynamic_cast<const rati_condition *>(&sc);
  const size_condition *scp = dynamic_cast<const size_condition *>(&sc);
  const time_condition *tcp = dynamic_cast<const time_condition *>(&sc);

  if (rcp)
    q.set_ratio(rcp->get_ratio());
  else
    // Disable the ratio condition.
    q.set_ratio(0);

  vector<int> arr;
  double prob;

  timer t;

  while(q.find_next(arr, prob))
    {
      get<0>(tr) += prob;
      ++get<1>(tr);
      get<2>(tr);

      if (acp && get<1>(tr) > acp->get_number())
	break;	

      if (pcp && get<0>(tr) > pcp->get_prob())
	break;	

      if (scp && q.get_size() > scp->get_size())
	break;

      if (tcp && t.elapsed() > tcp->get_time())
	break;
    }

  get<2>(tr) = t.elapsed();
  get<3>(tr) = q.get_size();

  return tr;
}

sample_results
for_a_sample(int R, const randis &d, const condition &sc)
{
  accumulator_set<double, stats<tag::mean, tag::error_of<tag::mean> > >
    acc_prob;
  accumulator_set<double, stats<tag::mean, tag::error_of<tag::mean> > >
    acc_arra;
  accumulator_set<double, stats<tag::mean, tag::error_of<tag::mean> > >
    acc_time;
  accumulator_set<double, stats<tag::mean, tag::error_of<tag::mean> > >
    acc_size;

  for (int test = 0; test < 100; ++test)
    {
      vector<distro> distros;

      for (int count = 0; count < R; ++count)
        distros.push_back(d.pick());

      test_results tr = for_a_test(distros, sc);
      acc_prob(get<0>(tr));
      acc_arra(get<1>(tr));
      acc_time(get<2>(tr));
      acc_size(get<3>(tr));
    }

  sample_results result;
  result.probability.first = mean(acc_prob);
  result.probability.second = accumulators::error_of<tag::mean>(acc_prob);
  result.arrangements.first = mean(acc_arra);
  result.arrangements.second = accumulators::error_of<tag::mean>(acc_arra);
  result.time.first = mean(acc_time);
  result.time.second = accumulators::error_of<tag::mean>(acc_time);
  result.size.first = mean(acc_size);
  result.size.second = accumulators::error_of<tag::mean>(acc_size);
  return result;
}

/**
 * For a distro.
 */
void
for_a_distro(const randis &d, const condition &sc)
{
  cout << "# For a random distro: " << d.to_string() << endl;

  for (int R = 2; R <= 10; R += 2)
    {
      sample_results sr = for_a_sample(R, d, sc);
      cout << R << " "
	   << sr.probability.first << " " << sr.probability.second << " "
	   << sr.arrangements.first << " " << sr.arrangements.second << " "
	   << sr.time.first << " " << sr.time.second << " "
	   << sr.size.first << " " << sr.size.second << endl;
    }

  cout << endl << endl;
}

/**
 * For a stop condition.
 */
template<typename T>
void
for_a_sc(T &gen, const condition &sc)
{
  cout << "# For a stop condition : " << sc.to_string() << endl;

  for_a_distro(random_poisson<T>(gen, 0, 1), sc);
  for_a_distro(random_geometric<T>(gen, 0.5, 1), sc);
}

int
main()
{
  minstd_rand gen;

  for_a_sc(gen, arra_condition(1000));
  for_a_sc(gen, prob_condition(0.9));
  for_a_sc(gen, rati_condition(1e-2));
  for_a_sc(gen, size_condition(1000));
  for_a_sc(gen, time_condition(1e-2));

  return 0;
}
