#ifndef TEST_HPP
#define TEST_HPP

#include "graph.hpp"

#include <cassert>
#include <iostream>
#include <map>

#include <boost/graph/property_iter_range.hpp>

// Prints a message when the condition is false.

#define TEST(expr) (!(expr) ? std::cout << "FAILED: " << #expr << " at " << __FILE__ << ":" << __LINE__ << "\n" : std::cout)

// Prints a message when the values are not equal.

#define EXPECT(expr1, expr2) (!((expr1) == (expr2)) ? std::cout << "FAILED: expected " << expr2 << ", got " << expr1 << " at " << __FILE__ << ":" << __LINE__ << "\n" : std::cout)

/**
 * This function tests the link loads.
 */
void
test_ll(const map<Edge, double> &ll,
        const Graph &g, std::ostream &os);

/**
 * Test for parallel edges.
 */
bool
test_parallel(const Graph &g);

/**
 * Tests two graphs for being the same.
 */
bool
test_graphs(const Graph &g1, const Graph &g2);

/**
 * This function makes sure that the template property is the same for
 * both graphs.
 */
template<typename T>
bool
test_property(const Graph &g1, const Graph &g2)
{
  typename graph_property_iter_range<Graph, T>::const_iterator g1_b, g1_e;
  tie(g1_b, g1_e) = get_property_iter_range(g1, T());

  typename graph_property_iter_range<Graph, T>::const_iterator g2_b, g2_e;
  tie(g2_b, g2_e) = get_property_iter_range(g1, T());

  while(g1_b != g1_e)
    {
      if (!(*g1_b == *g2_b))
        return false;

      ++g1_b;
      ++g2_b;
    }

  assert((g1_b == g1_e) && (g2_b == g2_e));

  return true;
}

#endif /* TEST_HPP */
