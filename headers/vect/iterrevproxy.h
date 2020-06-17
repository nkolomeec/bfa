#pragma once

#include "bfa.h"

namespace bf
{
  template<typename  T>
  class IterRevProxy
  {
  private:
    class IterRevProxyEndState
    {
    };

  public:
    inline IterRevProxy(T start, T end, bool isEnd):
      _current(end),
      _minimum(start),
      _end(isEnd)
    {
    }

    inline IterRevProxy(T nbVectors, bool isEnd) :
      _current(nbVectors - static_cast<T>(1)),
      _minimum(static_cast<T>(0)),
      _end(isEnd)
    {
    }

    inline T& operator*()
    {
      return _current;
    }

    inline void operator++()
    {
      if (_current == _minimum)
      {
        _end = true;
      }
      
      _current--;
    }

    inline IterRevProxy& begin()
    {
      return *this;
    }

    inline IterRevProxyEndState end()
    {
      return IterRevProxyEndState();
    }

    inline bool operator!=(const IterRevProxyEndState)
    {
      return !this->_end;
    }

  private:
    T _current;
    T _minimum;
    bool _end;
  };
}
