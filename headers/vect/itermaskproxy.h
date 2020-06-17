#pragma once

#include "bfa.h"

namespace bf
{
  template<typename  T>
  class IterMaskProxy
  {
  private:
    class IterMaskProxyEndState
    {
    };

  public:
    inline IterMaskProxy(T mask, bool skipZero):
      _current(static_cast<T>(0)),
      _mask(mask),
      _maximum(mask),
      _end(false)
    {
      if (skipZero)
      {
        ++(*this);
      }
    }

    inline IterMaskProxy(T start, T end, T mask) :
      _current(start & mask),
      _mask(mask),
      _maximum(end & mask)
    {
      _end = _current > _mask;
    }

    inline T& operator*()
    {
      return _current;
    }

    inline void operator++()
    {
      if (_current == _maximum)
      {
        _end = true;
      }
      
      _current = (_current + ~_mask + static_cast<T>(1)) & _mask;
    }

    inline IterMaskProxy& begin()
    {
      return *this;
    }

    inline IterMaskProxyEndState end()
    {
      return IterMaskProxyEndState();
    }

    inline bool operator!=(const IterMaskProxyEndState)
    {
      return !this->_end;
    }

  private:
    T _current;
    T _maximum;
    T _mask;
    bool _end;
  };
}
