#pragma once

#include "bv.h"
#include "func/vbf.h"
#include "vect/iterproxy.h"

#include <vector>

namespace bf
{
  class DDT
  {
  public:
    DDT()
      :
      _n(BV8(0)),
      _m(BV8(1)),
      _ddt(1, 0)
    {
    }

    explicit DDT(const VBF &f);

    std::vector<VBF> restore() const;

    bool apn() const;
    
    bool invertible() const;

    inline uint64_t count(bv32 delta, bv32 b) const
    {
      return _ddt[(((bv64)delta) << _m) + b];
    }
    
    inline uint64_t& count(bv32 delta, bv32 b)
    {
      return _ddt[(((bv64)delta) << _m) + b];
    }

    inline bv64 sizeDelta() const
    {
      return basis64(_n);
    }

    inline bv64 sizeValues() const
    {
      return basis64(_m);
    }

    inline int n() const
    {
      return _n;
    }

    inline int m() const
    {
      return _m;
    }

    inline IterProxy<bv32> domDelta() const
    {
      return brange(_n).skipFirst();
    }

    inline IterProxy<bv32> domValues() const
    {
      return brange(_m);
    }

    inline bool operator==(const DDT& ddt) const
    {
      return _ddt == ddt._ddt;
    }

    inline bool operator!=(const DDT& ddt) const
    {
      return !(*this == ddt);
    }

  private:
    bool ChangeF(std::vector<bv32>& F, bv32 currentX) const;
    bool CheckDDT(const std::vector<bv32>& F, std::vector<bv32>& tmp) const;
    bool AddFunction(std::vector<VBF>& functions, const std::vector<bv32>& function, std::vector<bv32>& tmp) const;

  private:
    bv8 _n;
    bv8 _m;
    std::vector<uint64_t> _ddt;
  };
}
