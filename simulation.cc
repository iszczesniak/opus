#include "simulation.hpp"
#include "config.hpp"
#include "generate.hpp"
#include "rand.hpp"
#include "utils_sim.hpp"
#include "utils.hpp"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <ctime>
#include <set>
#include <vector>

#include <boost/progress.hpp>

#define TS_LIMIT 100000

using namespace std;

boost::tuple<pp_matrix, pt_matrix>
sim_solution(const Graph &g, const fp_matrix &tm, int HL, int DL,
             ostream &os)
{
  // The simulation operates on packet pointers.  A packet is created
  // at the source node (with the new operator) and is destroyed (with
  // the delete operator) at one of the following:
  //
  // - rejection at the admission,
  //
  // - removal because it exceeded the hop limit,
  //
  // - delivery to the destination node.

  // GSL random number generator
  const gsl_rng_type *T_rng;
  gsl_rng *rng;
  gsl_rng_env_setup();
  T_rng = gsl_rng_default;
  rng = gsl_rng_alloc(T_rng);
  gsl_rng_set (rng, 0);

  // The number of packets that visit nodes.
  ppcm_matrix ppcmm;

  // The number of packets that traverse links.
  ptcm_matrix ptcmm;

  // This is a map of packet queues.  Each node has ist own queue,
  // that stores packets that will arrive to this node.  Packets in
  // sets will be sorted based on their next_ts field.
  map<Vertex, waiting_pkts> pqv;

  progress_display progress(TS_LIMIT, os);

  // In every iteration of this loop we simulate the behaviour of the
  // network for this specific timeslot ts.
  for (timeslot ts = 0; ts < TS_LIMIT; ts++)
    {
      // Here we will process every node j separately.  The j node is
      // the "current node" at which we process packets.  Packets are
      // sent to the i node.
      BGL_FORALL_VERTICES(j, g, Graph)
        {
          // These are the local packets that ask for admission.
          waiting_pkts local_add;

          // These are the packets for which this packet is the
          // destination.
          waiting_pkts local_drop;

          // We generate the local packets that ask for admission.  We
          // randomly choose them, and repeat this process for all the
          // destination nodes i.
          BGL_FORALL_VERTICES(i, g, Graph)
            fill_local_add(local_add, j, i, ts, tm, rng);

          // This function does all the packet processing.
          process_packets(g, j, ts, pqv, local_add, local_drop,
                          ppcmm, ptcmm, HL, DL);

          // Here we process the packets that asked for admission, but
          // were jilted.  We simply delete them.
          delete_waiting_pkts(local_add);

          // Here we process the packets that are arriving at the
          // destination, i.e. packets that we find in local_drop.
          delete_waiting_pkts(local_drop);
        }

      ++progress;
    }

  gsl_rng_free(rng);

  // Convert the simulation structures to structures that are
  // returned.
  pp_matrix ppm;
  pt_matrix ptm;
  ppcmm2ppm(ppcmm, ppm, TS_LIMIT);
  ptcmm2ptm(ptcmm, ptm, TS_LIMIT);

  return tie(ppm, ptm);
}
