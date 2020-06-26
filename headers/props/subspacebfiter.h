#pragma once

#include "bv.h"
#include "func/bf.h"

#include <vector>
#include <memory>

#include "props/utils/cosetstack.h"
#include "props/utils/sortingmap.h"

namespace bf
{
  class SubspaceBFIter 
  {
  public:
    SubspaceBFIter(const BF& f, bool includeCosets, bool includeAffinity, unsigned int startDim, unsigned int stopDim) :
      _f(f),
      _includeCosets(includeCosets),
      _includeAffinity(includeAffinity),
      _startDim(startDim),
      _stopDim(std::min(stopDim, (unsigned int)f.n())),
      _end(true),
      _ones(nullptr),
      _zeros(nullptr),
      _nones(BV16(0)),
      _nzeros(BV16(0)),
      _dim(BV16(0)),
      _affineCount(BV16(0)),
      _mode(0),
      _basis((size_t)f.n() + 1)
    {
      assert(f.n() <= 16);
      assert(f.n() < 16 || (f.weight() > 0 && f.weight() < basis64(16)));

      _gjbUpperBound = _includeAffinity ? basis32(f.n() - _startDim + 2) : basis32(f.n() - _startDim + 1);    
      _gjbUpperBound = std::min(_gjbUpperBound, basis32(f.n()));
    }

    SubspaceBFIter(const BF& f, bool includeCosets, unsigned int startDim, unsigned int stopDim, bool value) :
      SubspaceBFIter(f, includeCosets, false, startDim, stopDim)
    {
      _mode = ((unsigned int)value) + 1;
    }
    
    uint64_t requiredMemory() const;

  // current state intetface
  public:
    bool affine() const
    {
      return _affineCount > 0;
    }

    bv16 dim() const
    {
      return _dim;
    }

    bv16 nzeros() const
    {
      return _nzeros;
    }

    bv16 nones() const
    {
      return _nones;
    }

    const bv16* zeros() const
    {
      return _zeros;
    }

    const bv16* ones() const
    {
      return _ones;
    }

    const bv16* basis() const
    {
      return _basis.data() + 1;
    }

    bool withCosets() const
    {
      return _includeCosets;
    }

  public:
    SubspaceBFIter& begin();

    inline explicit operator bool() const
    {
      return !_end;
    }

    inline void operator++()
    {
      _end = !next();
    }

  private:
    inline bool found(bool affine, bv16 dim)
    {
      return _startDim <= dim && _stopDim >= dim && (_includeAffinity || !affine);
    }

    bool next();
    bool processTop();
    bv16 pushConstant();
    bv16 pushAffine();
    void mapConstant(bv16 gjbBoundary, const bv16* cosets, bv16 ncosets, bool one);

  private:
    void push(bv16 dim, bv16 basisVector, bv16 nzeros, const bv16* zeros, bv16 nones, const bv16* ones);
    void top();
    void pop();

    // current state
  private:
    bv16 _nzeros;
    bv16 _nones;
    bv16 _dim;
    std::vector<bv16> _basis;
    const bv16* _zeros;
    const bv16* _ones;

  private:
    BF _f;
    bool _includeCosets;
    bool _includeAffinity;
    unsigned int _startDim;
    unsigned int _stopDim;
    bv32 _gjbUpperBound;
    unsigned int _mode;
  
  private:
    std::unique_ptr<subspaceutils::CosetStack> _pStack;
    std::unique_ptr<subspaceutils::SortingMap> _pMap;

    bv16 _affineCount;

    bool _end;
  };
}
