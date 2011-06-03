#ifndef PACKET_HPP
#define PACKET_HPP

#include "config.hpp"
#include "graph.hpp"
#include "polynomial.hpp"

#include <boost/graph/iteration_macros.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/list.hpp>

using namespace std;

/**
 * This describes what links packets traverse in the subsequent hops.
 * Note that it's for hops, not time slots!
 *
 * The element packet_trajectory[k][e] gives the polynomial for hop k
 * and edge e.
 *
 * The whole object describes a single demand.
 */
typedef map<int, map<Edge, dist_poly> > packet_trajectory;

/**
 * Remembers the number of time slots during which a specific number
 * of packets traversed a link.  It's used to build packet_trajectory.
 */
typedef map<int, map<Edge, count_map_poly> > packet_trajectory_count_map;

/**
 * Remembers the number of packets and the distance they travelled in
 * a single time slot.  It's used to build
 * packet_trajectory_count_map.
 */
typedef map<int, map<Edge, count_poly> > packet_trajectory_count;

/**
 * This describes what nodes packets visit in the subsequent hops.
 * Note that it's for hops, not time slots!
 *
 * The element packet_presence[k][v] gives the polynomial for hop k
 * and vertex v.
 *
 * The whole object describes a single demand.
 */
typedef map<int, map<Vertex, dist_poly> > packet_presence;

/**
 * Remembers the number of time slots during which a specific number
 * of packets visited a node.  It's used to build packet_presence.
 */
typedef map<int, map<Vertex, count_map_poly> > packet_presence_count_map;

/**
 * Remembers the number of packets that visit node during a single
 * time slot.  It's used to build packet_presence_count_map.
 */
typedef map<int, map<Vertex, count_poly> > packet_presence_count;

/**
 * The packet structure.  There is a close match between the packet
 * struct and the packet_prefs.
 */
struct packet
{
  /**
   * Every packet has a unique ID.  I introduced it for debugging.
   */
  int ID;

  /**
   * The next available ID for a packet.
   */
  static int next_ID;

  packet(Vertex _src, Vertex _dst, timeslot _start_ts) :
    src(_src), dst(_dst), start_ts(_start_ts), next_ts(_start_ts),
    ID(next_ID++), hops(0) {}

  /**
   * This keeps the number of hops the packet made.
   */
  int hops;

  /**
   * The source of the packet.
   */
  Vertex src;

  /**
   * The destination of the packet.
   */
  Vertex dst;

  /**
   * The start time slot: the time slot a packet was created.
   */
  timeslot start_ts;

  /**
   * The next time slot: the time slot at which the packet arrives at
   * the next node.  The next node can be both an intermediate node or
   * the destination node.
   */
  timeslot next_ts;
};

/**
 * Compares two packets based on the next_ts field.  This is used to
 * sort packets based on time slots at which packets arrive at their
 * next nodes.
 */
class pkt_next_ts_cmp
{
public:
  bool operator()(const packet *a, const packet *b) const
  {
    return a->next_ts < b->next_ts;
  }
};

/**
 * This structure stores waiting packets.  This structure is used to
 * store packets that travel in a fiber.
 */
typedef multiset<packet *, pkt_next_ts_cmp> waiting_pkts;

/**
 * Stores the preferred output edges of a packet.
 */
class packet_prefs: public list<Vertex>
{
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int)
  {
    ar & boost::serialization::base_object<list<Vertex> >(*this);
    ar & cls;
    ar & dest;
  }

public:
  /**
   * The class of the packet.
   */
  int cls;

  /**
   * The destination of the packet.
   */
  Vertex dest;
};

/**
 * Prints the packet_prefs.
 */
ostream &operator << (ostream &out, const packet_prefs &prefs);

/**
 * Prints the packet.
 */
ostream &operator << (ostream &out, const packet &pkt);

/**
 * This is a function object to compare preferences of a packet, which
 * we use for sorting packet preferences.  For this task we need to
 * know graph g, node j where the packet currently is, and node i
 * where the packets is going.
 */
template<typename G>
class cmp_pref
{
  typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
  typedef typename graph_traits<Graph>::edge_descriptor Edge;

  // The node where packets currently are.
  const Vertex j;
  // The node where packets are going.
  const Vertex i;
  const G &g;

public:
  cmp_pref(const Vertex _j, const Vertex _i, const G &_g) :
    j(_j), i(_i), g(_g) {}

  /**
   * Nodes a and b are the neighor nodes of node j.  The function is
   * true, if by going to node a we would reach node i at a smaller
   * cost than by going to node b.
   */
  bool operator()(const Vertex a, const Vertex b)
  {
    Edge ea = edge(j, a, g).first;
    int eac = get(vertex_distance, g, a)[i];
    int vac = get(edge_weight, g, ea);

    Edge eb = edge(j, b, g).first;
    int ebc = get(vertex_distance, g, b)[i];
    int vbc = get(edge_weight, g, eb);

    return eac + vac < ebc + vbc;
  }
};

/**
 * This function generates packet preferences for a packet at node j,
 * which is destined to some node i in the graph g.
 */
template<typename G>
packet_prefs
make_prefs(const typename graph_traits<G>::vertex_descriptor &i,
           const typename graph_traits<G>::vertex_descriptor &j,
           const G &g)
{
  packet_prefs prefs;

  prefs.dest = i;

  BGL_FORALL_OUTEDGES_T(j, k, g, G)
    {
      Vertex v = target(k, g);
      prefs.push_back(v);
    }

  prefs.sort(cmp_pref<G>(j, i, g));

  prefs.cls = 0;

  if (prefs.size())
    {
      // Threshold distance: twice the smallest distance.
      int trs_dist = 0;
      {
        Vertex v = prefs.front();
        Edge e = edge(j, v, g).first;
        // Distance from vertex v to vertex i.
        trs_dist += get(vertex_distance, g, v)[i];
        // Distance from j to v.
        trs_dist += get(edge_weight, g, e);
        trs_dist *= 2;
      }

      // Here we calculate the class of the packet: we count the
      // number of preferences which yield the path shorter than
      // trs_dist.
      for(packet_prefs::iterator c = prefs.begin(); c != prefs.end(); ++c)
        {
          Vertex v = *c;
          Edge e = edge(j, v, g).first;
          int vc = get(vertex_distance, g, v)[i];
          int ec = get(edge_weight, g, e);
          if (vc + ec < trs_dist)
            ++prefs.cls;
      }
    }

  return prefs;
}

/**
 * This function finds the iterator, which points to an element whose
 * next_ts > ts, or it returns pkts.end().
 */
waiting_pkts::const_iterator
find_waiting_pkts(const waiting_pkts &pkts,
                  timeslot ts);

#endif /* PACKET_HPP */
