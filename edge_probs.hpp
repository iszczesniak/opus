#ifndef EDGE_PROBS_HPP
#define EDGE_PROBS_HPP

#include "graph.hpp"
#include "sparse_matrix.hpp"

#include <map>

using namespace std;

/**
 * These are probabilities of traversing links.
 */
typedef map<Edge, double> edge_probs;

/**
 * This type maps the edge_probs with destination node i.
 */
typedef map<Vertex, edge_probs> edge_probs_map;

/**
 * These are number of packets that traverse links.
 */
typedef map<Edge, int> edge_count;

/**
 * This type maps the edge_count with destination node i.
 */
typedef map<Vertex, edge_count> edge_count_map;

/**
 * Element edge_probs_matrix[i][j] stores edge_probs for each
 * destination node i to which packets want to go at node j
 */
typedef sparse_matrix<Vertex, edge_probs> edge_probs_matrix;

/**
 * This is a multiplication operator for edge_probs which multiplies
 * all probabilities by a constant.
 */
edge_probs &
operator *= (edge_probs &v, double c);

/**
 * This is a multiplication operator for edge_probs which multiplies
 * all probabilities by a constant.
 */
edge_probs 
operator * (double c, const edge_probs &v);

/**
 * This is the += operator for an edge_probs object: we add all the
 * probabilities from v2 to v1.
 */
edge_probs &
operator += (edge_probs &v1, const edge_probs &v2);

/**
 * This is a multiplication operator for edge_probs_map which
 * multiplies all probabilities by a constant.
 */
edge_probs_map &
operator *= (edge_probs_map &v, double c);

/**
 * This is the += operator for an edge_probs_map object: we add all
 * probabilities from v2 to v1.
 */
edge_probs_map &
operator += (edge_probs_map &v1, const edge_probs_map &v2);

/**
 * This is the += operator for an edge_count object: we add all the
 * probabilities from v2 to v1.
 */
edge_count &
operator += (edge_count &v1, const edge_count &v2);

/**
 * This is the += operator for an edge_count_map object: we add all
 * probabilities from v2 to v1.
 */
edge_count_map &
operator += (edge_count_map &v1, const edge_count_map &v2);

/**
 * Print the edge_probs to the stream.
 */
void
print_edge_probs(ostream &os, const edge_probs &probs, const Graph &g);

/**
 * Print the edge_probs_map to the stream.
 */
void
print_edge_probs_map(ostream &os, const edge_probs_map &map, const Graph &g);

/**
 * The << operator for edge_count.
 */
ostream &
operator << (ostream &os, const edge_count &count);

/**
 * The << operator for edge_count_map.
 */
ostream &
operator << (ostream &os, const edge_count_map &map);

#endif /* EDGE_PROBS_HPP */
