#ifndef ROU_ORDER_HPP
#define ROU_ORDER_HPP

#include "graph.hpp"
#include "packet.hpp"

#include <functional>

/**
 * With this class we establish the order of routing for packets that
 * are currently at node j given to the constructor.  The order
 * depends on the destination nodes v1 and v2 of packets.
 */
class rou_order : public binary_function<Vertex, Vertex, bool>
{
  const Graph &g;
  const Vertex j;

public:
  /**
   * @param _g the graph
   *
   * @param _j the vertex at which packets are
   */
  rou_order(const Graph &_g, const Vertex _j);

  /**
   * The function operators that establishes the order.
   */
  bool operator()(const Vertex &v1, const Vertex &v2) const;
};

#endif /* ROU_ORDER_HPP */
