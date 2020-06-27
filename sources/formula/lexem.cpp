#include "func/fpolynomial.h"
#include "vect/bvio.h"
#include "formula/lexem.h"

#include <sstream>
#include <vector>
#include <stack>
#include <cctype>

namespace lexem
{
  bool isDigit(int ch)
  {
    return '0' <= ch && ch <= '9';
  }

  bool isLetter(int ch)
  {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
  }

  Lexem NextLexem(std::istream &stream, const Lexem &previous, const std::vector<std::string> &functions, const std::vector<std::string>& constants)
  {
    if (previous.isConstant() && !previous.constant.special())
    {
      if (isDigit(stream.peek()))
      {
        return Lexem(ErrorLexem(stream.peek()), stream.tellg());
      }
    }

    char ch = 0;
    if (!(stream >> ch))
    {
      return stream.eof() ? Lexem() : Lexem(ErrorLexem('\0'), stream.tellg());
    }

    int64_t pos = stream.tellg();
    --pos;

    if (isDigit(ch))
    {
      stream.unget();

      bf::bv64 constant;
      
      if (stream >> bf::bv(constant))
      {
        return Lexem(ConstantLexem(constant), pos);
      }

      return Lexem(ErrorLexem(ch), pos);
    }
    
    if (ch == '@')
    {
      if (!(stream >> ch))
      {
        return Lexem(ErrorLexem('@'), pos);
      }

      if (isDigit(ch))
      {
        stream.unget();

        bf::bv64 constant;

        if (stream >> bf::bv(constant))
        {
          return Lexem(SpecialLexem(constant), pos);
        }        
      }
      else if (isLetter(ch))
      {
        std::string name{ ch };

        while (isLetter(stream.peek()))
        {
          name.append({ (char)std::tolower(stream.get())});
        }

        for (size_t i = 0; i < constants.size(); ++i)
        {
          if (name == constants[i])
          {
            return Lexem(ConstantLexem((int)i, true), pos);
          }
        }

        for (size_t i = 0; i < functions.size(); ++i)
        {
          if (name == functions[i])
          {
            return Lexem(OperationLexem((bf::bv8)i), pos);
          }
        }
      }

      return Lexem(ErrorLexem('@'), pos);
    }

    if (isLetter(ch))
    {
      bf::bv32 number = BV32(255);

      if (isDigit(stream.peek()))
      {
        if (!bf::ReadInteger(stream, number, std::ios_base::dec, true))
        {
          return Lexem(ErrorLexem(ch), pos);
        }

        return Lexem(VariableLexem(ch, (bf::bv8)number), pos);

      }

      return Lexem(VariableLexem(ch), pos);
    }

    switch (ch)
    {
    case '+':
      return Lexem(OperationLexem::Type::Plus, pos);
    
    case '-':
      return Lexem(OperationLexem::Type::Minus, pos);

    case '*':
      if (stream.peek() == '*')
      {
        stream.get();
        
        return Lexem(OperationLexem::Type::Pow, pos);
      }

      return Lexem(OperationLexem::Type::Mul, pos);
    
    case '/':
      return Lexem(OperationLexem::Type::Div, pos);
    
    case '^':
      return Lexem(OperationLexem::Type::Pow, pos);
    
    case '(':
      return Lexem(BracketLexem(true), pos);
    
    case ')':
      return Lexem(BracketLexem(false), pos);
    
    default:
      return Lexem(ErrorLexem(ch), pos);
    }
  }
}
