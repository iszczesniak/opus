#include "analysis.hpp"
#include "distro.hpp"
#include "graph.hpp"
#include "graph_serialization.hpp"
#include "nodistro.hpp"
#include "packet.hpp"
#include "poisson.hpp"
#include "sparse_matrix.hpp"
#include "tabdistro.hpp"
#include "test.hpp"
#include "utils.hpp"

#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/graph/adj_list_serialize.hpp>

using namespace std;

int
main()
{
  // This is the test for the serialization of the sparse_matrix.
  {
    sparse_matrix<int, int> m1, m2;

    m1[0][0] = 1;
    m1[0][1] = 2;
    m1[1][0] = 3;
    m1[1][1] = 4;
    m1[3][0] = 5;

    stringstream s;
    boost::archive::text_oarchive oa(s);
    oa << m1;

    boost::archive::text_iarchive ia(s);
    ia >> m2;

    TEST(m1 == m2);
    EXPECT(m1[0][0], 1);
    EXPECT(m1[0][1], 2);
    EXPECT(m1[1][0], 3);
    EXPECT(m1[1][1], 4);
    EXPECT(m1[3][0], 5);
  }

  // This is the serialization test of a small ptm.
  {
    stringstream s;

    Graph g(2);
    Edge e = add_edge(0, 1, g).first;

    distro d(poisson(1.2345));

    dist_poly dp;
    dp[0] = d;

    packet_trajectory pt;
    pt[0][e] = dp;

    pt_matrix ptm;
    ptm[0][1] = pt;

    p_graph = &g;
    boost::archive::text_oarchive oa(s);
    oa << ptm;

    pt_matrix ptm2;
    boost::archive::text_iarchive ia(s);
    ia >> ptm2;

    TEST(ptm == ptm2);
  }

  // This is the test for the serialization of the graph.  This is an
  // elaborate test for a live graph.
  {
    stringstream s;

    const char *dot_name = "../runs/six_node_net.dot";
    const char *tm_name = "../runs/six_node_net.tm";
    int HL = 5;
    int DL = 1000;

    // Here we create the graph.
    Graph g1;
    pt_matrix ptm1;
    TEST(read_graphviz_filename(dot_name, g1));
    complete_graph(g1);
    map<string, Vertex> vn = get_vertex_names(g1);

    // Here we create the traffic matrix.
    fp_matrix tm;
    TEST(load_tm_file(tm_name, vn, tm));

    // Here we run the analysis.  This is the test of packet
    // trajectory matrix that uses the poisson distribution.
    pp_matrix ppm;
    stringstream devnull;
    tie(ppm, ptm1) = ana_solution(g1, tm, HL, DL, 5, 5, devnull);

    // Here we set the pointer to the graph that should be used for
    // writing the objects.
    boost::archive::text_oarchive oa(s);
    oa << g1;
    p_graph = &g1;
    oa << ptm1;

    Graph g2;
    pt_matrix ptm2;
    // Here we set the pointer to the graph that should be used for
    // reading the objects.
    boost::archive::text_iarchive ia(s);
    ia >> g2;
    p_graph = &g2;
    ia >> ptm2;

    TEST(test_graphs(g1, g2));
    TEST(ptm1 == ptm2);

    // Iterate over the first index.
    TEST(ptm1.size() == ptm2.size());
    for(int i = 0; i < ptm1.size(); ++i)
      {
        // Iterate over the second index.
        TEST(ptm1[i].size() == ptm2[i].size());
        for(int j = 0; j < ptm1[i].size(); ++j)
          {
            // Iterate over the hops.
            TEST(ptm1[i][j].size() == ptm2[i][j].size());
            for(int t = 0; t < ptm1[i][j].size(); ++t)
              {
                // Iterate over the mapping elements
                TEST(ptm1[i][j][t].size() == ptm2[i][j][t].size());

                map<Edge, dist_poly>::iterator i1 = ptm1[i][j][t].begin();
                map<Edge, dist_poly>::iterator e1 = ptm1[i][j][t].end();
                map<Edge, dist_poly>::iterator i2 = ptm2[i][j][t].begin();
                map<Edge, dist_poly>::iterator e2 = ptm2[i][j][t].end();

                while(i1 != e1 && i2 != e2)
                  {
                    TEST(i1->first == i2->first);
                    TEST(i1->second == i2->second);
                    ++i1;
                    ++i2;
                  }

                TEST(i1 == e1);
                TEST(i2 == e2);
              }
          }
      }
  }

  // This is the test for the serialization of the distro that has a
  // nodistro, poisson or tabdistro.
  {
    stringstream s;

    distro a1;
    distro a2(poisson(1));
    tabdistro td;
    td.set_prob(prob_pair(0.75, 8));
    distro a3(td);
    boost::archive::text_oarchive oa(s);
    oa << a1;
    oa << a2;
    oa << a3;

    distro b1;
    distro b2;
    distro b3;
    boost::archive::text_iarchive ia(s);
    ia >> b1;
    ia >> b2;
    ia >> b3;
    EXPECT(b3.get_prob(8), 0.75);
    EXPECT(b3.get_prob(0), 0.25);
    EXPECT(b3.exists_ith_max(1), true);
    EXPECT(b3.exists_ith_max(2), false);

    TEST(a1 == b1);
    TEST(a2 == b2);
    TEST(a3 == b3);
  }

  return 0;
}
