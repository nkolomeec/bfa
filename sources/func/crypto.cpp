#include "func/bf.h"
#include "func/anf.h"
#include "func/crypto.h"
#include "props/walshspectrum.h"


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
  }
}
