#include "arr_queue.hpp"
#include "distro.hpp"
#include "nodistro.hpp"
#include "poisson.hpp"
#include "polynomial.hpp"
#include "rou_order.hpp"
#include "utils.hpp"
#include "utils_ana.hpp"

#include <cassert>
#include <ctime>
#include <queue>


void
make_hop(const Graph &g, const map<Vertex, dist_poly> &v,
         const trans_matrix &T, map<Vertex, dist_poly> &result,
         map<Edge, dist_poly> &used_links)
{
  assert(result.empty());

  // Here we iterate over the rows of the matrix.
  for(trans_matrix::const_iterator i = T.begin(); i != T.end(); ++i)
    {
      // Here we iterate over the columns of the row.
      for(trans_matrix::mapped_type::const_iterator
            j = i->second.begin(); j != i->second.end(); ++j)
        {
          // The polynomial over_link expresses the packet transition
          // over the link from node number j->first to node number
          // i->first.  The expression j->second is the (i, j) element
          // of the transition matrix, and we multiply it by the
          // element number j->first of the vector v.

          map<Vertex, dist_poly>::const_iterator pp = v.find(j->first);

          if (pp == v.end())
            continue;

          dist_poly over_link = j->second * pp->second;
          
          // We don't bother is no transition is made.  This can be
          // because there's no link or because there's no packet.
          if (!over_link.empty())
            {
              // This is the rate of packets that arrive at node
              // number i->first.
              result[i->first] += over_link;
              // Here we remember the traffic that passes over this
              // link.

              Edge e = edge(j->first, i->first, g).first;
              used_links[e] += over_link;
            }
        }
    }
}


map<Vertex, double>
admit_ana(distro& apd, int v, const map<Vertex, double> &betas)
{
  // This is the distribution vector used for generating packets.
  vector<distro> distros;
  distros.push_back(apd);

  // This is the total number of packets that ask admission.
  {
    double beta = 0;
    for(map<Vertex, double>::const_iterator
	  i = betas.begin(); i != betas.end(); ++i)
      beta += i->second;
    
    if (beta == 0)
      distros.push_back(nodistro());
    else
      distros.push_back(poisson(beta));
  }
  
  // This is the average number of admitted packets.
  double beta_prime = 0;

  // This is the average number of packets asking admission.
  double beta = 0;

  // The queue of arrangements with the given distros.
  arr_queue q(distros);
  vector<int> arr;
  double prob;

  // Now process one arrangement per one iteration.
  while(q.find_next(arr, prob))
    {
      // Number of packets in transit.
      int a = distros[0].get_ith_max(arr[0]).second;
      // Number of packets requesting admission.
      int b = distros[1].get_ith_max(arr[1]).second;
      // Number of remaining output slots.
      int r = max(v - a, 0);

      // This is the number of admitted packets in this arrangement.
      int b_prime = min(r, b);

      beta_prime += b_prime * prob;
      beta += b * prob;
    }

  double rho = 0;

  if (beta > 0) 
    rho = beta_prime / beta;

  // This is the returned object.
  map<Vertex, double> betas_prime;
  
  for(map<Vertex, double>::const_iterator
        i = betas.begin(); i != betas.end(); ++i)  
    betas_prime[i->first] = rho * i->second;

  return betas_prime;
}


map<Vertex, distro>
admit_ana_distro(distro &apd, int v, const map<Vertex, double> &betas)
{
  map<Vertex, double> betas_prime = admit_ana(apd, v, betas);
  map<Vertex, distro> distros;
  
  for(map<Vertex, double>::const_iterator
        i = betas_prime.begin(); i != betas_prime.end(); ++i)
    distros[i->first] = poisson(i->second);
  
  return distros;
}


