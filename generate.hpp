#ifndef GENERATE_HPP
#define GENERATE_HPP

#include "edge_probs.hpp"
#include "graph.hpp"
#include "matrixes.hpp"
#include "packet.hpp"

/**
 * Generates the input traffic matrix, which is returned as the itm
 * argument.  The itm matrix describes the traffic that enters nodes
 * from neighbor nodes.  The values stored are real numbers, which are
 * the mean rates of the Poisson distribution.
 *
 * The element [i][j] tells the rate of packets packets that enter
 * node j that are destined to node i.  This element doesn't contain
 * the traffic that has just been admitted at node j to node i.
 *
 * The matrix does contain rates for packets that arrive at the
 * destination node.  Element itm[i][i] is the rate of packets that
 * are sent to the local drop of node i, and it's a sum of rates for
 * all demands.
 *
 * @param g the graph
 *
 * @param ptm the packet trajectory matrix based on which we generate
 * the input traffic matrix
 *
 * @param itm the input traffic matrix that returns the resuls of this
 * function
 */
void
generate_itm(const Graph &g, const pt_matrix &ptm, fp_matrix &itm);

/**
 * Generates the admitted traffic matrix.  The mean rate of admitted
 * packets is put into the atm argument.  This function reflects the
 * policy of admitting packets into the network.  The input data is
 * the graph g, the traffic matrix tm and the input traffic matrix
 * itm.
 *
 * The element [i][j] gives the mean rate of packets that start at
 * node j and go to node i.  Real numbers are enough, and polynomials
 * are not necessary, because the admitted packets don't have any
 * history.
 *
 * @param g the graph
 *
 * @param tm the traffic matrix
 *
 * @param itm the input traffic matrix
 *
 * @param atm the admitted traffic matrix
 */
void
generate_atm(const Graph &g, const fp_matrix &tm,
	     const fp_matrix &itm, fp_matrix &atm);

/**
 * Generates the output traffic matrix otm based on the input traffic
 * matrix itm and the admitted traffic matrix atm.
 *
 * Element [i][j] tells the mean rate of packets that arrive at the
 * routing block of node j that want to go to node i.
 *
 * @param itm the input traffic matrix
 *
 * @param atm the admitted traffic matrix
 *
 * @param otm the output traffic matrix
 */
void
generate_otm(const fp_matrix &itm, const fp_matrix &atm, fp_matrix &otm);

/**
 * Generates the edge probability matrix based on the graph g and the
 * output traffic matrix otm.
 *
 * The element otm[i][j] tells the edge probabilities for a packet
 * that resides at node j and wants to get to node i.  Now, this
 * element is a map between edges (all these edges leave node j) and
 * probabilities, and it tells how likely a packet is to leave from
 * node j along this link.  These probabilities are conditional
 * probabilities, i.e. they make sense when you know the packet is
 * already at node j and wants to leave that node.
 *
 * @param g the graph
 *
 * @param otm the output traffic matrix
 *
 * @param epm the edge probability matrix
 */
void
generate_epm(const Graph &g, const fp_matrix &otm, edge_probs_matrix &epm);

/**
 * Here we create the transition matrix for packets which go to the
 * dest node.  We generate it based on the graph g and edge
 * probabilities stored in epm.  We put the result in the matrix T.
 *
 * @param g the graph
 *
 * @param dest the destination node
 *
 * @param epm the edge probability matrix
 *
 * @param T the transition matrix
 */
void
generate_T(const Graph &g, Vertex dest, const edge_probs_matrix &epm,
	   trans_matrix &T);

#endif /* GENERATE_HPP */
