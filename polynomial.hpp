#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP

#include "counter.hpp"
#include "distro.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <map>
#include <ostream>

#include <boost/serialization/map.hpp>

using namespace std;

/**
 * This is the polynomial class.  They can be regular polynomials, but
 * also modulo polynomials.
 *
 * A modulo polynomial belongs to the ring of polynomials modulo $x^S$
 * over the field of real numbers.  But what it really means is that
 * no matter what polynomials you multiply, the resulting polynomial
 * is going to be of at most (S - 1) degree, i.e. there will be no
 * terms that have the power equal or higher then S.  We achieve this
 * easily in the multiplication operators defined for this type: we
 * just discard the terms with too high a power.
 */
template<typename T>
class tpoly : public map<size_t, T>, private counter<tpoly<T> >
{
  friend class boost::serialization::access;

  typedef map<size_t, T> base_type;

  /**
   * This is the number which limits the number of terms in the
   * polynomial.  We are making it a static variable, because we
   * operate only on one value of S in one program run. The polynomial
   * will not have any term whose exponent is larger or equal to S.
   */
  static size_t itsS;

  /**
   * We use this variable if someone wants a coefficient of the power
   * equal to or greater of itsS.
   */
  static T itsTrash;

  /**
   * Serialization function.
   */
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & boost::serialization::base_object<base_type>(*this);
  }

public:
  tpoly() {}

  /**
   * With this constructor you can get a polynomial with the c0 (the
   * term of the lowest order) initialized.
   */
  tpoly(const T &c_0)
  {
    operator[](0) = c_0;
  }

  /**
   * We need to overload this operator, because we have to care about
   * the degree of the polynomial.
   */
  T& operator[](size_t i)
  {
    if (i < itsS)
      return base_type::operator[](i);
    else
      return itsTrash;
  }

  /**
   * Sets a coefficient at the i-th power to c_i.
   *
   * @param c_i the coefficient value
   *
   * @param i the index value
   *
   * @return the polynomial
   */
  tpoly &operator()(const T c_i, size_t i)
  {
    operator[](i) = c_i;

    return *this;
  }

  /**
   * Returns the value of S.
   */
  static int get_S()
  {
    return itsS;
  }

  /**
   * Sets the value of S.
   */
  static void set_S(size_t S)
  {
    itsS = S;
  }

  using counter<tpoly<T> >::how_many;
};

/**
 * The operator: tpoly<A> * tpoly<B>.
 */
template<typename A, typename B>
tpoly<B>
operator * (const tpoly<A> &p1, const tpoly<B> &p2)
{
  tpoly<B> result;

  for(typename tpoly<A>::const_iterator
        i = p1.begin(); i != p1.end(); ++i)
    for(typename tpoly<B>::const_iterator
          j = p2.begin(); j != p2.end(); ++j)
      {
        int index = i->first + j->first;

        if (index < tpoly<B>::get_S())
          result[index] += i->second * j->second;
      }

  return result;
}

/**
 * The *= operator for tpoly<T>.
 */
template<typename A, typename B>
tpoly<A> &
operator *= (tpoly<A> &p1, const B &c)
{
  for(typename tpoly<A>::iterator i = p1.begin(); i != p1.end(); ++i)
    i->second *= c;

  return p1;
}

/**
 * The addition operator for polynomials with coefficients of the same
 * type.
 */
template<typename T>
tpoly<T> &
operator += (tpoly<T> &p1, const tpoly<T> &p2)
{
  for(typename tpoly<T>::const_iterator
        i = p2.begin(); i != p2.end(); ++i)
    p1[i->first] += i->second;
  return p1;
}

/**
 * The addition operator for polynomials with coefficients of the same
 * type.
 */
template<typename T>
tpoly<T>
operator + (const tpoly<T> &p1, const tpoly<T> &p2)
{
  tpoly<T> result = p1;
  result += p2;
  return result;
}

/**
 * The output operator for a polynomial.
 */
template<typename T>
ostream &operator << (ostream &os, const tpoly<T> &p)
{
  if (!p.size())
    os << "0";

  for(typename tpoly<T>::const_reverse_iterator
        i = p.rbegin(); i != p.rend();)
    {
      os << i->second;

      if (i->first)
        os << "*x^" << i->first;;

      if (++i != p.rend())
        os << " + ";
    }

  return os;
}

template<typename T>
size_t tpoly<T>::itsS = numeric_limits<size_t>::max();

template<typename T>
T tpoly<T>::itsTrash;

/**
 * This is a polynomial with floating point variables as coefficients.
 * We use this type when we want to give probabilities for a packet.
 * The polynomial is used to encode also the distance.
 *
 * This polynomial is used in trans_matrix.
 */
typedef tpoly<double> fp_poly;

/**
 * This polynomial remembers the distributions for packets that travel
 * specific distances.
 *
 * Note this is the polynomial with the "distro *" coefficients.  The
 * polynomial owns the distro objects, and will destroy them.
 * Therefore once you put the pointer to the distro object, you
 * should not destroy it, because the polynomial will do it for you.
 */
typedef tpoly<distro> dist_poly;

/**
 * This keeps the number of time a specific number of packets
 * appeared.  We need a polynomial to remember the distance packets
 * travel.  The map tells the number of times the given number of
 * packets appeared.  Therefore count_map_poly[dist][count] is the
 * number of time slots during which there were exactly "count" number
 * of packets that travelled distance "dist".
 */
typedef tpoly<map<int, int> > count_map_poly;

/**
 * The polynomial with integers as coefficients.  It is use to count
 * the number of packets.
 */
typedef tpoly<int> count_poly;

#endif /* POLYNOMIAL_HPP */
