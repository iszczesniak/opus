#include "graph.hpp"
#include "polynomial.hpp"
#include "utils_sim.hpp"
#include "test.hpp"

#include <map>

using namespace std;


int
main()
{  
  // The simple test for an empty pqv.
  {
    map<Vertex, waiting_pkts> pqv;
    EXPECT(calc_nr_in_transit(pqv, 0, 0), 0);
  }

  // Exactly one packet goes to local drop.
  {
    map<Vertex, waiting_pkts> pqv;

    // The source is 0, the destination is 1, the time slot is 10.
    packet p1(0, 1, 10);
    
    // This packet is sheduled to arrive at node 1.
    pqv[1].insert(&p1);
    
    // The number of packets should be 0, because there's only one
    // packet that will go to the local drop.
    EXPECT(calc_nr_in_transit(pqv, 1, 10), 0);
  }


  // Exactly one packet is in transit.
  {
    map<Vertex, waiting_pkts> pqv;

    // The source is 0, the destination is 2, the time slot is 10.
    packet p1(0, 2, 10);
    
    // This packet is sheduled to arrive at node 1.
    pqv[1].insert(&p1);
    
    // We should get exactly one packet in transit that arrives at node
    // 1 during the time slot 10 .
    EXPECT(calc_nr_in_transit(pqv, 1, 10), 1);

    // No packet arrives one time slot before.
    EXPECT(calc_nr_in_transit(pqv, 1, 9), 0);

    // No packet arrive at some other node.
    EXPECT(calc_nr_in_transit(pqv, 0, 10), 0);
  }

  // Exactly one packet is in transit, and exactly one goes to the
  // local drop.
  {
    map<Vertex, waiting_pkts> pqv;

    // This packet goes to the local drop: the source is 0, the
    // destination is 1, the time slot is 10.
    packet p1(0, 1, 10);
    // This packet is sheduled to arrive at node 1.
    pqv[1].insert(&p1);

    // This packet is in transit: the source is 0, the destination is
    // 2, the time slot is 10.
    packet p2(0, 2, 10);
    // This packet is sheduled to arrive at node 1.
    pqv[1].insert(&p2);
    
    // We should get exactly one packet in transit that arrives at
    // node 1 during the time slot 10 .
    EXPECT(calc_nr_in_transit(pqv, 1, 10), 1);

    // No packet arrives one time slot before.
    EXPECT(calc_nr_in_transit(pqv, 1, 9), 0);

    // No packet arrives at node 0.
    EXPECT(calc_nr_in_transit(pqv, 0, 10), 0);

    // No packet arrives at node 2.
    EXPECT(calc_nr_in_transit(pqv, 2, 10), 0);
  }

  // Test count_poly.
  {
    count_map_poly cmp;
    cmp[0][1] = 9;
    cmp[0][2] = 1;
    cmp[3][1] = 2;
    
    dist_poly dp;
    cmp2dp(cmp, dp, 10);

    // Test the distro for the polynomial term x^0.
    EXPECT(dp[0].get_ith_max(0).first, 0.9);
    EXPECT(dp[0].get_ith_max(0).second, 1);
    EXPECT(dp[0].get_ith_max(1).first, 0.1);
    EXPECT(dp[0].get_ith_max(1).second, 2);
    EXPECT(dp[0].exists_ith_max(2), false);

    // Test the distro for the polynomial term x^3.
    EXPECT(dp[3].get_ith_max(0).first, 0.8);
    EXPECT(dp[3].get_ith_max(0).second, 0);
    EXPECT(dp[3].get_ith_max(1).first, 0.2);
    EXPECT(dp[3].get_ith_max(1).second, 1);
    EXPECT(dp[3].exists_ith_max(2), false);
  }

  // Test the ppcm2ppm function.
  {
    count_map_poly cmp;
    cmp[0][1] = 9;
    cmp[0][2] = 1;
    cmp[3][1] = 2;
    
    ppcm_matrix ppcmm;
    
    // This is the packet presence count matrix for the demand from
    // vertex 2 to vertex 1 during hop 5 at vertex 1.
    ppcmm[2][1][5][1] = cmp;
    
    pp_matrix ppm;
    ppcmm2ppm(ppcmm, ppm, 10);

    dist_poly dp = ppm[2][1][5][1];

    // Test the distro for the polynomial term x^0.
    EXPECT(dp[0].get_ith_max(0).first, 0.9);
    EXPECT(dp[0].get_ith_max(0).second, 1);
    EXPECT(dp[0].get_ith_max(1).first, 0.1);
    EXPECT(dp[0].get_ith_max(1).second, 2);
    EXPECT(dp[0].exists_ith_max(2), false);

    // Test the distro for the polynomial term x^3.
    EXPECT(dp[3].get_ith_max(0).first, 0.8);
    EXPECT(dp[3].get_ith_max(0).second, 0);
    EXPECT(dp[3].get_ith_max(1).first, 0.2);
    EXPECT(dp[3].get_ith_max(1).second, 1);
    EXPECT(dp[3].exists_ith_max(2), false);
  }

  return 0;
}
