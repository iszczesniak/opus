#include <cassert>
#include "graph.hpp"
#include "edge_probs.hpp"
#include "utils.hpp"

edge_probs &
operator *= (edge_probs &v, double c)
{
  for(edge_probs::iterator i = v.begin(); i != v.end(); ++i)
    i->second *= c;

  return v;
}

edge_probs
operator * (double c, const edge_probs &v)
{
  edge_probs vn = v;
  vn *= c;

  return vn;
}

edge_probs &
operator += (edge_probs &v1, const edge_probs &v2)
{
  for(edge_probs::const_iterator i = v2.begin(); i != v2.end(); ++i)
    v1[i->first] += i->second;
}

edge_probs_map &
operator *= (edge_probs_map &v, double c)
{
  for(edge_probs_map::iterator i = v.begin(); i != v.end(); ++i)
    i->second *= c;

  return v;
}

edge_probs_map &
operator += (edge_probs_map &v1, const edge_probs_map &v2)
{
  for(edge_probs_map::const_iterator i = v2.begin(); i != v2.end(); ++i)
    v1[i->first] += i->second;

  return v1;
}

edge_count &
operator += (edge_count &v1, const edge_count &v2)
{
  for(edge_count::const_iterator i = v2.begin(); i != v2.end(); ++i)
    v1[i->first] += i->second;
}

edge_count_map &
operator += (edge_count_map &v1, const edge_count_map &v2)
{
  for(edge_count_map::const_iterator i = v2.begin(); i != v2.end(); ++i)
    v1[i->first] += i->second;

  return v1;
}

void
print_edge_probs(ostream &os, const edge_probs &probs, const Graph &g)
{
  edge_probs::const_iterator i = probs.begin();

  if (i != probs.end())
    {
      os << "link " << to_string(i->first, g) << ": " << i->second;

      while(++i != probs.end())
	os << ", link " << to_string(i->first, g) << ": " << i->second;
    }
}

void
print_edge_probs_map(ostream &os, const edge_probs_map &map, const Graph &g)
{
  for(edge_probs_map::const_iterator i = map.begin(); i != map.end(); ++i)
    {
      os << "Destination node " << get(vertex_name, g, i->first) << ": ";
      print_edge_probs(os, i->second, g);
      os << endl;
    }
}

ostream &
operator << (ostream &os, const edge_count &count)
{
  edge_count::const_iterator i = count.begin();

  if (i != count.end())
    {
      os << i->first << ": " << i->second;

      while(++i != count.end())
        os << ", " << i->first << ": " << i->second;
    }

  return os;
}

ostream &
operator << (ostream &os, const edge_count_map &map)
{
  for(edge_count_map::const_iterator i = map.begin(); i != map.end(); ++i)
    os << "Node " << i->first << ": " << i->second << endl;

  return os;
}
