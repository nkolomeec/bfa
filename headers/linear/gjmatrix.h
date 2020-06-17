#pragma once

#include "bv.h"
#include "vect/chunkvector.h"

#include <vector>

namespace bf
{
  class GJMatrix
  {
  public:
    GJMatrix()
      :
      _n(0)
    {
    }

    GJMatrix(const ChunkVector *rows, bv64 nrows, bv64 ncolumns)
    {
      init(rows, nrows, ncolumns, ncolumns);
    }

    void init(const ChunkVector* rows, bv64 nrows, bv64 ncolumns, bv64 upToDim);

    const ChunkVector& operator[](bv64 i)
    {
      return _rows[i];
    }

    bv64 nrows() const
    {
      return _rows.size();
    }

    bv64 ncolumns() const
    {
      return _n;
    }

    bool nondegenerate() const
    {
      return nrows() == ncolumns();
    }

    ChunkVector firstSolution() const;

  private:
    std::vector<bv64> gauss(std::vector<ChunkVector>& matrix, bv64 ncolumns, bv64 upToDim);
    
    std::vector<ChunkVector> prepareMatrix(const ChunkVector* rows, bv64 nrows, bv64 ncolumns);

  private:
    bv64 _n;
    std::vector<ChunkVector> _rows;
    std::vector<bv64> _leads;
  };
}