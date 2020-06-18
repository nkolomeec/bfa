#pragma once

#include "bfa.h"

namespace bf
{
  template<typename  T>
  class IterProxy
  {
  private:
    class IterProxyEndState
    {
    };

  public:
    inline IterProxy(T start, T end, bool isEnd):
      _current(start),
      _maximum(end),
      _end(isEnd)
    {
    }

    inline IterProxy(T nbVectors, bool isEnd) :
      _current(static_cast<T>(0)),
      _maximum(nbVectors - static_cast<T>(1)),
      _end(isEnd)
    {
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
      
      _current++;
    }

    inline IterProxy& begin()
    {
      return *this;
    }

    inline IterProxyEndState end()
    {
      return IterProxyEndState();
    }

    inline bool operator!=(const IterProxyEndState)
    {
      return !this->_end;
    }

    inline T max()
    {
      return _maximum;
    }

    inline IterProxy& skipFirst()
    {
      ++(*this);
      return *this;
    }

  private:
    T _current;
    T _maximum;
    bool _end;
  };
}
