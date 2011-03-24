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

  // This is the current node that we study.
  Vertex j = vn["b"];

  // The distribution of the number of packets in transit.
  distro in_transit;
  BGL_FORALL_VERTICES(i, g, Graph)
    if (i != j)
      in_transit += sum(node_dist_poly(j, i, sim_ppm, false));

  // In this block we output the distribution of the packets in
  // transit.
  {
    // Output some information to compare with SVN.  Prefs of packets
    // that go from node b to node a, c, d, e, f.
    ofstream out("test_adm_it.gnuplot");

    out << "# Network file: " << dot_name << endl;
    out << "# Traffic matrix file: " << dot_name << endl;
    out << "# TLL = " << TTL << endl;
    out << "# MLL = " << MLL << endl;
    out << endl;

    out << "set terminal postscript eps\n";
    out << "set boxwidth 0.8\n";
    out << "set size 0.65, 0.5\n";
    out << "set border 15 lw 0.5\n";
    out << "set xtics (\"0\" 0, \"10\" 10, \"20\" 20, \"30\" 30)\n";
    out << "set style histogram clustered gap 1\n";
    out << "set style data histograms\n";
    out << endl;
    out << "set output \"test_adm_it.eps\"\n";
    out << "set xlabel \"number of packets\"\n";
    out << "set ylabel \"probability\"\n";
    out << "set key right\n";
    out << endl;
    out << "plot [-0.5:30.5]"
        << "'-' using 2 title \"analysis\" fs solid 1.00 lt -1 lw 0.5, "
        << "'-' using 2 title \"simulation\" fs solid 0.25 lt -1 lw 0.5\n";

    out << poisson(in_transit.mean()).to_tabular();
    out << "e" << endl;

    out << in_transit.to_tabular();
    out << "e" << endl;

    out.close();
  }

  distro apd = poisson(in_transit.mean());
  map<Vertex, double> betas;
  betas[vn["a"]] = tm[vn["a"]][j];
  betas[vn["c"]] = tm[vn["c"]][j];
  betas[vn["d"]] = tm[vn["d"]][j];
  betas[vn["e"]] = tm[vn["e"]][j];
  betas[vn["f"]] = tm[vn["f"]][j];
  map<Vertex, double> betas_prime;
  int v = get_output_capacity(g, j);
  betas_prime = admit_ana(apd, v, betas);

  cout << "ANALYSIS: Rate of admitted packets at node "
       << get(vertex_name, g, j) << ": " << endl;
  cout << "Going to node a: " << betas_prime[vn["a"]] << endl;
  cout << "Going to node c: " << betas_prime[vn["c"]] << endl;
  cout << "Going to node d: " << betas_prime[vn["d"]] << endl;
  cout << "Going to node e: " << betas_prime[vn["e"]] << endl;
  cout << "Going to node f: " << betas_prime[vn["f"]] << endl;

  cout << "SIMULATION: Rate of admitted packets at node "
       << get(vertex_name, g, j) << ": " << endl;
  cout << "Going to node a: "
       << get_adm_distro(sim_ppm, j, vn["a"]) << endl;
  cout << "Going to node c: "
       << get_adm_distro(sim_ppm, j, vn["c"]) << endl;
  cout << "Going to node d: "
       << get_adm_distro(sim_ppm, j, vn["d"]) << endl;
  cout << "Going to node e: "
       << get_adm_distro(sim_ppm, j, vn["e"]) << endl;
  cout << "Going to node f: "
       << get_adm_distro(sim_ppm, j, vn["f"]) << endl;

  // In this block we output the distribution of the admitted packets.
  list<Vertex> vlist;
  vlist.push_back(vn["a"]);
  vlist.push_back(vn["c"]);
  vlist.push_back(vn["d"]);
  vlist.push_back(vn["e"]);
  vlist.push_back(vn["f"]);

  for(list<Vertex>::iterator ii = vlist.begin(); ii != vlist.end(); ++ii)
    {
      Vertex i = *ii;
      string vname = get(vertex_name, g, i);
      string gnuplot_fname = "test_adm_" + vname + ".gnuplot";
      string eps_fname = "test_adm_" + vname + ".eps";
      ofstream out(gnuplot_fname.c_str());

      // Output some information to compare with SVN.
      out << "# Network file: " << dot_name << endl;
      out << "# Traffic matrix file: " << dot_name << endl;
      out << "# TLL = " << TTL << endl;
      out << "# MLL = " << MLL << endl;
      out << endl;

      out << "set terminal postscript eps\n";
      out << "set boxwidth 0.8\n";
      out << "set size 0.65, 0.5\n";
      out << "set border 15 lw 0.5\n";
      out << "set style histogram clustered gap 1\n";
      out << "set style data histograms\n";
      out << endl;
      out << "set output \"" << eps_fname << "\"\n";
      out << "set xlabel \"number of packets\"\n";
      out << "set ylabel \"probability\"\n";
      out << "set key right\n";
      out << endl;
      out << "plot "
<< "'-' using 2 title \"analysis\" fs solid 1.00 lt -1 lw 0.5, "
          << "'-' using 2 title \"simulation\" fs solid 0.25 lt -1 lw 0.5\n";

      out << poisson(betas_prime[i]).to_tabular();
      out << "e" << endl;

      out << get_adm_distro(sim_ppm, j, i).to_tabular();
      out << "e" << endl;

      out.close();
    }

  return 0;
}
