#pragma once

#include "bv.h"

#include <cstring>

namespace bf
{
  // Represent binary "chunk" vector (64 bit chunk size)
  // The vector stores only chunk size, not bit size!
  // For bit vectors of size at most 64 it does not allocate dynamic memory
  class ChunkVector final
  {
  // constructors & destructor
  public:
    inline ChunkVector()
      :
      _staticData(0),
      _pData(&_staticData)
    {
    }

    inline explicit ChunkVector(bv64 nbits)
      : 
      ChunkVector()
    {
      auto size = calculateSize(nbits);
      
      if (size > 1)
      {
        _pData = new bv64[size];
        _staticData = size;

        std::memset(_pData, 0, sizeof(bv64) * size);
      }
    }

    inline ChunkVector(const ChunkVector &v)
      :
      _staticData(v._staticData),
      _pData(&_staticData)
    {
      if (!v.isStatic())
      {
        _pData = new bv64[_staticData];
        
        std::memcpy(_pData, v._pData, sizeof(bv64) * _staticData);
      }
    }

    inline ChunkVector(ChunkVector &&v) noexcept
      : ChunkVector()
    {
      swap(v);
    }

    inline ~ChunkVector() noexcept
    {
       if (!isStatic())
       {
         delete[] _pData;
       }
    }

    // operators
  public:
    inline ChunkVector& operator=(ChunkVector v)
    {
      swap(v);

      return *this;
    }

    inline ChunkVector& operator^=(const ChunkVector &v) noexcept
    {
      auto size = this->size();

      assert(size == v.size());

      for (auto i = BV64(0); i < size; ++i)
      {
        _pData[i] ^= v._pData[i];
      }

      return *this;
    }

    inline ChunkVector operator^(const ChunkVector &v) const noexcept
    {
      ChunkVector vv(*this);
      
      vv ^= v;
      
      return vv;
    }
  
    inline ChunkVector& operator&=(const ChunkVector &v) noexcept
    {
      auto size = this->size();

      assert(size == v.size());

      for (auto i = BV64(0); i < size; ++i)
      {
        _pData[i] &= v._pData[i];
      }

      return *this;
    }

    inline ChunkVector operator&(const ChunkVector &v) const noexcept
    {
      ChunkVector vv(*this);

      vv &= v;

      return vv;
    }

    inline bv64 operator[](bv64 index) const noexcept
    {
      assert(index < size());
      
      return _pData[index];
    }

    inline bv64& operator[](bv64 index) noexcept
    {
      assert(index < size());

      return _pData[index];
    }

    // general operations
  public:
    inline void resize(bv64 nbits)
    {
      auto oldSize = size();
      auto newSize = calculateSize(nbits);
      
      if (newSize == oldSize)
      {
        return;
      }

      ChunkVector v(nbits);
      
      swap(v);
      
      auto size = std::min(newSize, oldSize);
      
      std::memcpy(_pData, v._pData, sizeof(bv64) * size);
    }

    inline void swap(ChunkVector &v) noexcept
    {
      if (this->isStatic())
        this->_pData = &v._staticData;

      if (v.isStatic())
        v._pData = &this->_staticData;

      std::swap(this->_pData, v._pData);
      std::swap(this->_staticData, v._staticData);
    }

    inline bv64 bitSize() const noexcept
    {
      return size() << 6;
    }
    
    inline bv64 size() const noexcept
    {
      return isStatic() ? 1 : _staticData;
    }

    inline ChunkVector copy(bv64 nbits) const
    {
      ChunkVector result(nbits);

      auto minSize = std::min(result.size(), size());

      std::memcpy(result._pData, _pData, sizeof(bv64) * minSize);

      auto rest = nbits & BV64(63);

      if (nbits < bitSize() && rest != 0)
      {
        result[minSize - 1] &= max64((bv8)rest);
      }

      return result;
    }

    bool operator*(const ChunkVector& v)
    {
      auto minSize = std::min(size(), v.size());

      bv64 result = 0;

      for (auto i = BV64(0); i < minSize; ++i)
      {
        result ^= (*this)[i] & v[i];
      }

      return bf::odd(result);
    }

  // get/set bits and block of bits
  public:
    inline bool get(bv64 index) const noexcept
    {
      assert(index < bitSize());
      
      return bf::get(_pData[index >> 6], static_cast<bv8>(index & BV32(63)));
    }

    inline void set(bv64 index, bool val) noexcept
    {
      assert(index < bitSize());

      bf::set(_pData[index >> 6], static_cast<bv8>(index & BV32(63)), val);
    }

    inline void set(bool val) noexcept
    {
      std::memset(_pData, static_cast<int>(val) * 0xFF, sizeof(bv64) * size());
    }

    inline bv32 getAligned(bv64 index, bv8 length) const noexcept
    {
      assert(index + length <= bitSize());
      assert(length > 0 && length <= 32);

      return (_pData[index >> 6] >> (index & BV64(63))) & ((BV64(1) << length) - 1);
    }
      
    inline void setAligned(bv64 index, bv32 val, bv8 length) const noexcept
    {
      assert(index + length <= bitSize());
      assert(length > 0 && length <= 32);
      assert(val < (BV64(1) << length));

      auto chunkIdx = index >> 6;
      auto shift = index & BV64(63);
      
      _pData[chunkIdx] ^= (_pData[chunkIdx] & (((BV64(1) << length) - 1) << shift)) ^ (static_cast<bv64>(val) << shift);
    }

    inline bv32 get(bv64 index, bv8 length) const noexcept
    {
      assert(index + length <= bitSize());
      assert(length > 0 && length <= 32);

      bv64 chunkIdx = index >> 6;
      bv64 shift = index & BV64(63);
      
      bv64 extracted = _pData[chunkIdx] >> shift;

      if (chunkIdx < ((index + length - 1) >> 6))
      {
        extracted = extracted ^ (_pData[chunkIdx + 1] << (64 - shift));
      }

      return extracted & ((BV64(1) << length) - 1);
    }

    inline void set(bv64 index, bv32 val, bv8 length) const noexcept
    {
      assert(index + length <= bitSize());
      assert(length > 0 && length <= 32);
      assert(val < (BV64(1) << length));

      auto chunkIdx = index >> 6;
      auto shift = index & BV64(63);

      _pData[chunkIdx] ^= (_pData[chunkIdx] & (((BV64(1) << length) - 1) << shift)) ^ (static_cast<bv64>(val) << shift);

      if (chunkIdx < ((index + length - 1) >> 6))
      {
        _pData[chunkIdx + 1] ^= (_pData[chunkIdx + 1] & ((BV64(1) << (length - (64 - shift))) - 1)) ^ 
          (static_cast<bv64>(val) >> (64 - shift));
      }
    }

    void invert(bv32 index)
    {
      assert(index < bitSize());

      bf::invert(_pData[index >> 6], static_cast<bv8>(index & BV32(63)));
    }

  private:
    inline bool isStatic() const noexcept
    {
      return _pData == &_staticData;
    }

    inline bv64 calculateSize(bv64 nbits) const noexcept
    {
      return (nbits + BV64(63)) >> 6;
    }

  private:
    bv64 _staticData;
    bv64 *_pData;
  };
}
