#include "utils_netgen.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <boost/graph/random.hpp>
#include <boost/graph/iteration_macros.hpp>

using namespace std;
using namespace boost;
using namespace boost::graph;
using namespace boost::random;

void
name_vertices(Graph &g)
{
  int count = 1;

  int number = num_vertices(g);
  int width = int(log10(number)) + 1;

  BGL_FORALL_VERTICES(v, g, Graph)
    {
      ostringstream out;
      out << "v" << setw(width) << setfill('0') << count++;
      get(vertex_name, g, v) = out.str();
    }
}

void
move(Vertex v, const Graph &g, std::set<Vertex> &lonely,
     std::set<Vertex> &connected, std::set<Vertex> &saturated)
{
  lonely.erase(v);

  if (num_vertices(g) >= 3)
    connected.insert(v);
  else
    saturated.insert(v);
}

void
move_if_needed(Vertex v, const Graph &g, std::set<Vertex> &connected,
               std::set<Vertex> &saturated)
{
  int n = num_vertices(g);
  int od = out_degree(v, g);

  // A node can have the out degree of at most (n - 1).
  assert(od < n);

  // A node is saturated if its out degree is (n - 1).
  if (od == n - 1)
    {
      connected.erase(v);
      saturated.insert(v);
    }
}

int
benes_interconnect(Graph &g, const vector<Vertex> &inputs,
		   const vector<Vertex> &outputs)
{
  int size = inputs.size();
  assert(size == outputs.size());

  if (size == 2)
    {
      Vertex nv = add_vertex(g);
      // Inputs to the node
      add_edge(inputs[0], nv, g);
      add_edge(inputs[1], nv, g);
      // Outputs of the node
      add_edge(nv, outputs[0], g);
      add_edge(nv, outputs[1], g);
    }
  else
    assert(("I shouldn't have gotten here.", false));
}

int
generate_benes_graph(Graph &g, int nodes)
{
  assert(num_vertices(g) == 0);
  assert(pop_count(nodes) == 1);
  assert(nodes != 1);

  // Create a graph with n
  g = Graph(nodes);

  // This is the vector of nodes.
  vector<Vertex> von(nodes);
  copy(vertices(g).first, vertices(g).second, von.begin());
  benes_interconnect(g, von, von);
}
