#include "func/vbf.h"
#include "func/vanf.h"

namespace bf 
{
  VBF::VBF(const VAnf &bf)
    :
    VBFBase(bf.MobiusTransform())
  {
  }
}
