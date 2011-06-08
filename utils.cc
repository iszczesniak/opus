#include "generate.hpp"
#include "graph.hpp"
#include "packet.hpp"
#include "poisson.hpp"
#include "polynomial.hpp"
#include "rou_order.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

#include <boost/graph/connected_components.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace std;

void complete_graph(Graph &g)
{
  // Here we do all-pair shortest path.  We could use the BGL
  // implementations of Floyd-Warshal or the Johnson algorithms here,
  // but they return only the distances.  They don't return the
  // parents, which we need.
  vector<int> dist(num_vertices(g));
  vector<Vertex> pred(num_vertices(g));

  BGL_FORALL_VERTICES (v, g, Graph)
    {
      dijkstra_shortest_paths
        (g, v, predecessor_map(&pred[0]).distance_map(&dist[0]));

      put(vertex_distance, g, v, dist);
      put(vertex_predecessor, g, v, pred);
    }

  // Here we calculate the matrix of packet preferences.  The
  // pref[i][j] entry in the matrix tells us the preferences of a
  // packet when it's in node j and want to get to node i.
  BGL_FORALL_VERTICES(j, g, Graph)
    {
      vector<packet_prefs> v_prefs(num_vertices(g));

      BGL_FORALL_VERTICES(i, g, Graph)
        v_prefs[i] = make_prefs(i, j, g);

      put(vertex_bundle, g, j, v_prefs);
    }
}

map<string, Vertex>
get_vertex_names(const Graph& g)
{
  map<string, Vertex> result;

  for (boost::graph_traits<Graph>::vertex_iterator
         vi = vertices(g).first; vi != vertices(g).second; ++vi)
    result[get(vertex_name, g, *vi)] = *vi;

  return result;
}

bool
read_graphviz_filename(const char* file_name, Graph &g)
{
  ifstream is(file_name);

  if (!is)
    {
      cerr << "Error opening a graphviz file '" << file_name << "'.\n";
      return false;
    }

  dynamic_properties dp;

  dp.property("node_id", get(vertex_name, g));
  dp.property("distance", get(edge_weight, g));
  dp.property("lambdas", get(edge_weight2, g));

  bool result = read_graphviz(is, g, dp);

  complete_graph(g);

  is.close();

  return result;
}

bool
load_tm_file(const char *file_name, const map<string, Vertex> &vertex_names,
             fp_matrix &tm)
{
  ifstream is(file_name);
  if (!is)
    {
      cerr << "Error opening a traffic matrix file '" << file_name << "'.\n";
      return false;
    }

  bool result = load_tm(is, vertex_names, tm);
  is.close();

  return result;
}

bool
load_tm(istream &is, const map<string, Vertex> &vertex_names,
        fp_matrix &tm)
{
  // This matches an empty line.
  std::string comm_str = "^\\s*$";
  boost::regex comm_regex;
  comm_regex.assign(comm_str, boost::match_default | boost::format_perl);

  // This matches a data line.
  std::string data_str =
    "^\\s*" // we don't care about leading blanks
    "(\\S+)" // the first identifier
    "\\s*->\\s*" // the separator beteen identifiers
    "(\\S+)" // the second identifier
    "\\s*\\:\\s*" // the separator preceeding data
    "(\\.\\d+|\\d+\\.\\d*|\\d+)"; // the data
  boost::regex data_regex;
  data_regex.assign(data_str, boost::match_default | boost::format_perl);

  // Now we read the input line-by-line.
  for(string line; getline(is, line);)
    {
      boost::smatch what;

      string::size_type pos = line.find('#');
      if (pos != string::npos)
        line.erase(pos);

      // Match a data line.
      if (boost::regex_search(line, what, data_regex))
        {
          // Reading the number here shouldn't fail, because we
          // matched it with a regular expression.
          double load;
          assert(std::istringstream(what[3]) >> load);

          map<string, Vertex>::const_iterator i1 = vertex_names.find(what[1]);
          map<string, Vertex>::const_iterator i2 = vertex_names.find(what[2]);

          string missing_name;

          if (i1 == vertex_names.end())
            missing_name = what[1];

          if (i2 == vertex_names.end())
            missing_name = what[2];

          if (!missing_name.empty())
            {
              cerr << "Vertex name '" << missing_name << "' is used in the "
                   << "traffic matrix file, but was not defined "
                   << "in the dot file.\n";
              return false;
            }

          assert(load != 0);
          tm[i2->second][i1->second] = load;
        }
      // Match a comment or report a wrong line.
      else if (!boost::regex_search(line, what, comm_regex))
        {
          cerr << "WRONG LINE: " << line << endl;
          return false;
        }
    }

  return true;
}

