#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "graph.hpp"
#include "matrixes.hpp"

#include <iostream>

/**
 * The simulative solver.
 *
 * @param g the graph
 *
 * @param tm the traffic matrix
 *
 * @param HL the hop limit
 *
 * @param DL the distance limit
 *
 * @return a pair of the packet presence matrix and the packet
 * trajectory matrix
 *
 * @see ana_solution
 */
boost::tuple<pp_matrix, pt_matrix>
sim_solution(const Graph &g, const fp_matrix &tm, int HL, int DL,
             ostream &os);

#endif /* SIMULATION_HPP */
