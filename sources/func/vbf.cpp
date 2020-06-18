#include "func/vbf.h"
#include "func/vanf.h"
#include "func/fpolynomial.h"

namespace bf 
{
  VBF::VBF(const VAnf &bf)
    :
    VBFBase(bf.MobiusTransform())
  {
  }

  VBF::VBF(const FPolynomial& poly)
    :
    VBFBase(poly.n(), poly.m())
  {
    for (auto x : dom())
    {
      set(x, poly.value(x));
    }
  }
}
