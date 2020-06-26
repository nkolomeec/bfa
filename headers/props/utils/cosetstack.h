#pragma once

#include "bv.h"
#include "func/bf.h"

#include <vector>

namespace bf
{
  namespace subspaceutils
  {
    class CosetStack
    {
    public:
      CosetStack(bv64 nzeros, bv64 nones) :
        _top(0),
        _stack(size(nzeros, nones))
      {
      }

    public:
      inline const uint64_t top() const
      {
        return _top;
      }

      inline const bool empty() const
      {
        return _top == BV64(0);
      }

      inline const bv16* peek(uint64_t addr) const
      {
        return &_stack[addr];
      }

      inline const bv16* peekTop() const
      {
        return peekTop(0);
      }

      inline const bv16* peekTop(uint64_t addr) const
      {
        return &_stack[_top - addr - 1];
      }

      inline const bv16* pop(bv64 count)
      {
        _top -= count;
        return &_stack[_top];
      }

      inline void push(bv16 value)
      {
        _stack[_top++] = value;
      }

      inline void push(const bv16* values, bv64 count)
      {
        for (auto i = BV64(0); i < count; ++i)
        {
          push(values[i]);
        }
      }

      inline void reserve(bv64 count)
      {
        _top += count;
      }

      inline void set(uint64_t addr, bv16 value)
      {
        _stack[addr] = value;
      }

      inline static bv64 size(bv64 nzeros, bv64 nones)
      {
        // bound for coset representative size: 2 * (nzeros * nzeros + nones * nones)) / 3
        // bound for coset group info (p element per group) size: 2 * p * (nzeros + nones),
        //  here p = 4: dimension, new basis vector, size of zeros, size of ones
        // + (nzeros + nones) for affine case: it is uses 2ab <= a*a + b*b, but it is possible that 2ab > a(a-1) + b(b-1)
        return (2 * (nzeros * nzeros + nones * nones)) / 3 + (4 * 2 + 1) * (nzeros + nones);
      }
    private:
      uint64_t _top;
      std::vector<bv16> _stack;
    };
  }
}