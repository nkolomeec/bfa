#pragma once

#include "bfa.h"
#include "bv.h"

namespace bf
{
  template<typename T>
  class Domain
  {
  private:
    class DomainElement
    {
    public:
      DomainElement(bv64 curr) :
        _curr(curr)
      {

      }

      inline T operator*() const
      {
        return (T)_curr;
      }

      inline void operator++()
      {
        ++_curr;
      }

      inline bool operator!=(const DomainElement &elem) const
      {
        return _curr != elem._curr;
      }

    private:
      bv64 _curr;
    };

  public:
    inline Domain(T start, T end) :
      _min((bv64)start),
      _max((bv64)end)
    {
    }

    inline Domain(int length) :
      _min(0u),
      _max(max64(length))
    {
    }


    inline DomainElement cbegin() const
    {
      return DomainElement(std::min(_min, _max + 1));
    }

    inline DomainElement begin() const
    {
      return cbegin();
    }

    inline DomainElement cend() const
    {
      return DomainElement(_max + 1);
    }

    inline DomainElement end() const
    {
      return cend();
    }

    inline T min() const
    {
      return (T)_min;
    }

    inline T max() const
    {
      return (T)_max;
    }

    inline Domain& skip(T n)
    {
      _min += n;
      return *this;
    }

    inline Domain& skip()
    {
      ++_min;
      return *this;
    }

  private:
    bv64 _min;
    bv64 _max;
  };
}
