#pragma once

#include "bfa.h"
#include "bv.h"

namespace bf
{
  template<typename T>
  class LinearCombinations
  {
  public:
    inline LinearCombinations(const T& coset, std::vector<T> basis, bool skipZero = false) :
      LinearCombinations(coset, basis.data(), basis.size(), skipZero)
    {

    }

    inline LinearCombinations(const T& coset, const T *basis, int n, bool skipZero = false):
      _max(max64(n)),
      _state(coset),
      _basis(n > 0 ? basis : &_state),
      _curr(0),
      _end(false)
    {
      assert(0 <= n && n <= 64);
      
      if (skipZero)
      {
        ++* this;
      }
    }

    inline LinearCombinations(T&& coset, const T* basis, int n, bool skipZero = false) :
      _max(max64(n)),
      _state(std::move(coset)),
      _basis(n > 0 ? basis : &_state),
      _curr(0),
      _end(false)
    {
      assert(0 <= n && n <= 64);

      if (skipZero)
      {
        ++*this;
      }
    }

    //LinearCombinations(const LinearCombinations&) = delete;
    //LinearCombinations(LinearCombinations&&) = delete;
    //LinearCombinations& operator=(const LinearCombinations&) = delete;
    //LinearCombinations& operator=(LinearCombinations&&) = delete;

    inline const T& combination() const
    {
      return _state;
    }

    inline bv64 coordinates() const
    {
      return _curr ^ (_curr >> 1);
    }

    inline operator bool() const
    {
      return !_end;
    }

    inline LinearCombinations& operator*()
    {
      return *this;
    }

    inline void operator++()
    {
      _end = (_curr == _max);
      _curr = (_curr + 1) & _max;
      _state ^= _basis[trailingZeros(leastOne(_curr))];
    }

    inline bool operator!=(const LinearCombinations&) const
    {
      return (bool)*this;
    }

    inline LinearCombinations& begin()
    {
      return *this;
    }

    inline LinearCombinations& end()
    {
      return *this;
    }

  private:
    T _state;
    const T* _basis;
    bv64 _curr;
    bool _end;
    bv64 _max;
  };
}
