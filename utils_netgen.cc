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
  else if (size == 4)
    {
      // Top nodes of the interconnection network.
      Vertex t1 = add_vertex(g);
      Vertex t2 = add_vertex(g);
      Vertex t3 = add_vertex(g);

      // Bottom nodes of the interconnection network.
      Vertex b1 = add_vertex(g);
      Vertex b2 = add_vertex(g);
      Vertex b3 = add_vertex(g);

      // Inputs to interconnection network.
      add_edge(inputs[0], t1, g);
      add_edge(inputs[1], t1, g);
      add_edge(inputs[2], b1, g);
      add_edge(inputs[3], b1, g);

      // Outputs of the interconnection network.
      add_edge(t3, outputs[0], g);
      add_edge(t3, outputs[1], g);
      add_edge(b3, outputs[2], g);
      add_edge(b3, outputs[3], g);

      // The links of the interconnection network.
      add_edge(t1, t2, g);
      add_edge(t2, t3, g);
      add_edge(b1, b2, g);
      add_edge(b2, b3, g);
      add_edge(t1, b2, g);
      add_edge(b1, t2, g);
      add_edge(t2, b3, g);
      add_edge(b2, t3, g);
    }
  else
    {
      int s2 = size / 2;
      vector<Vertex> in_stage(s2), out_stage(s2);

      // Create the new input stage.
      for (int i = 0; i < s2; ++i)
	{
	  Vertex v = add_vertex(g);
	  in_stage[i] = v;
	  add_edge(inputs[2 * i], v, g);
	  add_edge(inputs[2 * i + 1], v, g);
	}

      // Create the new output stage.
      for (int i = 0; i < s2; ++i)
	{
	  Vertex v = add_vertex(g);
	  out_stage[i] = v;
	  add_edge(v, inputs[2 * i], g);
	  add_edge(v, inputs[2 * i + 1], g);
	}

      benes_interconnect(g, in_stage, out_stage);
      benes_interconnect(g, in_stage, out_stage);
    }
}

int
generate_benes_graph(Graph &g, int n)
{
  assert(num_vertices(g) == 0);
  assert(pop_count(n) == 1);
  assert(n != 1);

  // Create a graph with n nodes
  g = Graph(n);

  // This is the vector of nodes.
  vector<Vertex> von(n);
  copy(vertices(g).first, vertices(g).second, von.begin());
  benes_interconnect(g, von, von);
}