int
get_distance(Vertex i, Vertex j, const Graph &g)
{
  return get(vertex_distance, g, i)[j];
}

int
get_output_capacity(const Graph& g, Vertex j)
{
  int v = 0;

  BGL_FORALL_OUTEDGES(j, edge, g, Graph)
    v += get(edge_weight2, g, edge);

  return v;
}

std::string
trace_path_back(Vertex i, Vertex j, const Graph &g)
{
  std::ostringstream str;

  if (i != j)
    {
      Vertex prev = get(vertex_predecessor, g, i)[j];
      str << trace_path_back(i, prev, g) << " -> "
	  << get(vertex_name, g, j);
    }
  else
    str << get(vertex_name, g, i);

  return str.str();
}

std::string
path_to_string(Vertex i, Vertex j, const Graph &g)
{
  std::ostringstream str;

  str << "From " << get(vertex_name, g, i)
      << " to " << get(vertex_name, g, j);

  if (get(vertex_predecessor, g, i)[j] != j)
    str << ": " << trace_path_back(i, j, g)
	<< ", distance = " << get_distance(i, j, g);
  else
    str << " doesn't exist.";

  return str.str();
}

std::string
to_string(Edge e, const Graph &g)
{
  std::string str;

  str += "(";
  str += get(vertex_name, g, source(e, g));
  str += ", ";
  str += get(vertex_name, g, target(e, g));
  str += ")";

  return str;
}

void
print_nn(const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "NETWORK NODES\n";
  os << "******************************************************\n";

  BGL_FORALL_VERTICES(v, g, Graph)
    os << "Node #" << v << " is " << get(vertex_name, g, v) << endl;
}

void
print_nl(const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "NETWORK LINKS\n";
  os << "******************************************************\n";

  BGL_FORALL_EDGES(e, g, Graph)
    os << "Link " << to_string(e, g) << ": "
       << get(edge_weight, g, e) << "km, "
       << get(edge_weight2, g, e) << " wavelengths" << endl;
}

void
print_tm(const Graph &g, const fp_matrix &tm, std::ostream &os)
{
  os << "#******************************************************\n";
  os << "#TRAFFIC MATRIX\n";
  os << "#******************************************************\n";

  BGL_FORALL_VERTICES(i, g, Graph)
    {
      double beta = 0;

      os << "# Demands from node " << get(vertex_name, g, i)
         << ":" << endl;

      BGL_FORALL_VERTICES(j, g, Graph)
        if (tm.exists(j, i))
          {
            double rate = tm.at(j, i);
            assert(rate != 0);

            os << get(vertex_name, g, i) << " -> "
               << get(vertex_name, g, j) << ": "
               << rate << endl;
            beta += rate;
          }

      os << "# sum = " << beta << endl;
    }
}

void
print_sp(const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "SHORTEST PATHS\n";
  os << "******************************************************\n";

  // Lists the path from i to j.
  BGL_FORALL_VERTICES(i, g, Graph)
    BGL_FORALL_VERTICES(j, g, Graph)
      os << path_to_string(i, j, g) << std::endl;
}

void
print_pp(const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "PACKET PREFERENCES\n";
  os << "******************************************************\n";

  // For each current node j.
  BGL_FORALL_VERTICES(j, g, Graph)
    {
      os << "Packet preferences at node "
         << get(vertex_name, g, j) << ":" << endl;

      // For each destination node i.
      BGL_FORALL_VERTICES(i, g, Graph)
        if (i != j)
          {
            os << "Destination " << get(vertex_name, g, i) << ": "
               << make_prefs(i, j, g) << endl;
          }
    }
}

void
print_rp(const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "ROUTING PRIORITIES\n";
  os << "******************************************************\n";

  // For each current node j.
  BGL_FORALL_VERTICES(j, g, Graph)
    {
      vector<Vertex> order;

      // For each destination node i.
      BGL_FORALL_VERTICES(i, g, Graph)
        if (i != j)
          order.push_back(i);

      sort(order.begin(), order.end(), rou_order(g, j));

      os << "Routing priority at node "
         << get(vertex_name, g, j) << ": ";

      for(vector<Vertex>::iterator
            v = order.begin(); v != order.end(); ++v)
        os << get(vertex_name, g, *v) << ", ";

      os << endl;
    }
}

void
print_input(const Graph &g, const fp_matrix &tm, std::ostream &os)
{
  print_nn(g, os);
  print_nl(g, os);
  print_tm(g, tm, os);
  print_sp(g, os);
  print_pp(g, os);
  print_rp(g, os);
}

