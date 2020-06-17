#pragma once

#include <cctype>
#include <istream>
#include <string>

#include "bfa.h"
#include "bv.h"

namespace bf
{
  template<typename T>
  struct bv_io
  {    
    bv_io(T &value, bv8 length)
      :
      _value(value),
      length(length),
      _constant(0)
    {
    }

    bv_io(const T &&value, bv8 length)
      :
      _constant(value),
      _value(_constant),
      length(length)
    {
    }
   
    void operator=(const bv_io&) = delete;

    const bv8 length;
    T& _value;

  private:
    T _constant;
  };

  inline bv_io<bv8> bv(bv8 &vect, bv8 length = 255)
  {
    return bv_io<bv8>(vect, length);
  }

  inline bv_io<bv32> bv(bv32 &vect, bv8 length = 255)
  {
    return bv_io<bv32>(vect, length);
  }

  inline bv_io<bv64> bv(bv64 &vect, bv8 length = 255)
  {
    return bv_io<bv64>(vect, length);
  }

  inline const bv_io<bv8> bv(const bv8 &vect, bv8 length = 255)
  {
    return bv_io<bv8>((bv8)vect, length);
  }

  inline const bv_io<bv32> bv(const bv32 &vect, bv8 length = 255)
  {
    return bv_io<bv32>((bv32)vect, length);
  }

  inline const bv_io<bv64> bv(const bv64 &vect, bv8 length = 255)
  {
    return bv_io<bv64>((bv64)vect, length);
  }

  inline const bv_io<bv8> bv(const int8_t &vect, bv8 length = 255)
  {
    return bv_io<bv8>((bv8)vect, length);
  }

  inline const bv_io<bv32> bv(const int32_t &vect, bv8 length = 255)
  {
    return bv_io<bv32>((bv32)vect, length);
  }

  inline const bv_io<bv64> bv(const int64_t &vect, bv8 length = 255)
  {
    return bv_io<bv64>((bv64)vect, length);
  }

  std::istream& operator>>(std::istream &stream, bv_io<bv8> &read);
  std::istream& operator>>(std::istream &stream, bv_io<bv32> &read);
  std::istream& operator>>(std::istream &stream, bv_io<bv64> &read);

  std::ostream& operator<<(std::ostream &stream, const bv_io<bv8> &write);
  std::ostream& operator<<(std::ostream &stream, const bv_io<bv32> &write);
  std::ostream& operator<<(std::ostream &stream, const bv_io<bv64> &write);

  
  // it should be moved to private utilities
  template <typename T>
  inline std::istream& ReadInteger(std::istream &stream, T &ovect, int flag, bool skipWhitespaces)
  {
    if (!skipWhitespaces)
    {
      auto ch = stream.peek();

      if (ch != EOF && std::isspace((char)ch))
      {
        stream.setstate(std::ios_base::failbit);

        return stream;
      }
    }

    auto flags = stream.flags();

    stream.setf(flag, std::ios_base::basefield);

    bf::bv64 vect;

    stream >> vect;

    if (static_cast<bf::bv64>(static_cast<T>(vect)) != vect)
    {
      stream.setstate(std::ios_base::failbit);
    }
    else
    {
      ovect = static_cast<T>(vect);
    }

    stream.flags(flags);

    return stream;
  }
}
