#ifndef MATRIXES_HPP
#define MATRIXES_HPP

#include "packet.hpp"
#include "polynomial.hpp"
#include "sparse_matrix.hpp"

/**
 * Element [i][j] of this matrix stores a packet presence for demand
 * (j, i), where j is the source node and i is the destination node.
 */
typedef sparse_matrix<Vertex, packet_presence> pp_matrix;

/**
 * Used in simulation to count the number of packets that arrive at a
 * node during the whole simulation.  Here we count the number of time
 * slots during which packets arrived in a speficic number.  It's used
 * to build the pp_matrix.
 */
typedef sparse_matrix<Vertex, packet_presence_count_map> ppcm_matrix;

/**
 * Used in simulation to count the number of packets that arrive at a
 * node in a single time slot.  It's used to build ppcm_matrix.
 */
typedef sparse_matrix<Vertex, packet_presence_count> ppc_matrix;

/**
 * Element [i][j] of this matrix stores a packet trajectory for demand
 * (j, i), where j is the source node and i is the destination node.
 */
typedef sparse_matrix<Vertex, packet_trajectory> pt_matrix;

/**
 * Used in simulation to count the number of packets that traverse a
 * link during the whole simulation.  Here we count the number of time
 * slots during which packets arrived in a specific number.  It's used
 * to build the pt_matrix.
 */
typedef sparse_matrix<Vertex, packet_trajectory_count_map> ptcm_matrix;

/**
 * Used in simulation to count the number of packets that traverse a
 * link in a single time slot.  It's used to build ptcm_matrix.
 */
typedef sparse_matrix<Vertex, packet_trajectory_count> ptc_matrix;

/**
 * This is matrix that for every element [i][j] stores a floating
 * point value.
 *
 * It's used for the traffic matrix, the input traffic matrix, the
 * admitted traffic matrix and the output traffic matrix.
 *
 * Element [i][j] of the traffic matrix tells how many packets ask
 * admission at node j (this can be the source node) that want to go
 * to node i.
 */
typedef sparse_matrix<Vertex, double> fp_matrix;

/**
 * This is the type of the transition matrix.  The elements are
 * polynomials with floating point coefficients.
 */
typedef sparse_matrix<Vertex, fp_poly> trans_matrix;

/**
 * This is the matrix with dist_poly as elements.  It's used to store
 * the data on admitted and delivered traffic.
 */
typedef sparse_matrix<Vertex, dist_poly> dp_matrix;

#endif /* MATRIXES_HPP */
