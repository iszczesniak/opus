#ifndef UTILS_TRAGEN_HPP
#define UTILS_TRAGEN_HPP

#include "graph.hpp"
#include "matrixes.hpp"
#include "tragen_args.hpp"

/**
 * Generate the traffic matrix.
 */
void
generate_tm(const Graph &g, fp_matrix &tm, const tragen_args &args);

/**
 * Generate the random traffic matrix.
 */
void
generate_tm(const Graph &g, fp_matrix &tm, int demands, double poisson_mean);

/**
 * Generate the random traffic matrix between the vertexes in the given set.
 */
void
generate_tm(const Graph &g, fp_matrix &tm, int demands, double poisson_mean,
	    set<Vertex> s);

#endif /* UTILS_TRAGEN_HPP */
