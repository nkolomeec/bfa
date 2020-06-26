#pragma once

#include "bv.h"
#include "func/bf.h"

#include <vector>

namespace bf
{
  namespace subspaceutils
  {
    class SortingMap
    {
    public:
      SortingMap(int n, bv64 nzeros, bv64 nones) :
        _nkeys(BV32(0)),
        _count(BV64(0)),
        _keys(basis32(n)),
        _keyIndices(basis32(n), -1),
        _nzeros(basis32(n)),
        _nones(basis32(n)),
        _map(size(nzeros, nones)),
        _tmp(basis32(n)),
        _skipUnknown(false)
      {
      }

      void AddKeys(const bv16* keys, bv16 count)
      {
        for (auto i = BV16(0); i < count; ++i)
        {
          AddKey(keys[i]);
        }

        _skipUnknown = true;
      }

      void Add(bv16 key, bv16 value, bool one)
      {
        if (_keyIndices[key] < 0)
        {
          if (_skipUnknown)
          {
            return;
          }

          AddKey(key);
        }

        auto keyId = _keyIndices[key];

        if (one)
        {
          _nones[keyId]++;
        }
        else
        {
          _nzeros[keyId]++;
        }

        _map[_count << 1] = keyId;
        _map[(_count << 1) + 1] = value;

        ++_count;
      }

      void clear()
      {
        for (auto i = BV32(0); i < _nkeys; ++i)
        {
          _nzeros[i] = 0;
          _nones[i] = 0;

          _keyIndices[_keys[i]] = -1;
        }

        _nkeys = 0;
        _count = 0;
      }

      bv32 push(const BF& f, bv16 dim, CosetStack& stack)
      {
        auto top = stack.top();

        int64_t nelems = 0;
        bv16 nkeys = 0;
        for (auto i = BV32(0); i < _nkeys; ++i)
        {
          if (zero(_nzeros[i]) && zero(_nones[i]))
          {
            continue;
          }

          ++nkeys;

          nelems += _nzeros[i];
          nelems += _nones[i];
          nelems += 4;

          _tmp[i] = nelems;

          auto end = top + nelems;

          stack.set(--end, dim);
          stack.set(--end, _keys[i]);
          stack.set(--end, _nones[i]);

          end -= _nones[i];
          stack.set(--end, _nzeros[i]);
        }

        for (auto i = BV64(0); i < _count; ++i)
        {
          auto keyId = _map[i << 1];
          auto coset = _map[(i << 1) + 1];
          auto idx = top;

          if (f.get(coset))
          {
            idx += _tmp[keyId] - 3 - _nones[keyId];
            --_nones[keyId];
          }
          else
          {
            idx += _tmp[keyId] - 3;
            idx -= (uint64_t)*stack.peek(idx) + _nzeros[keyId] + 1;
            --_nzeros[keyId];
          }

          stack.set(idx, coset);
        }

        stack.reserve(nelems);

        clear();

        return nkeys;
      }

    public:
      inline static bv64 size(bv64 nzeros, bv64 nones)
      {
        return nzeros * nzeros + nones * nones;
      }

    private:
      inline void AddKey(bv16 key)
      {
        _keys[_nkeys] = key;
        _keyIndices[key] = _nkeys;
        ++_nkeys;
      }

    private:
      bv32 _nkeys;
      bv64 _count;
      std::vector<bv16> _keys;
      std::vector<int32_t> _keyIndices;
      std::vector<bv16> _nzeros;
      std::vector<bv16> _nones;
      std::vector<bv16> _map;
      std::vector<int64_t> _tmp;
      bool _skipUnknown;
    };
  }
}
