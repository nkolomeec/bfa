#include "vect/bvio.h"
#include "vect/enum.h"
#include "linear/gjmatrix.h"

namespace
{
  bool lexicographicalLess(const bf::ChunkVector& v1, const bf::ChunkVector& v2, bf::bv64 ncolumns)
  {
    for (auto i = (bf::bv64)(v1.size() - 1); !bf::zero(i); --i)
    {
      if (!bf::zero(v1[i]) || !bf::zero(v2[i]))
      {
        return v1[i] < v2[i];
      }
    }

    return v1[0] < v2[0];
  }

  int64_t findLead(const bf::ChunkVector& v)
  {   
    for (auto i = (bf::bv64)(v.size() - 1); !bf::zero(i); --i)
    {
      if (!bf::zero(v[i]))
      {
        return (i << 6) + bf::lead(v[i]);
      }
    }

    return bf::zero(v[0]) ? -1 : bf::lead(v[0]);
  }
}

namespace bf 
{
  std::vector<bv64> GJMatrix::gauss(std::vector<ChunkVector> &matrix, bv64 ncolumns, bv64 upToDim)
  {
    std::vector<bv64> leads;
    bv64 nrows = matrix.size();
    bv64 i;
    bv64 dim = 0;
    bv64 previousLead = ncolumns;

    for (i = BV64(0); i < nrows; ++i)
    {
      bv64 maxIdx = i;

      for (bv64 j = i + 1; j < nrows; ++j)
      {
        if (lexicographicalLess(matrix[maxIdx], matrix[j], ncolumns))
        {
          maxIdx = j;
        }
      }

      auto currentLead = findLead(matrix[maxIdx]);

      if (currentLead < 0)
      {
        break;
      }

      dim += previousLead - currentLead - 1;

      if (dim >= upToDim)
      {
        break;
      }
      
      previousLead = currentLead;

      matrix[i].swap(matrix[maxIdx]);

      leads.push_back(currentLead);

      for (bv64 j = BV64(0); j < i; ++j)
      {
        if (matrix[j].get(currentLead))
        {
          matrix[j] ^= matrix[i];
        }
      }

      for (bv64 j = i + 1; j < nrows; ++j)
      {
        if (matrix[j].get(currentLead))
        {
          matrix[j] ^= matrix[i];
        }
      }
    }

    matrix.resize(i);

    return leads;
  }

  std::vector<ChunkVector> GJMatrix::prepareMatrix(const ChunkVector* rows, bv64 nrows, bv64 ncolumns)
  {
    std::vector<ChunkVector> matrix(nrows);

    for (auto i = BV64(0); i < nrows; ++i)
    {
      matrix[i] = rows[i].copy(ncolumns);
    }

    return matrix;
  }

  void GJMatrix::init(const ChunkVector* rows, bv64 nrows, bv64 ncolumns, bv64 upToDim)
  {
    auto matrix = prepareMatrix(rows, nrows, ncolumns);

    _leads = gauss(matrix, ncolumns, upToDim);
    
    _rows = matrix;
    _n = ncolumns;
  }

  ChunkVector GJMatrix::firstSolution() const
  {
    ChunkVector result(ncolumns());

    if (nondegenerate())
    {
      return result;
    }

    auto nonLead = ncolumns() - 1;
    
    for (size_t i = 0; i < _leads.size(); ++i)
    {
      if (_leads[i] != nonLead)
      {
        break;
      }

      --nonLead;
    }

    result.set(nonLead, true);

    for (size_t i = 0; i < _rows.size(); ++i)
    {
      result.set(_leads[i], _rows[i].get(nonLead));
    }

    return result;
  }
}
