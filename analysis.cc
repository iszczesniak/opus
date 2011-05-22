#include "analysis.hpp"

#include "config.hpp"
#include "generate.hpp"
#include "graph.hpp"
#include "poisson.hpp"
#include "polynomial.hpp"
#include "serializer.hpp"
#include "utils.hpp"
#include "utils_ana.hpp"

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/progress.hpp>

boost::tuple<pp_matrix, pt_matrix>
ana_iteration(const Graph &g, const fp_matrix &tm,
              int HL, const list<pt_matrix> &ptm_list,
              ostream &os)
{
  // This is the matrix of input traffic.
  fp_matrix itm;

  // This is the admitted traffic matrix.
  fp_matrix atm;

  // This is the matrix of output demands.
  fp_matrix otm;

  os << "Generating ITM, ATM and OTM" << endl;

  BOOST_FOREACH(const pt_matrix &ptm, ptm_list)
    {
      // These are the matrices for a single iteration.
      fp_matrix pitm;
      fp_matrix patm;
      fp_matrix potm;

      generate_itm(g, ptm, pitm);
      generate_atm(g, tm, pitm, patm);
      generate_otm(pitm, patm, potm);

      atm = atm + patm;
      otm = otm + potm;
    }

  float weight = 1.0 / ptm_list.size();
  atm = weight * atm;
  otm = weight * otm;

  // This is the edge probability matrix.
  edge_probs_matrix epm;
  os << "Generating EPM" << endl;
  generate_epm(g, otm, epm);

  int steps_needed = 0;

  // Calculate the needed steps.
  BGL_FORALL_VERTICES(i, g, Graph)
    BGL_FORALL_VERTICES(j, g, Graph)
    if (i != j && atm.exists(i, j))
      steps_needed += HL;

  progress_display progress(steps_needed, os);

  pp_matrix ppm;
  pt_matrix ptm;

  // For every destination node i.
  BGL_FORALL_VERTICES(i, g, Graph)
    {
      // This is the transition matrix.  This matrix is shared
      // among all packets that have the same destination node.
      trans_matrix T;
      generate_T(g, i, epm, T);

      // For every source node j.
      BGL_FORALL_VERTICES(j, g, Graph)
        if (i != j && atm.exists(i, j))
          {
            double admitted = atm.at(i, j);
            assert(admitted != 0);

            // Packet presence vector.
            map<Vertex, dist_poly> P;
            // Here we remember the admission probability.
            P[j] = dist_poly(distro(poisson(admitted)));
            ppm[i][j][0] = P;

            // In this loop we trace all the hops of a packet.
            for(int hop = 1; hop <= HL; ++hop)
              {
                // Each loop iteration corresponds to the packet
                // traversing some links, because it makes one
                // hop.  For every link we want to know the
                // polynomial that describes the traversing
                // packets.
                map<Vertex, dist_poly> nodes;
                map<Edge, dist_poly> links;

                make_hop(g, P, T, nodes, links);
                ppm[i][j][hop] = nodes;
                ptm[i][j][hop] = links;
                P = nodes;

                ++progress;
              }
          }
    }

  return tie(ppm, ptm);
}

boost::tuple<pp_matrix, pt_matrix>
ana_solution(const Graph &g, const fp_matrix &tm,
             int HL, int DL, int iters, int AL, ostream &os,
             serializer &s)
{
  list<pp_matrix> ppm_list;
  list<pt_matrix> ptm_list;

  // By polynomials we model the distance a packet has travelled.  We
  // allow packets to travel DL or less.  Packets are removed if they
  // try to travel more than DL.
  dist_poly::set_S(DL + 1);

  for(int iter = 1; iter <= iters; ++iter)
    {
      pp_matrix ppm;
      pt_matrix ptm;
      os << "Iteration #" << iter << ": " << endl;
      tie(ppm, ptm) = ana_iteration(g, tm, HL, ptm_list, os);

      s(ppm, ptm, iter);

      if (ppm_list.size() == AL)
        ppm_list.pop_front();
      ppm_list.push_back(ppm);

      if (ptm_list.size() == AL)
        ptm_list.pop_front();
      ptm_list.push_back(ptm);
    }

  return tie(ppm_list.back(), ptm_list.back());
}

boost::tuple<pp_matrix, pt_matrix>
ana_solution(const Graph &g, const fp_matrix &tm,
             int HL, int DL, int iters, int AL, ostream &os)
{
  arguments args;
  serializer s(args, g, tm);

  return ana_solution(g, tm, HL, DL, iters, AL, os, s);
}
