#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <vector>

#include <boost/graph/adjacency_list.hpp>

using namespace std;
using namespace boost;

class packet_prefs;

typedef
adjacency_list_traits<vecS, vecS, undirectedS>::vertex_descriptor
Vertex;

/**
 * The type of the graph we use.
 *
 * Property edge_weight is the distance, while property edge_weight2
 * is the number of wavelengths.
 */
typedef
adjacency_list <vecS, vecS, undirectedS,
                property<vertex_name_t, string,
                property<vertex_distance_t, vector<int>,
                property<vertex_predecessor_t, vector<Vertex>,
                property<vertex_bundle_t, vector<packet_prefs> > > > >,
                property<edge_weight_t, int,
                property<edge_weight2_t, int> > >
Graph;

typedef graph_traits<Graph>::edge_descriptor Edge;

#include "packet.hpp"

namespace std {

  /**
   * This is the operator needed by std::map when its key is the Edge
   * type.
   */
  bool operator <(const Edge &, const Edge &);

  bool operator == (const Edge &, const Edge &);

}

#endif /* GRAPH_HPP */
