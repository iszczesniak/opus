#include "generate.hpp"
#include "graph.hpp"
#include "edge_probs.hpp"
#include "matrixes.hpp"
#include "distro.hpp"
#include "nodistro.hpp"
#include "poisson.hpp"
#include "utils.hpp"
#include "utils_ana.hpp"

#include <cassert>
#include <iostream>
#include <map>

#include <boost/foreach.hpp>

void
generate_itm(const Graph &g, const pt_matrix &ptm, fp_matrix &itm)
{
  // We want an empty itm matrix.
  assert(itm.empty());

  // We get all the information from the packet trajectory matrix ptm,
  // so we iterate over all its elements and fill in the itm matrix.
  FOREACH_MATRIX_ELEMENT(ptm, i, j, e, pt_matrix)
    // The element e is the packet trajectory for the packet which
    // started at node j and that goes to node i.  Now we iterate over
    // the hops of the packet trajectory.  We start with the first
    // element, because it described the first hop a packet makes.
    for(packet_trajectory::const_iterator
          t = e.begin(); t != e.end(); ++t)
      // Iterate over the links of the hop.  Iterator l points to a
      // pair of <Edge, dist_poly>.
      for(packet_trajectory::mapped_type::const_iterator
            l = t->second.begin(); l != t->second.end(); ++l)
        {
          // The node where the packet arrives along this link.
          Vertex ld = target(l->first, g);

          // The rate of packets that arrive at node l.
          double rate = sum(l->second).mean();

          // We don't expect zero rates, because the loops assure that
          // we iterate over the existing elements of the matrix, and
          // they have to be non-zero.
          assert(rate != 0);

          // Here we add this rate to matrix itm for the node ld,
          // remembering that the packets go to node i.
          itm[i][ld] += rate;
        }
}


void
generate_atm(const Graph &g, const fp_matrix &tm,
             const fp_matrix &itm, fp_matrix &atm)
{
  // We want an empty atm matrix.
  assert(atm.empty());

  // Calculate admission rates for each source node j.
  BGL_FORALL_VERTICES(j, g, Graph)
    {
      // This is the total transit traffic rate at node j.  Here we
      // add rates of all demands that arrive at node j, except the
      // demand for which node j is the destination.
      double alpha_prime = 0;
      BGL_FORALL_VERTICES(i, g, Graph)
        if (i != j && itm.exists(i, j))
          {
            double rate = itm.at(i, j);
            assert(rate != 0);
            alpha_prime += rate;
          }

      // Get the output capacity of node j.
      int v = get_output_capacity(g, j);

      // The mapping between the vertex and a floating point value.
      typedef map<Vertex, double> ver2fp;

      // These are the demands for destination nodes.  i is the
      // destination node.
      ver2fp beta;
      BGL_FORALL_VERTICES(i, g, Graph)
        if (tm.exists(i, j))
          {
            double rate = tm.at(i, j);
            assert(rate != 0);
            beta[i] = rate;
          }

      // We know that at node j the number of packets in transit is
      // alpha_prime, the output capacity is v, and the average
      // numbers of packets asking admission are given by beta.
      distro apd;
      if (alpha_prime != 0)
        apd = poisson(alpha_prime);

      ver2fp beta_prime = admit_ana(apd, v, beta);

      BOOST_FOREACH(ver2fp::value_type &ii, beta_prime)
        // Make sure the admission rate is not zero.
        if (ii.second != 0)
          // ii.first is the destination node of the demand.
          atm[ii.first][j] = ii.second;
    }
}


void
generate_otm(const fp_matrix &itm, const fp_matrix &atm, fp_matrix &otm)
{
  // We want an empty otm matrix.
  assert(otm.empty());

  // Here we copy anything there is in the itm matrix except the
  // elements that describe the packets that arrive at their
  // destinations, and hence the condition "i != j" below.
  FOREACH_MATRIX_ELEMENT(itm, i, j, rate, fp_matrix)
    if (i != j)
      {
        assert(rate != 0);
        otm[i][j] = rate;
      }

  // Here we add anything there is in the atm matrix.
  FOREACH_MATRIX_ELEMENT(atm, i, j, rate, fp_matrix)
    {
      assert(i != j);
      assert(rate != 0);
      otm[i][j] += rate;
    }
}


void
generate_epm(const Graph &g, const fp_matrix &otm, edge_probs_matrix &epm)
{
  // We want an empty epm matrix.
  assert(epm.empty());

  // For every node j, calculate the routing probabilities.
  BGL_FORALL_VERTICES(j, g, Graph)
    {
      map<Vertex, distro> input;

      // Get the rate of packets that go to node i.
      BGL_FORALL_VERTICES(i, g, Graph)
        if (otm.exists(i, j))
          {
            // We assert that there are no packets that want to
            // leave node j to get back to the very same node j.
            assert(i != j);
            double rate = otm.at(i, j);
            assert(rate != 0);
            input[i] = poisson(rate);
          }

      edge_probs_map probs;
      route_ana(g, j, input, probs);

      for(edge_probs_map::const_iterator
            i = probs.begin(); i != probs.end(); ++i)
        epm[i->first][j] = i->second;
    }
}


void
generate_T(const Graph &g, Vertex dest, const edge_probs_matrix &epm,
           trans_matrix &T)
{
  // We want an empty T matrix.
  assert(T.empty());

  // The element pointed out by dest_row_iter is a map, which stores
  // the probabilities when a packet is at the dest node.  The map is
  // addressed with another index, which is the node at which a packet
  // is currently residing, and the value is an edge_probs.
  edge_probs_matrix::const_iterator dri = epm.find(dest);

  // If we didn't find node dest, then we don't generate matrix T.
  if (dri != epm.end())
    // Here we iterate over the current node, which is j.  This index
    // also refers to the j index of the T matrix.
    for(edge_probs_map::const_iterator
          j = dri->second.begin(); j != dri->second.end(); j++)
      for(edge_probs::const_iterator
            i = j->second.begin(); i != j->second.end(); i++)
        if ((bool) i->second && j->first != dest)
          // At this line we create a polynomial with the function
          // operator.  The coefficient is i->second, which is the
          // probability of traversing link i->first, and the power is
          // the length of the link, i.e. the edge_weight property.
          T[target(i->first, g)][j->first]
            (i->second, get(edge_weight, g, i->first));
}
