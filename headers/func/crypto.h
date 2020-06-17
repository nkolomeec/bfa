#pragma once

#include "func/bf.h"
#include "func/anf.h"
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
  }
}
