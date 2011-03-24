#ifndef UTILS_TRAGEN_HPP
#define UTILS_TRAGEN_HPP

#include "graph.hpp"
#include "matrixes.hpp"

/**
 * Generate the traffic matrix.
 */
void
generate_tm(const Graph &g, fp_matrix &tm, int demands, double load);

#endif /* UTILS_TRAGEN_HPP */
