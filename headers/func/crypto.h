#pragma once

#include "func/bf.h"
#include "func/anf.h"
#include "func/vbf.h"
#include "func/vanf.h"
#include "props/walshspectrum.h"

namespace bf
{
  namespace crypto
  {
    int32_t degree(const BF& f);
    
    bv32 nonlinearity(const BF& f);
    
    bv64 gac(const BF& f);
    
    int ci(const BF& f);
    
    int resilency(const BF& f);

    Anf ai(const BF& f);

    bool balanced(const BF& f);

    bool sac(const BF& f);

    int pc(const BF& f);

    int degree(const VBF& f);

    uint32_t nonlinearity(const VBF& f);

    int componentAi(const VBF& f);

    uint64_t uniformity(const VBF& f);

    bool uniformity(const VBF& f, uint64_t upToDelta);
  }
}
