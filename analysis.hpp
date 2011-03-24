#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include "graph.hpp"
#include "matrixes.hpp"
#include "serializer.hpp"

#include <boost/tuple/tuple.hpp>

#include <list>

using namespace std;

boost::tuple<pp_matrix, pt_matrix>
ana_iteration(const Graph &g, const fp_matrix &tm,
              int HL, const list<pt_matrix> &prev_ptm, ostream &os);

/**
 * The analytical solver.
 *
 * @param g the graph
 *
 * @param tm the traffic matrix
 *
 * @param HL the hop limit
 *
 * @param DL the distance limit
 *
 * @param iters the number of iterations
 *
 * @param AL the number of iterations taken for averages
 *
 * @return a pair of the packet presence matrix and the packet
 * trajectory matrix
 *
 * @see sim_solution
 */
boost::tuple<pp_matrix, pt_matrix>
ana_solution(const Graph &g, const fp_matrix &tm, int HL, int DL,
             int iters, int AL, ostream &os, serializer &s);

boost::tuple<pp_matrix, pt_matrix>
ana_solution(const Graph &g, const fp_matrix &tm, int HL, int DL,
             int iters, int AL, ostream &os);

#endif /* ANALYSIS_HPP */
