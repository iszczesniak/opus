#include "edge_probs.hpp"
#include "polynomial.hpp"
#include "rand.hpp"
#include "tabdistro.hpp"
#include "utils.hpp"
#include "utils_ana.hpp"
#include "utils_sim.hpp"

#include <gsl/gsl_randist.h>

#include <map>

void
report_packet_presence(Vertex v, const packet &pkt, ppc_matrix &ppcm)
{
  // This is the packet presence of the demand.
  packet_presence_count &ppc = ppcm[pkt.dst][pkt.src];

  // The map of polynomials ppmap describe where packets reside after
  // they made hop number pkt.hops.
  map<Vertex, count_poly> &v2cp = ppc[pkt.hops];

  // This is the delay the packet incurred in the network.
  int delay = pkt.next_ts - pkt.start_ts;

  // Here we finally fill in the right info.
  v2cp[v][delay]++;
}

void
report_packet_transition(Edge e, const packet &pkt, ptc_matrix &ptcm)
{
  // This is the packet trajectory of the demand.
  packet_trajectory_count &pt = ptcm[pkt.dst][pkt.src];

  // The map of polynomials ptmap describe what links packets take
  // during the hop number pkt.hops.
  map<Edge, count_poly> &e2cp = pt[pkt.hops];

  // This is the delay the packet incurred in the network.
  int delay = pkt.next_ts - pkt.start_ts;

  // Here we finally fill in the right info.
  e2cp[e][delay]++;
}

void
process_packets(const Graph &g, Vertex j, timeslot ts,
                map<Vertex, waiting_pkts> &pqv,
                waiting_pkts &local_add, waiting_pkts &local_drop,
                ppcm_matrix &ppcmm, ptcm_matrix &ptcmm,
                int HL, int DL)
{
  // This is the reference to the queue of packets that wait to be
  // processed.
  waiting_pkts &wp = pqv[j];

  // Keeps the number of packets that visited node j.
  ppc_matrix ppcm;

  // Keeps the number of packets that traversed links leaving node j.
  ptc_matrix ptcm;

  // These are packets that will be processed in this time slot.
  waiting_pkts to_route;

  // Here we find the packets, which arrived from neighbor nodes.
  waiting_pkts::iterator first = wp.begin();
  waiting_pkts::iterator last = find_waiting_pkts(wp, ts);
  to_route.insert(first, last);
  wp.erase(first, last);

  // Now we report these packets arrived at node j.
  for(waiting_pkts::iterator k = to_route.begin(); k != to_route.end(); ++k)
    report_packet_presence(j, **k, ppcm);

  // In the to_route structure there can be packts which are destined
  // to this node, and so we need to move them to local_drop.  In
  // to_route there are now only packets that came from other nodes.
  for(waiting_pkts::iterator k = to_route.begin(); k != to_route.end();)
    {
      // We need to play this way with iterators because we can remove
      // the entry pointed out by this iterator, which invalidates the
      // iterator.
      waiting_pkts::iterator l = k;
      k++;

      if ((*l)->dst == j)
        {
          // This packet is destined to this node, so add it to
          // local_drop and remove it from to_route.
          local_drop.insert(*l);
          to_route.erase(l);
        }
    }

  // Then if there are any outputs left, then we add the local_add
  // packets into the to_route structure.  But first we need to know
  // how many more packets we can add, and in the loop below we do it.
  // "j" is the current node, "edge" is the outgoing edge, "g" is the
  // graph.
  int v = get_output_capacity(g, j);

  // In every iteration of the loop we chose at random one packet from
  // local_add and move it to to_route.  We repeat this as many times
  // as we can.
  while(!local_add.empty() && to_route.size() < v)
    {
      waiting_pkts::iterator k = local_add.begin();
      advance(k, myrand(local_add.size()));

      // We report the newly admitted packets.
      report_packet_presence(j, **k, ppcm);

      to_route.insert(*k);
      local_add.erase(k);
    }

  // We know what packets to send: all the ones from the to_route
  // structure.  But we have to translate these packets into a
  // structure that we can pass for routing.
  map<Vertex, int> arr;
  for(waiting_pkts::iterator i = to_route.begin(); i != to_route.end(); i++)
    arr[(*i)->dst]++;

  edge_count_map result;
  route_arr(g, j, arr, result);

  // And here I finally get the packets from the to_route structure
  // and put them in the right place in the "pqv" structure again.
  // Now we route packets according to what we find in object
  // "result".
  while(!to_route.empty())
    {
      // Get the first element from the to_route structure.
      packet *pkt = *(to_route.begin());
      to_route.erase(to_route.begin());

      // The edge_count for the destination node of packet pkt.
      edge_count &ecm = result[pkt->dst];

      // Find an available output edge.
      edge_count::iterator ei = ecm.begin();
      while(ei != ecm.end() && ei->second == 0)
        ++ei;

      // Check whether the we can route the packet.
      if (ei == ecm.end())
        // Drop the packet since we found no available wavelength.
        delete pkt;
      else
        {
          // Take one wavelength on this edge.
          --ei->second;

          // The edge along which the packet is sent.
          Edge e = ei->first;

          // The delay of edge e.
          int delay = get(edge_weight, g, e);

          // Make sure that the node j is really the source node of
          // this link.  We do this to make sure that we get the BGL
          // concepts right.  I don't know how BGL is able to tell the
          // source and target of a link, since it's a unidirectional
          // link.
          assert(j != target(e, g));

          // Increase the next_ts field by the delay the next link
          // will inflict.
          pkt->next_ts += delay;

          // Increase the number of hops that the packet made.
          ++pkt->hops;

          // This is the total time the packet spent in network.
          timeslot len = pkt->next_ts - pkt->start_ts;

          // We can send the packet to the next node only if it's hop
          // number HL or less, and that the distance it has travelled
          // wasn't greater than DL.
          if (pkt->hops > HL || len > DL)
            // Remove the packet
            delete pkt;
          else
            {
              // Now "send" the packet to the next node.
              pqv[target(e, g)].insert(pkt);

              // Report the transition of the packet along link e.
              report_packet_transition(e, *pkt, ptcm);
            }
        }
    }

  ppcmm += ppcm;
  ptcmm += ptcm;
}

