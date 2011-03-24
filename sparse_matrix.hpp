#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include <cassert>
#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/base_object.hpp>

using namespace std;

/**
 * Implements a sparse matrix.  Therefore the implementation isn't
 * based on a vector, but on a map.
 *
 * The important thing to know is that when you addess the matrix with
 * exactly one index, then you get a map, and that map gives elements
 * in a row.
 */
template <typename K, typename T>
class sparse_matrix : public map<K, map<K, T> >
{
  friend class boost::serialization::access;
  typedef map<K, map<K, T> > base_type;

  /**
   * Serialization function.
   */
  template<class Archive>
  void serialize(Archive &ar, const unsigned int)
  {
    ar & boost::serialization::base_object<base_type>(*this);
  }

public:
  /**
   * This function returns the value of an element.  Make sure you can
   * call this function with function "exists," otherwise this
   * function could fail with an assertion.
   */
  T at(K i, K j) const
  {
    typename base_type::const_iterator ii = this->find(i);
    assert(ii != this->end());

    typename map<K, T>::const_iterator ji = ii->second.find(j);
    assert(ji != ii->second.end());

    return ji->second;
  }

  /**
   * Returns true if the element exists or false otherwise.
   */
  bool exists(K i, K j) const
  {
    typename base_type::const_iterator ii = this->find(i);

    if (ii != this->end())
      {
        typename map<K, T>::const_iterator ji = ii->second.find(j);

        if (ji != ii->second.end())
          return true;
      }

    return false;
  }
};

/**
 * Adds matrixes.
 */
template <typename K, typename T>
sparse_matrix<K, T> operator + (const sparse_matrix<K, T> &a,
                                const sparse_matrix<K, T> &b)
{
  sparse_matrix<K, T> result;

  for(typename sparse_matrix<K, T>::const_iterator
        i = a.begin(); i != a.end(); ++i)
    for(typename sparse_matrix<K, T>::mapped_type::const_iterator
          j = i->second.begin(); j != i->second.end(); ++j)
      result[i->first][j->first] += j->second;

  for(typename sparse_matrix<K, T>::const_iterator
        i = b.begin(); i != b.end(); ++i)
    for(typename sparse_matrix<K, T>::mapped_type::const_iterator
          j = i->second.begin(); j != i->second.end(); ++j)
      result[i->first][j->first] += j->second;

  return result;
}

/**
 * Multiplies the matrix by a constant.
 */
template <typename K, typename T>
sparse_matrix<K, T> operator * (double c, const sparse_matrix<K, T> &mat)
{
  sparse_matrix<K, T> result;

  for(typename sparse_matrix<K, T>::const_iterator
        i = mat.begin(); i != mat.end(); ++i)
    for(typename sparse_matrix<K, T>::mapped_type::const_iterator
          j = i->second.begin(); j != i->second.end(); ++j)
      result[i->first][j->first] = c * j->second;

  return result;
}

/**
 * Prints the matrix.
 */
template <typename K, typename T>
ostream& operator << (ostream &os, const sparse_matrix<K, T> &mat)
{
  for(typename sparse_matrix<K, T>::const_iterator
        i = mat.begin(); i != mat.end(); ++i)
    for(typename sparse_matrix<K, T>::mapped_type::const_iterator
          j = i->second.begin(); j != i->second.end(); ++j)
      os << "(" << i->first << ", " << j->first << ") = "
         << j->second << "\n";

  return os;
}

#endif /* SPARSE_MATRIX_HPP */