void
print_nf(const pp_matrix &ppm, const pt_matrix &ptm,
         const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "NODE FLOWS\n";
  os << "******************************************************\n";

  fp_matrix itm;
  generate_itm(g, ptm, itm);

  // j is the current node.
  BGL_FORALL_VERTICES(j, g, Graph)
    {
      os << "Flows for node " << get(vertex_name, g, j) << endl;

      double alfa = 0;
      double alfa_prime = 0;
      double alfa_jj = 0;

      // nj is the current node, ni is the destination node
      FOREACH_MATRIX_ELEMENT(itm, ni, nj, e, fp_matrix)
        if (nj == j)
          {
            alfa += e;

            if (ni == j)
              alfa_jj = e;
            else
              alfa_prime += e;

            os << "\tRate of packets from neighbor nodes to node "
               << get(vertex_name, g, ni) << " = " << e << endl;
          }

      os << "\tRate of all packets from neighbors = " << alfa << endl;
      os << "\tPackets in transit = " << alfa_prime << endl;
      os << "\tPackets that go to local drop = " << alfa_jj << endl;
      os << endl;

      double beta = 0;
      double beta_prime = 0;

      // nj is the current node, ni is the destination node.
      FOREACH_MATRIX_ELEMENT(ppm, ni, nj, pp, pp_matrix)
        if (nj == j)
          {
            // This is the rate at which packets of demand (nj, ni)
            // were admitted.
            double rate = get_adm_distro(ppm, nj, ni).mean();
            beta_prime += rate;

            os << "\tAdmittance rate of demand ("
               << get(vertex_name, g, nj)
               << ", " << get(vertex_name, g, ni)
               << ") = " << rate << endl;
          }

      os << "\tAdmittance rate of all demands = "
         << beta_prime << endl;

      double micro_prime = alfa_prime + beta_prime;

      os << "\tRate of packets that go to routing = "
         << micro_prime << endl;

      os << endl;
      os << "Distribution of packets that ask routing:" << endl;

      BGL_FORALL_VERTICES(i, g, Graph)
        if (j != i)
          {
            distro d = sum(node_dist_poly(j, i, ppm, true));

            if (!d.is_nodistro())
              {
                os << "\tFor destination " << get(vertex_name, g, i)
                   << ": " << d;

                if (d.is_tabdistro())
                  os << ":" << endl << d.to_tabular();

                os << endl;
              }
          }

      os << endl;
    }
}

void
print_ll(const map<Edge, double> &ll,
         const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "LINK LOADS\n";
  os << "******************************************************\n";

  double avg_load = 0;

  for(map<Edge, double>::const_iterator i = ll.begin(); i != ll.end(); ++i)
    {
      os << "Link " << to_string(i->first, g) << ": "
         << 100 * i->second << "%\n";

      avg_load += i->second;
    }

  avg_load /= ll.size();

  os << "AVERAGE: " << 100 * avg_load << "%\n";
}

void
print_pr(const pp_matrix &ppm, const pt_matrix &ptm,
         const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << " PROBABILITIES OF ROUTING\n";
  os << "******************************************************\n";

  // j is the current node.
  BGL_FORALL_VERTICES(j, g, Graph)
    {
      os << "------------------------------------" << endl;

      os << "Probabilities of routing at node "
         << get(vertex_name, g, j) << ":" << endl;

      edge_probs_map probs = calc_edge_probs_map(g, j, ppm, ptm);

      print_edge_probs_map(os, probs, g);
    }
}

void
print_ppm(const pp_matrix &ppm, const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "PACKET PRESENCE MATRIX\n";
  os << "******************************************************\n";

  // i is the destination node, j is the source node
  FOREACH_MATRIX_ELEMENT(ppm, i, j, pp, pp_matrix)
    {
      os << "Packet presence for demand ("
         << get(vertex_name, g, j) << ", "
         << get(vertex_name, g, i) << "):" << endl;

      for (packet_presence::const_iterator
             k = pp.begin(); k != pp.end(); ++k)
        {
          os << "Hop #" << k->first << endl;

          for(packet_presence::mapped_type::const_iterator
                l = k->second.begin(); l != k->second.end(); ++l)
            os << "Node " << get(vertex_name, g, l->first)
               << ": " << l->second << endl;
        }
    }
}

