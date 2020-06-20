#pragma once

#include "bv.h"

#include <vector>

namespace bf
{
  class CnkIter 
  {
  private:  
    std::vector<bv32> _values;
    bv32 _n;
    bv32 _k;
    bool _isEnd;

  private:
    CnkIter() = delete;

  public:
    // constructors
    CnkIter(bv32 n, bv32 k)
      :
      _n(n),
      _k(k),
      _values(k),
      _isEnd(true)
    {
      begin();
    }

    bv32 operator[](int64_t i) const
    {
      return _values[i];
    }

    bv32 first32() const
    {
      bv32 result = 0;

      for (bv32 i = BV32(0); i < _k; ++i)
      {
        if (_values[i] >= BV32(32))
        {
          break;
        }

        set1(result, _values[i]);
      }

      return result;
    }

  // "virtual" iterator functions
  public: 
    CnkIter& begin()
    {
      if (_k > _n)
      {
        return *this;
      }

      for (bv32 i = 0; i < _k; ++i) {
        _values[i] = i;
      }

      _isEnd = false;

      return *this;
    }

    explicit operator bool() const
    {
      return !_isEnd;
    }
    
    void operator++()
    {
      if (_k == 0)
      {
        _isEnd = true;
        return;
      }

      int32_t i = _k - 1;
      bv32 tmp;

      while (i >= 0 && _values[i] == _n - _k + i)
      {
        i--;
      }

      if (i >= 0)
      {
        tmp = ++_values[i++];

        while (i < (int32_t)_k)
        {
          _values[i++] = ++tmp;
        }
      }
      else
      {
        _isEnd = true;
      }
    }
  };
}

