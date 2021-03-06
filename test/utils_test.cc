#include "matrixes.hpp"
#include "poisson.hpp"
#include "tabdistro.hpp"
#include "test.hpp"
#include "utils.hpp"
#include "utils_netgen.hpp"

#include <gsl/gsl_randist.h>

// This tests the utilities from utils.hpp.

using namespace std;

int
main()
{
  // This is the test for various graphs utilities.
  {
    Graph g;

    // Make sure that reading the file works.
    EXPECT(read_graphviz_filename("net.dot", g), true);

    // Make sure that the name properties are fine and that
    // the names are mapped to vertexes.
    map<string, Vertex> names = get_vertex_names(g);
    EXPECT(names["a"], 0);
    EXPECT(names["b"], 1);
    EXPECT(names["c"], 2);
    EXPECT(names["d"], 3);
    EXPECT(names["e"], 4);
    EXPECT(names["f"], 5);

    // Make sure that the vertex_distance attribute is OK.
    EXPECT(get(vertex_distance, g, names["d"])[names["a"]], 380);

    // Make sure that reading in the traffic matrix works.  Here the
    // traffic matrix is just a matrix of real numbers.
    fp_matrix tm;
    EXPECT(load_tm_file("net.tm", names, tm), true);
    double load;

    load = tm[names["b"]][names["c"]];
    TEST(load > 2.9);
    TEST(load < 3.1);

    load = tm[names["e"]][names["d"]];
    TEST(load > 1.9);
    TEST(load < 2.1);

    load = tm[names["c"]][names["f"]];
    TEST(load > 0.4);
    TEST(load < 0.6);

    // Test the get_output_capacity function.
    EXPECT(get_output_capacity(g, names["a"]), 16);
  }

  // Test the sum function for tabdistro.
  {
    tabdistro t1;
    t1.set_prob(make_pair(0.5, 2));

    tabdistro t2;
    t2.set_prob(make_pair(0.5, 2));

    dist_poly dp;
    dp[1] = t1;
    dp[2] = t2;
    EXPECT(dp.size(), 2);
    EXPECT(dp[1].mean(), 1.0);
    EXPECT(dp[2].mean(), 1.0);

    distro s = sum(dp);
    EXPECT(s.mean(), 2.0);
  }

  // Test the function get_vertexes.
  {
    Graph g;
    set<Vertex> sov = get_vertexes<set<Vertex> >(g);
    EXPECT(sov.size(), 0);
  }

  // Test the function get_vertexes.
  {
    Graph g(10);
    set<Vertex> sov = get_vertexes<set<Vertex> >(g);
    EXPECT(sov.size(), 10);
    for (int i = 1; i < sov.size(); ++i)
      {
        set<Vertex>::iterator it = sov.begin();
        advance(it, i);
        EXPECT(*it, i);
      }
  }

  // Test the function get_random_element.
  {
    set<int> s;
    int sum = 0;
    minstd_rand gen;

    for (int i = 0; i < 100; ++i)
      {
        s.insert(i);
        sum += i;
      }

    for (int i = 0; i < 100; ++i)
      {
        int e = get_random_element(s, gen);
        s.erase(e);
        sum -= e;
      }

    EXPECT(sum, 0);
  }

  // Test the function get_random_element.
  {
    set<int> s;
    minstd_rand gen;

    for (int i = 0; i < 1000; ++i)
      s.insert(i);

    // The average value of a random element should be about 500.
    int sum = 0;
    for (int i = 0; i < 100; ++i)
      {
        int e = get_random_element(s, gen);
        s.erase(e);
        sum += e;
      }
    sum /= 100;

    TEST(sum > 400);
    TEST(sum < 600);
  }

  // Test the function generate_graph, and make sure it produces only
  // one connected component.
  {
    minstd_rand gen;
    Graph g;

    for(int n = 3; n < 50; ++n)
      {
        int e = n * (n - 1) / 2;
        EXPECT(generate_graph(g, n, e - 1, gen), e - 1);
        TEST(check_components(g));
        EXPECT(generate_graph(g, n, e, gen), e);
        TEST(check_components(g));
        EXPECT(generate_graph(g, n, e + 1, gen), e);
        TEST(check_components(g));
      }
  }

  // Test the function get_components.
  {
    Graph g(6);
    add_edge(0, 1, g);
    add_edge(0, 2, g);
    add_edge(3, 4, g);

    list<set<Vertex> > c = get_components(g);
    list<set<Vertex> >::iterator i = c.begin();

    EXPECT(c.size(), 3);
    EXPECT(i->size(), 3);
    ++i;
    EXPECT(i->size(), 2);
    ++i;
    EXPECT(i->size(), 1);
  }

  return 0;
}
