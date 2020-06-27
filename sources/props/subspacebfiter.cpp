#include "bv.h"
#include "func/bf.h"
#include "props/subspacebfiter.h"

#include <vector>
#include <iostream>
#include <cstring>

namespace bf
{
  uint64_t SubspaceBFIter::requiredMemory() const
  {
    auto nones = _f.weight();
    auto nzeros = _f.size() - nones;

    return 2 * (subspaceutils::CosetStack::size(nones, nzeros) + subspaceutils::SortingMap::size(nones, nzeros));
  }

  void SubspaceBFIter::push(bv16 dim, bv16 basisVector, bv16 nzeros, const bv16* zeros, bv16 nones, const bv16* ones)
  {
    // zeros
    _pStack->push(zeros, nzeros);
    _pStack->push(nzeros);

    // ones
    _pStack->push(ones, nones);
    _pStack->push(nones);

    // dimension and basis vector
    _pStack->push(basisVector);
    _pStack->push(dim);
  }
  
  void SubspaceBFIter::top()
  {
    _dim = *_pStack->peekTop();
    _basis[_dim] = *_pStack->peekTop(1);

    _nones = *_pStack->peekTop(2);
    _ones = _pStack->peekTop((uint64_t)_nones + 2);

    _nzeros = *_pStack->peekTop((uint64_t)_nones + 3);
    _zeros = _pStack->peekTop((uint64_t)_nzeros + _nones + 3);
  }
  
  void SubspaceBFIter::pop()
  {
    _pStack->pop((uint64_t)4 + _nones + _nzeros);
  }

  bv16 SubspaceBFIter::pushConstant()
  {
    auto minElement = bf::max16(bf::size(_basis[_dim]));

    if (!_includeCosets)
    {
      if (_nzeros > 0 && zero(*_zeros))
      {
        _pMap->AddKeys(_zeros + 1, _nzeros - BV16(1));
      }
      else if (_nones > 0 && zero(*_ones))
      {
        _pMap->AddKeys(_ones + 1, _nones - BV16(1));
      }
      else
      {
        return BV16(0);
      }
    }

    mapConstant(minElement, _zeros, _nzeros, false);
    mapConstant(minElement, _ones, _nones, true);

    return (bv16)_pMap->push(_f, _dim + BV16(1), *_pStack);
  }

  void SubspaceBFIter::mapConstant(bv16 gjbBoundary, const bv16 *cosets, bv16 ncosets, bool one)
  {
    auto upperBound = _gjbUpperBound << dim();

    for (auto i = 0; i < ncosets; ++i)
    {
      for (auto j = i + 1; j < ncosets; ++j)
      {
        bv16 nextVector = cosets[i] ^ cosets[j];

        if (nextVector <= gjbBoundary || nextVector >= upperBound)
        {
          continue;
        }

        _pMap->Add(nextVector, cosets[i], one);
      }
    }
  }

  bv16 SubspaceBFIter::pushAffine()
  {

    if (!_includeCosets)
    {
      if (_nzeros > 0 && zero(*_zeros))
      {
        _pMap->AddKeys(_ones, _nones);
      }
      else if (_nones > 0 && zero(*_ones))
      {
        _pMap->AddKeys(_zeros, _nzeros);
      }
      else
      {
        return BV16(0);
      }
    }

    for (auto i = 0; i < _nzeros; ++i)
    {
      for (auto j = 0; j < _nones; ++j)
      {
        if (!_includeCosets && !zero(_zeros[i]) && !zero(_ones[j]))
        {
          continue;
        }

        bv16 nextVector = _zeros[i] ^ _ones[j];

        if (_zeros[i] < _ones[j])
        {
          _pMap->Add(nextVector, _zeros[i], false);
        }
        else
        {
          _pMap->Add(nextVector, _ones[j], true);
        }
      }
    }
    
    return (bv16)_pMap->push(_f, _dim + BV16(1), *_pStack);
  }

  bool SubspaceBFIter::processTop()
  {
    // fill current state from the stack top
    top();

    // affinity case is processed firstly
    if (found(true, _dim + BV16(1)))
    {
      // create cosets for affinity case
      _affineCount = pushAffine();

      if (_affineCount > 0)
      {
        top();

        return true;
      }
    }

    return found(false, _dim);
  }

  bool SubspaceBFIter::next()
  {
    if (_affineCount >= 1)
    {
      --_affineCount;
      
      pop();
      
      top();

      if (_affineCount > 0 || found(false, _dim))
      {
        return true;
      }
    }

    while (true)
    {
      pop();

      if (_dim < _stopDim)
      {
        pushConstant();
      }

      if (_pStack->empty())
      {
        return false;
      }

      if (processTop())
      {
        return true;
      }
    }
    
    return false;
  }

  SubspaceBFIter& SubspaceBFIter::begin()
  {
    std::vector<bv16> zeros;
    std::vector<bv16> ones;
    
    for (auto i : _f.dom())
    {
      if (_f.get(i))
      {
        if (_mode != 1)
        {
          ones.push_back(i);
        }
      }
      else
      {
        if (_mode != 2)
        {
          zeros.push_back(i);
        }
      }
    }

    if ((zeros.size() == 0 && ones.size() == 0) ||
      (_mode != 0 && !_includeCosets && _f.get(0) != (_mode == 2)))
    {
      _end = true;
      return *this;
    }

    _pStack = std::make_unique<subspaceutils::CosetStack>(zeros.size(), ones.size());
    _pMap = std::make_unique<subspaceutils::SortingMap>(_f.n(), zeros.size(), ones.size());

    _affineCount = 0;
    _end = false;

    // prepare initial state
    push(BV16(0), BV16(0), (bv16)zeros.size(), zeros.data(), (bv16)ones.size(), ones.data());

    if (!processTop())
    {
      _end = !next();
    }

    return *this;
  }
}