void
print_ptm(const pt_matrix &ptm, const Graph &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "PACKET TRAJECTORY MATRIX\n";
  os << "******************************************************\n";

  // i is the destination node, j is the source node
  FOREACH_MATRIX_ELEMENT(ptm, i, j, pt, pt_matrix)
    {
      os << "Packet trajectory for demand ("
         << get(vertex_name, g, j) << ", "
         << get(vertex_name, g, i) << "):" << endl;

      for (packet_trajectory::const_iterator
             k = pt.begin(); k != pt.end(); ++k)
        {
          os << "Hop #" << k->first << endl;

          for(packet_trajectory::mapped_type::const_iterator
                l = k->second.begin(); l != k->second.end(); ++l)
            os << "Link " << to_string(l->first, g) << ": "
               << l->second << endl;
        }
    }
}

void
print_output(const pp_matrix &ppm, const pt_matrix &ptm,
             const Graph &g, std::ostream &os)
{
  // Stores the results: the link load.
  map<Edge, double> ll;
  calculate_ll(ptm, ll, g);
  print_ll(ll, g, os);

  // print_nf(ppm, ptm, g, os);
  // print_pr(ppm, ptm, g, os);
  print_ppm(ppm, g, os);
  print_ptm(ptm, g, os);
}

void
calculate_ll(const pt_matrix &ptm, map<Edge, double> &ll, const Graph &g)
{
  assert(ll.empty());

  // In calculating the link loads we take into account each data of
  // the ptm matrix.  The task is to figure out where exactly the data
  // should be put.
  FOREACH_MATRIX_ELEMENT(ptm, i, j, e, pt_matrix)
    // Iterate over the time slot of the packet trajectory.
    for(packet_trajectory::const_iterator
          t = e.begin(); t != e.end(); ++t)
      // Iterate over the link of the time slot.
      for(packet_trajectory::mapped_type::const_iterator
            l = t->second.begin(); l != t->second.end(); ++l)
        ll[l->first] += sum(l->second).mean();

  // Divide by the link capacity in order to get the utilization.
  for(map<Edge, double>::iterator i = ll.begin(); i != ll.end(); ++i)
    i->second /= get(edge_weight2, g, i->first);
}

void
calculate_at(const pp_matrix &ppm, dp_matrix &at)
{
  // For every demand that starts at node j and goes to node i.
  FOREACH_MATRIX_ELEMENT(ppm, i, j, e, pp_matrix)
    {
      // Find the element for hop number 0.
      packet_presence::const_iterator e0 = e.find(0);
      assert(e0 != e.end());
      // Find the element for node j.
      map<Vertex, dist_poly>::const_iterator e0j = e0->second.find(j);
      assert(e0j != e0->second.end());
      // This is the dist_poly for the admitted traffic.
      const dist_poly &dp = e0j->second;
      at[i][j] = dp;
    }
}

void
calculate_dt(const pp_matrix &ppm, dp_matrix &dt)
{
  // For every demand that starts at node j and goes to node i.
  FOREACH_MATRIX_ELEMENT(ppm, i, j, e, pp_matrix)
    // Iterate over the hops of a packet_presence.
    for(packet_presence::const_iterator t = e.begin(); t != e.end(); ++t)
      {
	map<Vertex, dist_poly>::const_iterator ti = t->second.find(i);
	// Make sure the packet arrives at the node i after that hop.
	if (ti != t->second.end())
	  {
	    // This distribution polynomial describes the packets that
	    // arrive at node i.
	    const dist_poly &dp = ti->second;
	    dt[i][j] += dp;
	  }
      }
}

dist_poly
node_dist_poly(Vertex j, Vertex i, const pp_matrix &ppm, bool admitted)
{
  dist_poly d;

  // Here we get a map of packet_presence objects of demands with the
  // destination node i.
  pp_matrix::const_iterator ii = ppm.find(i);

  if (ii != ppm.end())
    {
      // Here we iterate over the demands found in the map ii.second.
      // The source node is ji.first, but it really doesn't matter.
      for(pp_matrix::mapped_type::const_iterator
            ji = ii->second.begin(); ji != ii->second.end(); ++ji)
        // Then for each packet_presence iterate over the hops.
        for(packet_presence::const_iterator
              hi = ji->second.begin(); hi != ji->second.end(); ++hi)
          // Skip the just-admitted packets unless admitted is true.
          if (hi != ji->second.begin() || admitted)
            {
              packet_presence::mapped_type::const_iterator

                vi = hi->second.find(j);

              if (vi != hi->second.end())
                d += vi->second;
            }
    }

  return d;
}

