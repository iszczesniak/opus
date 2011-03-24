#include <cassert>
#include "rou_order.hpp"

rou_order::rou_order(const Graph &_g, const Vertex _j) : g(_g), j(_j)
{
}

/**
 * This function returns true, if v1 should be routed first,
 * and false otherwise.
 */
bool rou_order::operator()(const Vertex &v1, const Vertex &v2) const
{
  const packet_prefs &p1 = get(vertex_bundle, g, j)[v1];
  const packet_prefs &p2 = get(vertex_bundle, g, j)[v2];

  // It can happen that a destination can be of class zero.  This
  // happens when there is no path to the destination.  If there are
  // two destinations of class zero, then we need to establish a
  // consistent order between them: we just say that the node of lower
  // index is more important.
  if (!p1.cls && !p2.cls)
    {
      return v1 < v2;
    }

  // p1 should go first, because it has some class, while p2 has zero.
  if (p1.cls && !p2.cls)
    {
      return true;
    }

  // p2 should go first, because it has some class, while p1 has zero.
  if (!p1.cls && p2.cls)
    {
      return false;
    }

  // If classes of the packets are the same, then we need to do a more
  // complicated comparison: we need to figure out which edge leads to
  // the destination along a shorter path.
  if (p1.cls == p2.cls)
    {
      assert(p1.size() && p2.size());

      Edge e1 = edge(j, p1.front(), g).first;
      Edge e2 = edge(j, p2.front(), g).first;

      return get(edge_weight, g, e1) < get(edge_weight, g, e2);
    }

  return (p1.cls < p2.cls);
}
