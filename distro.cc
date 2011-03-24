#include "distro.hpp"
#include "nodistro.hpp"
#include "poisson.hpp"
#include "tabdistro.hpp"

#include <cassert>
#include <string>

distro &
distro::operator += (const distro &d)
{
  nodistro *this_nodistro = dynamic_cast<nodistro *>(db);
  poisson *this_poisson = dynamic_cast<poisson *>(db);
  tabdistro *this_tabdistro = dynamic_cast<tabdistro *>(db);
  assert(this_nodistro || this_poisson || this_tabdistro);

  nodistro *d_nodistro = dynamic_cast<nodistro *>(d.db);
  poisson *d_poisson = dynamic_cast<poisson *>(d.db);
  tabdistro *d_tabdistro = dynamic_cast<tabdistro *>(d.db);
  assert(d_nodistro || d_poisson || d_tabdistro);

  if (this_poisson && d_poisson)
    this->operator=(poisson(db->mean() + d.db->mean()));
  else if (this_tabdistro && d_tabdistro)
    (*this_tabdistro) += (*d_tabdistro);
  else if (this_nodistro && !d_nodistro)
    this->operator=(d);
  else if (d_nodistro)
    {} // do nothing
  else
    assert(this_nodistro && d_nodistro);

  return *this;
}

distro &
distro::operator *= (double p)
{
  nodistro *this_nodistro = dynamic_cast<nodistro *>(db);
  poisson *this_poisson = dynamic_cast<poisson *>(db);
  tabdistro *this_tabdistro = dynamic_cast<tabdistro *>(db);
  assert(this_nodistro || this_poisson);
  assert(!this_tabdistro);

  if (this_poisson)
    operator = (distro(poisson(p * mean())));
}

bool
distro::operator == (const distro &d) const
{
  nodistro *this_nodistro = dynamic_cast<nodistro *>(db);
  poisson *this_poisson = dynamic_cast<poisson *>(db);
  tabdistro *this_tabdistro = dynamic_cast<tabdistro *>(db);
  assert(this_nodistro || this_poisson || this_tabdistro);

  nodistro *d_nodistro = dynamic_cast<nodistro *>(d.db);
  poisson *d_poisson = dynamic_cast<poisson *>(d.db);
  tabdistro *d_tabdistro = dynamic_cast<tabdistro *>(d.db);
  assert(d_nodistro || d_poisson || d_tabdistro);

  if (this_nodistro && d_nodistro)
    return *this_nodistro == *d_nodistro;

  if (this_poisson && d_poisson)
    return *this_poisson == *d_poisson;

  if (this_tabdistro && d_tabdistro)
    return *this_tabdistro == *d_tabdistro;

  return false;
}

distro
operator * (double p, const distro &d)
{
  distro result = d;
  result *= p;
  return result;
}

ostream &
operator << (ostream &out, const distro &d)
{
  d.db->output(out);
  return out;
}