void
route_arr(const Graph &g, Vertex j, const map<Vertex, int> &arr,
          edge_count_map &count)
{
  // We want an empty object that we are supposed to fill in.
  assert(count.empty());
  
  // This avail map stores the number of wavelengths left on each edge
  // that leaves node j.
  map<Edge, int> avail;
  BGL_FORALL_OUTEDGES(j, edge, g, Graph)
    avail[edge]= get(edge_weight2, g, edge);

  // We route packets depending only on their destination node.  We
  // get the right order with this priority queue.
  priority_queue<Vertex, vector<Vertex>, rou_order> q(rou_order(g, j));
  for(map<Vertex, int>::const_iterator i = arr.begin(); i != arr.end(); ++i)
    q.push(i->first);

  // We route one entry of arr per one iteration of this loop.
  while(!q.empty())
    {
      // This is the destination node of packets.
      Vertex i = q.top();
      q.pop();
      
      // The number of packets that go to node i.
      map<Vertex, int>::const_iterator iter = arr.find(i);
      assert(iter != arr.end());
      int np = iter->second;
      
      // We route one packet per one iteration of this loop.  There
      // are the count packets to route.
      while(np--)
        {
          // Preferences of packets at node j destined to node i.
          const packet_prefs &prefs = get(vertex_bundle, g, j)[i];
          
          // Here we iterate over the packet preferences.  We start
          // with the most desired packet preference.
          for(packet_prefs::const_iterator
                v = prefs.begin(); v != prefs.end(); ++v)
            {
              // Find the edge to which the packet preferences refer.
              Edge e = edge(j, *v, g).first;
              map<Edge, int>::iterator l = avail.find(e);
              assert(l != avail.end());
              
              // We check if there is an available wavelength on
              // that edge.  If so, we take the wavelength.
              if (l->second)
                {
                  // We take one wavelength for the packet.
                  l->second--;
                  
                  // In the result object we find the edge_probs
                  // object for destination with index *i.
                  ++count[i][e];
                  
                  break;
                }
            }
        }
    }
}


void
arr_route_prob(const Graph &g, Vertex j, const map<Vertex, int> &arr,
               edge_probs_map &probs)
{
  // We want an empty return argument.
  assert(probs.empty());

  edge_count_map count;
  route_arr(g, j, arr, count);

  for(edge_count_map::const_iterator
        vi = count.begin(); vi != count.end(); ++vi)
    for(edge_count::const_iterator
          ei = vi->second.begin(); ei != vi->second.end(); ++ei)
      {
        map<Vertex, int>::const_iterator fi = arr.find(vi->first);
        assert(fi != arr.end());
        double prob = double(ei->second) / double(fi->second);
        probs[vi->first][ei->first] = prob;
      }
}


void
route_ana(const Graph &g, Vertex j, const map<Vertex, distro> &mus,
          edge_probs_map &probs)
{
  // We want an empty return argument.
  assert(probs.empty());

  // Element aggr[n] stores the sum of P_{rou}(e) * x_{i,n,e} over n.
  map<Vertex, double> aggr;

  // Before we start figuring out where to send packets, we first need
  // to calculate the vectors of probabilities of getting some
  // packets.
  vector<Vertex> vertexes;
  vector<distro> distros;

  for(map<Vertex, distro>::const_iterator i = mus.begin(); i != mus.end(); i++)
    vertexes.push_back(i->first), distros.push_back(i->second);

  arr_queue q(distros);
  vector<int> marr;
  double prob;

  int count = 1000;

  // Now process one arrangement of packets per one iteration.
  while(count-- != 0 && q.find_next(marr, prob))
    {
      // We fill in the arrangement for routing.
      map<Vertex, int> arr;
      for(int i = 0; i < marr.size(); ++i)
        {
          Vertex v = vertexes[i];
          int count = distros[i].get_ith_max(marr[i]).second;

          if (count)
            {
              aggr[v] += prob * count;
              arr[v] = count;
            }
        }

      // Here we calculate the probabilities that a packet is sent to
      // given outputs.
      edge_probs_map arr_probs;
      arr_route_prob(g, j, arr, arr_probs);

      for(edge_probs_map::iterator
            vi = arr_probs.begin(); vi != arr_probs.end(); ++vi)
        vi->second *= prob * arr[vi->first];

      probs += arr_probs;
    }

  // Here we are normalizing the edge probabilities.
  for(edge_probs_map::iterator
        vi = probs.begin(); vi != probs.end(); ++vi)
    vi->second *= 1.0 / aggr[vi->first];
}
