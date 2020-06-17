#pragma once

#include "vect/bvdata.h"
#include "vect/bv8.h"
#include "vect/bv16.h"
#include "vect/bv32.h"
#include "vect/bv64.h"

namespace bf
{
  inline int power(bool value)
  {
    return 1 - (static_cast<unsigned int>(value) << 1);
  }
}