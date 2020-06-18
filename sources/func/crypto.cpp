#include "func/bf.h"
#include "func/anf.h"
#include "func/crypto.h"
#include "props/walshspectrum.h"

namespace
{
  uint64_t calculateUniformity(const bf::VBF& f, uint64_t breakOnDelta)
  {
    std::vector<uint64_t> b(bf::basis64(f.m()));

    uint64_t max = 0U;

    for (auto a : f.dom().skipFirst())
    {
      for (auto& x : b)
      {
        x = 0;
      }

      for (auto x : f.dom())
      {
        auto val = f.get(x) ^ f.get(x ^ a);
        ++b[val];

        if (b[val] > max)
        {
          max = b[val];

          if (max > breakOnDelta)
          {
            return max;
          }
        }
      }
    }

    return max;
  }
}

namespace bf 
{
  namespace crypto
  {
    int32_t degree(const BF& f)
    {
      return Anf(f).deg();
    }

    bv32 nonlinearity(const BF& f)
    {
      return WalshSpectrum(f).nonlinearity();
    }

    bv32 gac(const BF& f)
    {
      if (bf::zero(f.n()))
      {
        return BV32(1);
      }

      auto autocorrelation = WalshSpectrum(f).autocorrelation();
      auto gacValue = BV64(0);

      for (auto i = BV64(1); i < autocorrelation.size(); ++i)
      {
        auto a = std::abs(autocorrelation[i]);

        if ((bv64)a > gacValue)
        {
          gacValue = (bv64)a;
        }
      }

      return (bv32)gacValue;
    }

    bv8 ci(const BF& f)
    {
      return WalshSpectrum(f).ci();
    }

    bv8 resilency(const BF& f)
    {
      return WalshSpectrum(f).resilency();
    }

    Anf ai(const BF& f)
    {
      Anf anf(f);

      auto bound1 = anf.deg();

      if (bound1 <= 0)
      {
        return Anf(true);
      }

      auto bound2 = (f.n() >> 1) + (f.n() & BV8(1));
      
      auto bound = std::min((bv8)bound1, (bv8)bound2);

      auto annihilator1 = anf.minAnnihilator(bound);
      auto deg1 = annihilator1.deg();
      
      if (deg1 > 0)
      {
        bound = (bv8)(deg1 - 1);
      }

      anf.invert(BV32(0));

      auto annihilator2 = anf.minAnnihilator(bound);
      auto deg2 = annihilator2.deg();

      if (deg2 > 0)
      {
        return annihilator2;
      }

      return annihilator1;
    }

    bool balanced(const BF& f)
    {
      return f.balanced();
    }

    bool sac(const BF& f)
    {
      if (bf::zero(f.n()))
      {
        return false;
      }

      for (auto i = BV8(0); i < f.n(); ++i)
      {
        if (f.autocorrelation(bf::basis32(i)) != 0)
        {
          return false;
        }
      }

      return true;
    }

    bv8 pc(const BF& f)
    {
      auto autocorrelation = WalshSpectrum(f).autocorrelation();

      bv8 pcValue = f.n();

      for (bv64 i = 1; i < autocorrelation.size(); ++i)
      {
        if (autocorrelation[i] != 0)
        {
          auto w = bf::weight(i);

          if (w <= pcValue)
          {
            pcValue = w - 1;

            if (zero(pcValue))
            {
              return pcValue;
            }
          }
        }
      }

      return pcValue;
    }

    int32_t degree(const VBF& f)
    {
      return VAnf(f).deg();
    }

    bv32 nonlinearity(const VBF& f)
    {
      auto nonlin = bf::basis32(f.n() - 1);

      for (auto comp : f.components())
      {
        auto compNonlin = nonlinearity(f.component(comp));

        if (compNonlin < nonlin)
        {
          nonlin = compNonlin;
        }
      }

      return nonlin;
    }

    bv8 componentAi(const VBF& f)
    {
      VAnf vanf(f);

      auto currAI = std::min((bv8)vanf.deg(), (bv8)((f.n() >> 1) + (f.n() & BV8(1))));

      for (auto comp : f.components())
      {
        auto compAnf = vanf.component(comp);

        for (auto j = 0; j < 2; ++j)
        {
          auto currBound = std::min((bv8)(currAI - 1), (bv8)compAnf.deg());

          if (j != 0)
          {
            compAnf.invert(BV32(0));
          }

          auto annihilator = compAnf.minAnnihilator(currBound);

          auto currDeg = annihilator.deg();

          if (currDeg < 0)
          {
            continue;
          }

          if (currDeg < currAI)
          {
            currAI = currDeg;
   
            if (zero(currAI))
            {
              return currAI;
            }
          }
        }
      }

      return currAI;
    }

    uint64_t uniformity(const VBF& f)
    {
      return calculateUniformity(f, f.size() - 1);
    }

    bool uniformity(const VBF& f, uint64_t upToDelta)
    {
      auto uniformity =  calculateUniformity(f, upToDelta);

      return uniformity <= upToDelta;
    }
  }
}
