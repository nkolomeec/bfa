#include "gf2/gf2table.h"

namespace bf
{
    bv32 GF2Table::mult(bv32 a, bv32 b) const noexcept
    {
      assert(a < _nbelements && b < _nbelements);
      
      if (zero(a) || zero(b)) 
      {
        return BV32(0);
      }
      
      return (_elements[(_degrees[a] + _degrees[b]) % _nbdegs]);
    }

    bv32 GF2Table::pow(bv32 a, bv64 n) const noexcept
    {
      assert(a < _nbelements && (!zero(a) || !zero(n)));

      if (zero(a))
      {
        return BV32(0);
      }

      return _elements[(_degrees[a] * ((bv64)n)) % _nbdegs];
    }

    bool GF2Table::tr(bv32 a) const noexcept
    {
      assert(a < _nbelements);

      return product(_trcoeff, a);
    }

    bool GF2Table::isField()
    {
      return true;
    }

    GF2Table::GF2Table(bv64 polynomial)
      :
      GF2(polynomial)
    {
      _n = lead(Polynomial);
      
      _nbelements = basis64(_n);
      _nbdegs = (bv32)(_nbelements - 1);

      _primary = BV32(0);

      _degrees.resize(_nbelements, 0);
      _elements.resize(_nbelements, 0);

      _degrees[0] = (bv32)(_nbelements - 1);
      _degrees[1] = BV32(0);

      _elements[0] = BV32(1);
      _elements[_nbelements - 1] = BV32(0);

      std::vector<bool> checked(_nbelements);
      checked[0] = true;
      checked[1] = true;

      for (auto i = BV32(2); i < _nbelements; ++i) 
      {
        if (checked[i])
        {
          continue;
        }

        auto primaryPow = BV32(1);
   
        for (auto j = BV32(1); j < _nbdegs; ++j) 
        {
          primaryPow = GF2::mult(primaryPow, i);

          if (primaryPow <= BV32(1))
          {
            break;
          }

          checked[primaryPow] = true;

          _degrees[primaryPow] = j;
          _elements[j] = primaryPow;
        }

        if (_elements[_nbdegs - 1] != 0) 
        {
          _primary = i;
          break;
        }
      }
      
      if (Polynomial == BV64(3))
      {
        _primary = BV32(1);
      }

      if (_primary == BV32(0)) 
      {
        throw 1;
      }

      _trcoeff = 0;
      for (bv8 i = 0; i < _n; i++) 
      {
        if (GF2::tr(basis32(i))) 
        {
          set1(_trcoeff, i);
        }
      }
    }

    bv32 GF2Table::generator() const
    {
      return _primary;
    }
}
