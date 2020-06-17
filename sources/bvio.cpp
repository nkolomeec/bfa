#include "vect/bvio.h"
#include "vect/enum.h"

#include <cctype>

namespace
{ 
  bool checkInterval(int ch, char min, char max)
  {
    return ch >= min && ch <= max;
  }

  template <typename T>
  std::istream& ReadInteger(std::istream &stream, T &ovect)
  {
    char ch = 0;
    
    if (!(stream >> ch))
    {
      return stream;
    }

    if (ch != '0') 
    {
      stream.unget();
      
      return bf::ReadInteger(stream, ovect, std::ios::dec, true);
    }

    auto next = stream.peek();

    if (next == EOF)
    {
      ovect = (T)0;

      return stream;
    }

    if (checkInterval(next, '0', '7'))
    {
      return bf::ReadInteger(stream, ovect, std::ios::oct, false);
    }

    if (next == 'x' || next == 'X')
    {
      stream.get();

      return bf::ReadInteger(stream, ovect, std::ios::hex, false);
    }

    if (next == 'b' || next == 'B')
    {
      stream.get();

      T vect = (T)0;
      auto maxSize = BV8(sizeof(T) * 8);
      bool fail = true;

      for (auto i = BV8(0); next != EOF;)
      {
        next = stream.peek();

        if (!checkInterval(next, '0', '1'))
        {
          break;
        }

        if (i >= maxSize)
        {
          fail = true;      
          break;
        }

        fail = false;

        stream.get();

        vect <<= 1;
        bf::set(vect, 0, (next == '1'));

        if (vect != (T)0)
        {
          ++i;
        }
      }

      if (fail)
      {
        stream.setstate(std::ios_base::failbit);
      }
      else
      {
        ovect = vect;
      }
      
      return stream;
    }

    ovect = (T)0;

    return stream;
  }

  template<typename T>
  std::istream& OperatorRead(std::istream &stream, bf::bv_io<T> &read)
  {
    T ovect;

    ReadInteger(stream, ovect);

    if (!stream)
    {
      return stream;
    }

    if (bf::size(ovect) <= read.length)
    {
      read._value = ovect;
    }
    else
    {
      stream.setstate(std::ios::failbit);
    }

    return stream;
  }

  template<typename T>
  std::ostream& OperatorWrite(std::ostream &stream, const bf::bv_io<T> &write)
  {
    auto value = write._value;
    
    int i = write.length == 255 
      ? (int)bf::lead(value) 
      : (int)std::min<bf::bv8>(write.length, (bf::bv8)(sizeof(T) * 8)) - 1;

    for (; i >= 0; --i)
    {
      stream << bf::get(value, (bf::bv8)i);
    }

    return stream;
  }
}

namespace bf 
{
  std::istream& operator>>(std::istream &stream, bv_io<bv8> &read)
  {
    return OperatorRead(stream, read);
  }

  std::istream& operator>>(std::istream &stream, bv_io<bv32> &read)
  {
    return OperatorRead(stream, read);
  }
  
  std::istream& operator>>(std::istream &stream, bv_io<bv64> &read)
  {
    return OperatorRead(stream, read);
  }

  std::ostream& operator<<(std::ostream &stream, const bv_io<bv8> &write)
  {
    return OperatorWrite(stream, write);
  }
  
  std::ostream& operator<<(std::ostream &stream, const bv_io<bv32> &write)
  {
    return OperatorWrite(stream, write);
  }
  
  std::ostream& operator<<(std::ostream &stream, const bv_io<bv64> &write)
  {
    return OperatorWrite(stream, write);
  }
}
