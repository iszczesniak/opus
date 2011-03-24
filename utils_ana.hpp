#ifndef UTILS_ANA_HPP
#define UTILS_ANA_HPP

#include "distro.hpp"
#include "edge_probs.hpp"
#include "graph.hpp"
#include "matrixes.hpp"

#include <map>

/**
 * Performs this operation: result = v * T
 */
void
make_hop(const Graph &g, const map<Vertex, dist_poly> &v,
         const trans_matrix &T, map<Vertex, dist_poly> &result,
         map<Edge, dist_poly> &used_links);

/**
 * This function calculates the average number of admitted packets
 * based on the average number of packets in transit alpha_prime, the
 * output capacity v, and the mean rates of demands given by beta.
 * The return value is the map of mean rates of the admitted packets.
 *
 * @param apd the distribution of alpha_prime, i.e. the number
 * of packets in transit
 *
 * @param v the output capacity of the node
 *
 * @param betas the mean rates of packets asking admission
 */
map<Vertex, double>
admit_ana(distro& apd, int v, const map<Vertex, double> &betas);

/**
 * This function returns the distribution of the admitted demands.
 * Now the returned distributions are Poisson with the mean rates
 * returned by function admit_ana.
 *
 * @param apd the distribution of alpha_prime, i.e. the number
 * of packets in transit
 *
 * @param v the output capacity of the node
 *
 * @param betas the mean rates of packets asking admission
 */
map<Vertex, distro>
admit_ana_distro(distro &apd, int v,
                 const map<Vertex, double> &betas);

/**
 * This function returns the number of packets that should be sent
 * along an edge.
 *
 * @param g the graph
 *
 * @param j the current node
 *
 * @param arr the arrangement, there are exactly arr[i] packets going
 * to node i
 *
 * @param count the element[i][e] is the number of packets destined to
 * node i that should be sent along edge e
 */
void
route_arr(const Graph &g, Vertex j, const map<Vertex, int> &arr,
          edge_count_map &count);

/**
 * This function calculates the probabilities of routing for packets
 * in a specific arrangement for node j.  The input are graph g, node
 * j, arrangement arr, and the results are returned in epm.  As the
 * output we get a map of edge_probs which stores the routing
 * probabilities, i.e. the probabilities that packets traverse
 * specific output edges.
 *
 * @param g the graph
 *
 * @param j the current node
 *
 * @param arr the arrangement, there are exactly arr[i] packets going
 * to node i
 *
 * @param epm the routing probabilities, element epm[i][e] is the
 * probability that packets destined to node i are routed to link e
 */
void
arr_route_prob(const Graph &g, Vertex j, const map<Vertex, int> &arr,
               edge_probs_map &probs);

/**
 * This function calculates the routing probabilities with analysis.
 * As the input we need graph g and node j for which probabilities are
 * calculated.  Element mus[i] tells the rate of packets that are
 * destined to node i.  As the output we get a map of edge_probs which
 * stores the routing probabilities, i.e. the probabilities that
 * packets traverse specific output edges.
 *
 * @param g the graph
 *
 * @param j the node for which the routing probabilities are calculated
 *
 * @param mus the distros of packets asking routing; element mus[i]
 * is the distro of packets going to node i
 *
 * @param epm the routing probabilities, element epm[i][e] is the
 * probability that packets destined to node i are routed to link e
 */
void
route_ana(const Graph &g, Vertex j, const map<Vertex, distro> &mus,
          edge_probs_map &probs);

#endif /* UTILS_ANA_HPP */
