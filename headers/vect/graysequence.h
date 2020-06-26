#pragma once

#include "bfa.h"
#include "bv.h"

namespace bf
{
  class GraySequence
  {
  private:
    class GraySequenceElement
    {
    public:
      GraySequenceElement(T initialState, bv64 curr) :
        _curr(curr)
      {

      }

      inline int operator*() const
      {
        return bitToInt((_curr + 1) & ~_curr);
      }

      inline void operator++()
      {
        ++_curr;
      }

      inline bool operator!=(const GraySequenceElement& elem) const
      {
        return _curr != elem._curr;
      }

    private:
      bv64 _curr;
    };

  public:
    inline GraySequence(int n) :
      _n(n)
    {
      assert(n > 0 && n <= 64);
    }


    inline GraySequenceElement cbegin() const
    {
      return GraySequenceElement(0);
    }

    inline GraySequenceElement begin() const
    {
      return cbegin();
    }

    inline GraySequenceElement cend() const
    {
      return GraySequenceElement(max64(_n));
    }

    inline GraySequenceElement end() const
    {
      return cend();
    }

  private:
    int _n;
  };
}
