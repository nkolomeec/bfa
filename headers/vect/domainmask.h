#pragma once

#include "bfa.h"
#include "bv.h"

namespace bf
{
  template<typename T>
  class DomainMask
  {
  private:
    class DomainMaskElement
    {
    public:
      DomainMaskElement(bv64 mask) :
        _curr(0u),
        _max(mask),
        _mask(mask),
        _end(false)
      {
      }

      inline T operator*() const
      {
        return (T)_curr;
      }

      inline void operator++()
      {
        _end |= (_curr == _max);
        _curr = (_curr + ~_mask + BV64(1)) & _mask;
      }

      inline bool operator!=(const DomainMaskElement&) const
      {
        return !_end;
      }

      inline void decMax()
      {
        _end |= (_curr == _max);
        _max = (_curr - BV64(1)) & _mask;
      }

    private:
      bv64 _curr;
      bv64 _max;
      bv64 _mask;
      bool _end;
    };

  public:
    inline DomainMask(T mask) :
      _elem((bv64)mask)
    {
    }

    inline DomainMaskElement cbegin() const
    {
      return _elem;
    }

    inline DomainMaskElement begin() const
    {
      return cbegin();
    }

    inline DomainMaskElement cend() const
    {
      return cbegin();
    }

    inline DomainMaskElement end() const
    {
      return cend();
    }

    inline DomainMask& skip()
    {
      ++_elem;
      return *this;
    }

    inline DomainMask& skipLast()
    {
      _elem.decMax();
      return *this;
    }

  private:
    DomainMaskElement _elem;
  };
}
