#ifndef GRAPH_SERIALIZATION_HPP
#define GRAPH_SERIALIZATION_HPP

#include "graph.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>

extern Graph *p_graph;

namespace boost {
  namespace serialization {

    template<class A>
    void load(A &ar, Edge &p, const unsigned int)
    {
      Vertex s, t;
      ar & s & t;
      p = edge(s, t, *p_graph).first;
    }

    template<class A>
    void save(A &ar, const Edge &p, const unsigned int)
    {
      Vertex s = source(p, *p_graph);
      Vertex t = target(p, *p_graph);
      ar & s & t;
    }

  }
}

BOOST_SERIALIZATION_SPLIT_FREE(Edge)

#endif /* GRAPH_SERIALIZATION_HPP */