dist_poly
link_dist_poly(Edge e, Vertex i, const pt_matrix &ptm)
{
  dist_poly d;

  // Here we get a map of packet_trajectory objects of demands with
  // the destination node i.
  pt_matrix::const_iterator ii = ptm.find(i);

  if (ii != ptm.end())
    {
      // Here we iterate over the demands found in the map ii.second.
      // The source node is ji.first, but it really doesn't matter.
      for(pt_matrix::mapped_type::const_iterator
            ji = ii->second.begin(); ji != ii->second.end(); ++ji)
        // Then for each packet_trajectory iterate over the hops.
        for(packet_trajectory::const_iterator
              hi = ji->second.begin(); hi != ji->second.end(); ++hi)
          {
            packet_trajectory::mapped_type::const_iterator
              ei = hi->second.find(e);

            if (ei != hi->second.end())
              d += ei->second;
          }
    }

  return d;
}

edge_probs_map
calc_edge_probs_map(const Graph &g, Vertex j,
                    const pp_matrix &ppm, const pt_matrix &ptm)
{
  // Rate per destination per edge.  The element rpdpe[v][e] is the
  // rate of packets that are destined to node v and that are going
  // through link e.
  map<Vertex, map<Edge, double> > rpdpe;

  // Rate per destination.  The element rpd[v] is the rate of packets
  // that are destined to node v.
  map<Vertex, double> rpd;

  // We iterate over the ppm to get the rate of packets present at
  // node j that want to go to node i.
  BGL_FORALL_VERTICES(i, g, Graph)
    {
      dist_poly dp = node_dist_poly(j, i, ppm, true);
      double rate = sum(dp).mean();

      if (rate != 0)
        rpd[i] += rate;
    }

  // We iterate over the ptm to get the rate of packets destined to
  // node i that are traversing link.  Links e leave node j.
  BGL_FORALL_VERTICES(i, g, Graph)
    BGL_FORALL_OUTEDGES(j, e, g, Graph)
      {
        dist_poly dp = link_dist_poly(e, i, ptm);
        double rate = sum(dp).mean();

        if (rate != 0)
          rpdpe[i][e] += rate;
      }

  // Now we divide the numbers in rpdpe to get the mean probabilities.
  for(edge_probs_map::iterator i = rpdpe.begin(); i != rpdpe.end(); ++i)
    i->second *= 1.0 / rpd[i->first];

  return rpdpe;
}

distro
get_adm_distro(const pp_matrix &ppm, Vertex j, Vertex i)
{
  distro adm_distro;

  // Here we get a map of packet_presence objects of demands with the
  // destination node i.
  pp_matrix::const_iterator ii = ppm.find(i);

  if (ii != ppm.end())
    {
      pp_matrix::mapped_type::const_iterator ji = ii->second.find(j);

      if (ji != ii->second.end())
        {
          const packet_presence& pp = ji->second;

          // We assume the packet presence has at least one entry for
          // the slot 0 when it's admitted.
          assert(pp.size());

          // We assume the packet was admitted at time slot 0.
          assert(!(pp.begin()->first));

          const map<Vertex, dist_poly> &mvp = pp.begin()->second;

          // We assume the packet was admitted at node j.
          assert(mvp.begin()->first == j);

          // This is the polynomial that describes the admitted
          // packet.
          const dist_poly &poly = mvp.begin()->second;

          // We assume the polynomial has only one term.
          assert(poly.size() == 1);

          // This is the pair that describes the first polynomial
          // term.
          const pair<int, distro> &pr = *(poly.begin());

          // The first pair describes the term of the power 0.
          assert(pr.first == 0);

          // This is the rate at which packets of demand (nj, ni)
          // were admitted.
          adm_distro = pr.second;
        }
    }

  return adm_distro;
}

// The function for sorting the list of sets.
static bool stlos(const set<Vertex> &s1, const set<Vertex> &s2)
{
  return s1.size() > s2.size();
}

list<set<Vertex> >
get_components(const Graph &g)
{
  // An element of this vector corresponds to a vertex, i.e. c[v] is
  // the component number that vertex v belongs to.
  std::vector<int> c(num_vertices(g));
  // "num" is the number of connected components.
  int num = connected_components(g, &c[0]);

  // Each element of the list is a set that contains vertexes
  // belonging to a component.
  list<set<Vertex> > l(num);
  // In every iteration add one vertex to the appropriate set.
  for (std::vector<int>::iterator i = c.begin(); i != c.end(); ++i)
    {
      list<set<Vertex> >::iterator li = l.begin();
      advance(li, *i);

      // Iterator i refers to the element for vertex v.
      Vertex v = distance(c.begin(), i);
      li->insert(v);
    }

  l.sort(stlos);

  return l;
}

bool
check_components(const Graph &g)
{
  list<set<Vertex> > l = get_components(g);

  int connected = 0;
  // Count the number of connected components.
  for (list<set<Vertex> >::iterator i = l.begin(); i != l.end(); ++i)
    connected += (i->size() >= 2);

  return connected == 1;
}
