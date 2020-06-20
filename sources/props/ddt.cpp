#include "props/ddt.h"

namespace bf
{
  DDT::DDT(const VBF& f)
    :
    _n((bv8)f.n()),
    _m((bv8)f.m()),
    _ddt(basis64(f.n() + f.m()))
  {
    _ddt[0] = sizeValues();
    
    for (auto delta : domDelta())
    {
      for (auto x : f.dom())
      {
        ++count(delta, f.get(x) ^ f.get(x ^ delta));
      }
    }
  }

  bool DDT::apn() const
  {
    for (auto delta : domDelta())
    {
      for (auto b : domValues())
      {
        if (count(delta, b) > 2)
        {
          return false;
        }
      }
    }

    return true;
  }

  bool DDT::invertible() const
  {
    if (_n != _m)
    {
      return false;
    }

    for (auto delta : domDelta())
    {
      if (count(delta, 0) > 0)
      {
        return false;
      }
    }

    return true;
  }

  bool DDT::ChangeF(std::vector<bv32>& F, bv32 currentX) const
  {
    auto nElems = sizeValues();

    for (bv32 d = F[currentX] < nElems ? F[currentX] + 1 : 0; d < nElems; ++d)
    {
      bv32 x = BV32(0);
      for (; x < currentX; ++x)
      {
        if (zero(count(currentX ^ x, d ^ F[x])))
        {
          break;
        }
      }

      if (x < currentX)
      {
        continue;
      }

      F[x] = d;

      return true;
    }

    return false;
  }

  bool DDT::CheckDDT(const std::vector<bv32>& F, std::vector<bv32>& tmp) const
  {
    auto nElems = sizeValues();

    for (auto i = BV32(0); i < F.size(); ++i)
    {
      for (auto y = BV32(0); y < nElems; ++y)
      {
        tmp[y] = 0;
      }

      for (auto x = BV32(0); x < F.size(); ++x)
      {
        tmp[F[x] ^ F[x ^ i]]++;
      }

      for (auto y = BV32(0); y < nElems; ++y)
      {
        if (tmp[y] != count(i, y))
        {
          return false;
        }
      }
    }

    return true;
  }

  bool DDT::AddFunction(std::vector<VBF>& functions, const std::vector<bv32>& function, std::vector<bv32>& tmp) const
  {
    if (!CheckDDT(function, tmp))
    {
      return false;
    }
    
    functions.push_back(VBF(_n, _m));

    auto& func = functions[functions.size() - 1];

    for (auto x : func.dom())
    {
      func.set(x, function[x]);
    }

    return true;
  }


  std::vector<VBF> DDT::restore() const
  {
    std::vector<VBF> result;

    auto nX = (bv32)sizeDelta();
    auto nElems = (bv32)sizeValues();

    std::vector<bv32> F(nX, nElems);
    std::vector<bv32> tmp(nElems + BV64(1));

    F[0] = 0;

    if (nX == 1u)
    {
      AddFunction(result, F, tmp);

      return result;
    }

    for (auto b = BV32(0); b < nElems; ++b)
    {
      if (!zero(count(BV32(1), b)))
      {
        F[1] = b;
        
        break;
      }
    }

    if (F[1] == nElems)
    {
      return result;
    }

    if (nX == BV32(2))
    {
      AddFunction(result, F, tmp);
      
      return result;
    }

    auto isAPN = apn();
    auto currentX = BV32(2);

    while (true)
    {
      if (currentX < BV32(2))
      {
        break;
      }

      bool xChange = ChangeF(F, currentX);

      if (currentX == 2)
      {
        // std::cout << "\t\tprogress: " << F[currentX] << " of " << nElems << std::endl;
      }

      if (xChange)
      {
        if (isAPN && currentX == BV32(3))
        {
          auto d2 = F[3] ^ F[1];
          auto d3 = F[2] ^ F[1];

          if (F[2] > d2 || (F[2] == d2 && F[3] > d3))
          {
            continue;
          }
        }

        if (currentX + 1 == nX)
        {
          AddFunction(result, F, tmp);
        }
        else
        {
          currentX++;
        }
      }
      else
      {
        F[currentX] = nElems;
        currentX--;
      }
    }

    return result;
  }
}
