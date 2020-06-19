#pragma once

#include "bv.h"

#include <vector>

namespace bf
{
  template<typename T>
  int rank(const T *elements, bv64 count)
  {
    std::vector<T> matrix(count);

    for (bv64 i = 0; i < count; ++i)
    {
      matrix[i] = elements[i];
    }
    
    for (bv64 i = 0; i < count; ++i)
    {
      bv64 maxIdx = i;
      
      for (bv64 j = i + 1; j < count; ++j)
      {
        if (matrix[j] > matrix[maxIdx])
        {
          maxIdx = j;
        }
      }

      if (matrix[maxIdx] == 0)
      {
        return (bv8)i;
      }

      std::swap(matrix[i], matrix[maxIdx]);

      for (bv64 j = i + 1; j < count; ++j)
      {
        if (sameLeads(matrix[j], matrix[i]))
        {
          matrix[j] ^= matrix[i];
        }
      }
    }

    return (int)count;
  }
}