#ifndef UTILS_SIM_HPP
#define UTILS_SIM_HPP

#include "edge_probs.hpp"
#include "graph.hpp"
#include "matrixes.hpp"
#include "distro.hpp"

#include <map>

#include <gsl/gsl_rng.h>

/**
 * We use this function to report the presence of packets.  This
 * function fills in the matrix of packet presence.  The packet pkt is
 * present at node v.
 */
void
report_packet_presence(Vertex v, const packet &pkt, ppc_matrix &ppcm);

/**
 * We use this function to report the transition of packets.  This
 * function fills in the matrix of packet trajectories.  The packet
 * pkt transits along link e.
 */
void
report_packet_transition(Edge e, const packet &pkt, ptc_matrix &ptcm);

/**
 * This function is called to process packets at a node.  This
 * function is called for a single node -- the node where packets are
 * routed.
 *
 * The function knows how to route the packets, because it knows the
 * network topology with the g parameter.
 */
void
process_packets(const Graph &g, Vertex j, timeslot ts,
                map<Vertex, waiting_pkts> &pqv,
                waiting_pkts &local_add, waiting_pkts &local_drop,
                ppcm_matrix &ppcmm, ptcm_matrix &ptcmm,
                int HL, int DL);

/**
 * Calculates the number of packets in transit at node j during the
 * time slot ts based on the information in the packet queue per
 * vector.
 *
 * @param pqv the packet queue per vector
 *
 * @param j the vertex for which we calculate the number
 *
 * @param ts the time slot for which we calculate the number
 *
 * @return the number of packets in transit
 */
int
calc_nr_in_transit(const map<Vertex, waiting_pkts> &pqv,
                   Vertex j, timeslot ts);

/**
 * Deletes the packets from the given structure.
 *
 * @param wp the structure with packets to delete
 */
void
delete_waiting_pkts(waiting_pkts &wp);

/**
 * Put the packets into the local_add queue.
 *
 * @param local_add the place where packets will be put
 *
 * @param j the current node
 *
 * @param i the destination node
 *
 * @param ts the time slot
 *
 * @param tm the traffic matrix
 *
 * @param rng the random number generator
 */
void
fill_local_add(waiting_pkts &local_add, Vertex j, Vertex i,
	       timeslot ts, const fp_matrix &tm, gsl_rng *rng);

/**
 * This function converts a ppcm_matrix to pp_matrix.
 */
void
ppcmm2ppm(const ppcm_matrix &ppcmm, pp_matrix &ppm, int time_slots);

/**
 * This function converts a ptc_matrix to pt_matrix.
 */
void
ptcmm2ptm(const ptcm_matrix &ptcmm, pt_matrix &ptm, int time_slots);

/**
 * Converts a count_map_poly to a dist_poly.
 */
void
cmp2dp(const count_map_poly &cp, dist_poly &dp, int time_slots);

/**
 * Adds to the ppcm_matrix object the ppc_matrix object.
 */
ppcm_matrix &
operator += (ppcm_matrix &ppcmm, const ppc_matrix &ppcm);

/**
 * Adds to the ptcm_matrix object the ptc_matrix object.
 */
ptcm_matrix &
operator += (ptcm_matrix &ptcmm, const ptc_matrix &ptcm);

#endif /* UTILS_SIM_HPP */
