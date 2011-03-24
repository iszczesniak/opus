#include "analysis.hpp"
#include "edge_probs.hpp"
#include "graph.hpp"
#include "simulation.hpp"
#include "tabdistro.hpp"
#include "utils.hpp"
#include "utils_ana.hpp"
#include "utils_sim.hpp"

#include <fstream>
#include <iostream>
#include <map>

using namespace std;

int
main (int argc, char* argv[])
{
  cout.flags(ios::fixed);
  cout.precision(6);

  const char *dot_name = "../runs/six_node_net.dot";
  const char *tm_name = "../runs/six_node_net.tm";
  int TTL = 5;
  int MLL = 1000;

  // Here we create the graph.
  Graph g;
  if (!read_graphviz_filename(dot_name, g))
    {
      cerr << "Error loading the graphviz file.\n";
      return 1;
    }

  map<string, Vertex> vn = get_vertex_names(g);

  // Here we create the traffic matrix.
  fp_matrix tm;
  if (!load_tm_file(tm_name, vn, tm))
    {
      cerr << "Error loading the traffic matrix file.\n";
      return 1;
    }

  // Stores the simulation results: the packet presence matrix.
  pp_matrix sim_ppm;

  // Stores the simulation results: the packet trajectory matrix.
  pt_matrix sim_ptm;

  // Running simulation
  cerr << "\tRunning simulation... ";
  tie(sim_ppm, sim_ptm) = sim_solution(g, tm, TTL, MLL, cerr);
  cerr << "done." << endl;

  // These are the mean rates of packets that are at node x, and want
  // to go to node a, c, d, e and f.  This is the data from
  // simulation.
  map<Vertex, distro> sim_rou_input;

  // This is the current node that we study.
  Vertex j = vn["b"];

  BGL_FORALL_VERTICES(i, g, Graph)
    if (i != j)
      sim_rou_input[i] = sum(node_dist_poly(j, i, sim_ppm, true));

  {
    ofstream out("test_rou_i.gnuplot");

    // Output a some information to compare with SVN.  Prefs of
    // packets that go from node b to node a, c, d, e, f.
    out << "# Network file: " << dot_name << endl;
    out << "# Traffic matrix file: " << dot_name << endl;
    out << "# TLL = " << TTL << endl;
    out << "# MLL = " << MLL << endl;
    out << endl;
    out << "# Preferences from node b to nodes:" << endl;
    out << "# a: " << make_prefs(vn["a"], vn["b"], g) << endl;
    out << "# c: " << make_prefs(vn["c"], vn["b"], g) << endl;
    out << "# d: " << make_prefs(vn["d"], vn["b"], g) << endl;
    out << "# e: " << make_prefs(vn["e"], vn["b"], g) << endl;
    out << "# f: " << make_prefs(vn["f"], vn["b"], g) << endl;
    out << endl;

    out << "set terminal postscript eps\n";
    out << "set boxwidth 0.8\n";
    out << "set size 0.65, 0.5\n";
    out << "set border 15 lw 0.5\n";
    out << "set xtics scale 0\n";
    out << "set style histogram clustered gap 1\n";
    out << "set style data histograms\n";
    out << endl;
    out << "set output \"test_rou_i.eps\"\n";
    out << "set xlabel \"number of packets\"\n";
    out << "set ylabel \"probability\"\n";
    out << "set key right\n";
    out << endl;
    out << "plot [-0.5:9.5]"
        << "'-' using 2:xtic(1) title \"analysis\" fs solid 1.00 lt -1 lw 0.5, "
        << "'-' using 2 title \"simulation\" fs solid 0.25 lt -1 lw 0.5\n";

    out << poisson(sim_rou_input[vn["c"]].mean()).to_tabular();
    out << "e" << endl;

    out << sim_rou_input[vn["c"]].to_tabular();
    out << "e" << endl;
    out << endl;
    out << endl;
  }

  cout << "Destined to node a: " << sim_rou_input[vn["a"]].mean() << endl;
  cout << "Destined to node c: " << sim_rou_input[vn["c"]].mean() << endl;
  cout << "Destined to node d: " << sim_rou_input[vn["d"]].mean() << endl;
  cout << "Destined to node e: " << sim_rou_input[vn["e"]].mean() << endl;
  cout << "Destined to node f: " << sim_rou_input[vn["f"]].mean() << endl;

  // Run the analysis.
  cerr << "\tRunning analysis of routing... ";
  edge_probs_map ana_res;
  route_ana(g, j, sim_rou_input, ana_res);
  cerr << "done." << endl;

  edge_probs_map sim_res = calc_edge_probs_map(g, j, sim_ppm, sim_ptm);

  cout << "Simulation:" << endl;
  cout << sim_res << endl;
  cout << "Analysis:" << endl;
  cout << ana_res << endl;

  return 0;
}
