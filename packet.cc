#include "packet.hpp"

int packet::next_ID = 0;

ostream &operator << (ostream &out, const packet_prefs &prefs)
{
  out << "cls = " << prefs.cls << ": ";

  copy(prefs.begin(), prefs.end(),
       ostream_iterator<Vertex>(out, " "));

  out << "| dest = " << prefs.dest;

  return out;
}


ostream &operator << (ostream &out, const packet &pkt)
{
  out << "<" << pkt.ID << ">: src = " << pkt.src << ", dst = "
      << pkt.dst << ", start_ts = " << pkt.start_ts
      << ", next_ts = " << pkt.next_ts;

  return out;
}


waiting_pkts::const_iterator
find_waiting_pkts(const waiting_pkts &pkts, timeslot ts)
{
  packet test_pkt(0, 0, ts);
  
  waiting_pkts::const_iterator result = pkts.upper_bound(&test_pkt);

  // If we found a sequence of packets, then we want to make sure the
  // first packet in the sequence has the time slot equal to ts.  This
  // ensures that we don't return packets with the time slot number
  // smaller than ts.
  if (result != pkts.begin())
    assert((*pkts.begin())->next_ts == ts);

  return result;
}
