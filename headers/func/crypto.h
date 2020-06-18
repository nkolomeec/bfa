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
    
    bv32 gac(const BF& f);
    
    bv8 ci(const BF& f);
    
    bv8 resilency(const BF& f);

    Anf ai(const BF& f);

    bool balanced(const BF& f);

    bool sac(const BF& f);

    bv8 pc(const BF& f);

    int32_t degree(const VBF& f);

    bv32 nonlinearity(const VBF& f);

    bv8 componentAi(const VBF& f);

    uint64_t uniformity(const VBF& f);

    bool uniformity(const VBF& f, uint64_t upToDelta);
  }
}