int
calc_nr_in_transit(const map<Vertex, waiting_pkts> &pqv,
                   Vertex j, timeslot ts)
{
  int nr = 0;
  map<Vertex, waiting_pkts>::const_iterator ji = pqv.find(j);

  if (ji != pqv.end())
    {
      const waiting_pkts &wp = ji->second;

      waiting_pkts::const_iterator first = wp.begin();
      waiting_pkts::const_iterator last = find_waiting_pkts(wp, ts);

      while(first != last)
        {
          if ((*first)->dst != j)
            nr++;
          ++first;
        }
    }

  return nr;
}

void
delete_waiting_pkts(waiting_pkts &wp)
{
  while(!wp.empty())
    {
      waiting_pkts::iterator i = wp.begin();
      delete *i;
      wp.erase(i);
    }
}

void
fill_local_add(waiting_pkts &local_add, Vertex j, Vertex i,
               timeslot ts, const fp_matrix &tm, gsl_rng *rng)
{
  if (tm.exists(i, j))
    {
      double rate = tm.at(i, j);
      assert(rate != 0);

      // This is the number of packets that ask for admission at node
      // j and want to go to node i.  This number of packets is for a
      // single slot only.  Packets ask for admission with the Poisson
      // distribution.  The mean number of packets is taken from the
      // traffic matrix tm.
      int number = gsl_ran_poisson(rng, rate);

      // Here we add these packets to the local_add queue.
      while(number--)
        {
          packet *pkt = new packet(j, i, ts);
          local_add.insert(pkt);
        }
    }
}

void
ppcmm2ppm(const ppcm_matrix &ppcmm, pp_matrix &ppm, int time_slots)
{
  // Iterate over the elements of the matrix.
  FOREACH_MATRIX_ELEMENT(ppcmm, i, j, ppcm, ppcm_matrix)
    // Iterate over hops
    for(packet_presence_count_map::const_iterator
          hi = ppcm.begin(); hi != ppcm.end(); ++hi)
      // Iterate over vertexes
      for(packet_presence_count_map::mapped_type::const_iterator
            vi = hi->second.begin(); vi != hi->second.end(); ++vi)
        {
          dist_poly p;
          cmp2dp(vi->second, p, time_slots);
          ppm[i][j][hi->first][vi->first] = p;
        }
}

void
ptcmm2ptm(const ptcm_matrix &ptcmm, pt_matrix &ptm, int time_slots)
{
  // Iterate over the elements of the matrix.
  FOREACH_MATRIX_ELEMENT(ptcmm, i, j, ptcm, ptcm_matrix)
    // Iterate over hops
    for(packet_trajectory_count_map::const_iterator
          hi = ptcm.begin(); hi != ptcm.end(); ++hi)
      // Iterate over edges
      for(packet_trajectory_count_map::mapped_type::const_iterator
            ei = hi->second.begin(); ei != hi->second.end(); ++ei)
        {
          dist_poly p;
          cmp2dp(ei->second, p, time_slots);
          ptm[i][j][hi->first][ei->first] = p;
        }
}

void
cmp2dp(const count_map_poly &cmp, dist_poly &dp, int time_slots)
{
  for(count_map_poly::const_iterator i = cmp.begin(); i != cmp.end(); ++i)
    {
      tabdistro td;
      for(count_map_poly::mapped_type::const_iterator
            j = i->second.begin(); j != i->second.end(); ++j)
        td.set_prob(make_pair((double)j->second / time_slots, j->first));
      dp[i->first] = td;
    }
}

ppcm_matrix &
operator += (ppcm_matrix &ppcmm, const ppc_matrix &ppcm)
{
  // Iterate over the elements of the matrix.
  FOREACH_MATRIX_ELEMENT(ppcm, i, j, ppc, ppc_matrix)
    // Iterate over hops
    for(packet_presence_count::const_iterator
          hi = ppc.begin(); hi != ppc.end(); ++hi)
      // Iterate over vertexes
      for(packet_presence_count::mapped_type::const_iterator
            vi = hi->second.begin(); vi != hi->second.end(); ++vi)
        for(count_poly::const_iterator
              di = vi->second.begin(); di != vi->second.end(); ++di)
          ++(ppcmm[i][j][hi->first][vi->first][di->first][di->second]);

  return ppcmm;
}

ptcm_matrix &
operator += (ptcm_matrix &ptcmm, const ptc_matrix &ptcm)
{
  // Iterate over the elements of the matrix.
  FOREACH_MATRIX_ELEMENT(ptcm, i, j, ptc, ptc_matrix)
    // Iterate over hops
    for(packet_trajectory_count::const_iterator
          hi = ptc.begin(); hi != ptc.end(); ++hi)
      // Iterate over edges
      for(packet_trajectory_count::mapped_type::const_iterator
            vi = hi->second.begin(); vi != hi->second.end(); ++vi)
        for(count_poly::const_iterator
              di = vi->second.begin(); di != vi->second.end(); ++di)
          ++(ptcmm[i][j][hi->first][vi->first][di->first][di->second]);

  return ptcmm;
}
