#ifndef COUNTER_HPP
#define COUNTER_HPP

#include <cstdlib>

/**
 * This class keeps track of the number of objects of your class.
 *
 * To use it, derive your class privatelly from it.  And then make the
 * how_many function publically available in your class.  The how_many
 * class will tell the number of all objects of your class, be it
 * static, global, automatic or dynamic.
 *
 * Example:
 *
 * class my_class: private counter<my_class> {
 *
 * ...
 *
 * public:
 *   using counter<my_class>::how_many;
 *
 * };
 */
template<typename T>
class counter {
public:
  counter()
  {
    ++count;
  }

  counter(const counter&)
  {
    ++count;
  }

  ~counter()
  {
    --count;
  }

  /// Returns the number of objects alive.
  static size_t how_many()
  {
    return count;
  }
  
private:
  static size_t count;
};


template<typename T>
size_t
counter<T>::count = 0;

#endif /* COUNTER_HPP */
