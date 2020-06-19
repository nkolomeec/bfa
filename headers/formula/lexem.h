#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace lexem
{
  struct BracketLexem
  {
    bool isOpen;

    BracketLexem()
      :
      BracketLexem(false)
    {
    }

    BracketLexem(bool isOpen)
    {
      this->isOpen = isOpen;
    }

    void info()
    {
      std::cout << (isOpen ? '(' : ')');
    }
  };

  struct ConstantLexem
  {
    bf::bv64 constant;

    bool isRational;

    ConstantLexem(bf::bv64 constant)
    {
      this->constant = constant;
      isRational = false;

    }

    void info()
    {
      std::cout << constant;
    }
  };

  struct VariableLexem
  {
    char symbol;
    bf::bv8 number;

    VariableLexem(char symbol, bf::bv8 number)
    {
      this->symbol = symbol;
      this->number = number;
    }

    explicit VariableLexem(char symbol)
      :
      VariableLexem(symbol, BV8(255))
    {
    }

    bool isNumbered() const
    {
      return number != BV8(255);
    }

    void info()
    {
      std::cout << symbol;
      
      if (isNumbered())
      {
        std::cout << (int)number;
      }
    }

  };

  struct ErrorLexem
  {
    char symbol;

    ErrorLexem(char symbol)
    {
      this->symbol = symbol;
    }

    void info()
    {
      std::cout << "Error: ";
      if (symbol != 0)
      {
        std::cout << symbol;
      }
      else
      {
        std::cout << "no symbol";
      }
    }
  };

  struct OperationLexem
  {
    enum class Type
    {
      Plus = 0,
      Minus,
      Mul,
      Div,
      Pow,
      UMinus,
      UFunction
    };

    Type type;

    bool isRational;

    bf::bv8 functionId;

    bf::bv64 lastArgumentPosition;

    OperationLexem(Type type)
    {
      this->type = type;
      isRational = false;
      lastArgumentPosition = 0;
      functionId = 0;
    }

    OperationLexem(bf::bv8 functionId)
    {
      this->type = Type::UFunction;
      this->functionId = functionId;
      isRational = false;
      lastArgumentPosition = 0;
    }

    void info()
    {
      std::cout << (int)type;
      
      if (type == Type::UFunction)
      {
        std::cout << (int)functionId;
      }

      std::cout << ", " << isRational;
    }

    bool isUnary() const
    {
      return type == Type::UMinus || type == Type::UFunction;
    }

    bool isRightAssociative() const
    {
      return type == Type::Pow;
    }

    unsigned int priority() const
    {
      switch (type)
      {
      case Type::Plus:
        return 1;
      case Type::Minus:
        return 1;
      case Type::Mul:
        return 2;
      case Type::Div:
        return 2;
      case Type::Pow:
        return 5;
      case Type::UMinus:
        return 4;
      case Type::UFunction:
        return 3;
      default:
        return 0;
      }
    }

    bool morePriority(const OperationLexem &op) const
    {
      auto p = priority();
      auto p1 = op.priority();

      return p > p1 || (p == p1 && !isRightAssociative());
    }

    bool startRationalContext() const
    {
      return type == Type::Pow;
    }
  };

  struct SpecialLexem
  {
    bf::bv64 c;

    SpecialLexem(bf::bv64 c)
    {
      this->c = c;
    }

    void info()
    {
      std::cout << "@ " << c;
    }
  };
  
  struct Lexem
  {
    enum class Type
    {
      None = 0,
      Special,
      Error,
      Constant,
      Variable,
      Bracket,
      Operation
    };

    Type type;

    int64_t position;

    union
    {
      ErrorLexem error;
      ConstantLexem constant;
      VariableLexem variable;
      BracketLexem bracket;
      OperationLexem operation;
      SpecialLexem special;
    };

    Lexem()
      :
      Lexem(0)
    {
    }

    Lexem(uint64_t pos)
      :
      type(Type::None),
      error('\0'),
      position(pos)
    {
    }

    Lexem(OperationLexem::Type op, int64_t pos)
      :
      position(pos),
      type(Type::Operation)
    {
      operation = OperationLexem(op);
    }
 
    Lexem(const OperationLexem &o, int64_t pos)
      :
      position(pos),
      type(Type::Operation)
    {
      operation = o;
    }

    Lexem(const ConstantLexem &c, int64_t pos)
      :
      position(pos),
      type(Type::Constant)
    {
      constant = c;
    }

    Lexem(const VariableLexem &v, int64_t pos)
      :
      position(pos),
      type(Type::Variable)
    {
      variable = v;
    }

    Lexem(const BracketLexem &b, int64_t pos)
      :
      position(pos),
      type(Type::Bracket)
    {
      bracket = b;
    }

    Lexem(const ErrorLexem &e, int64_t pos)
      :
      position(pos),
      type(Type::Error)
    {
      error = e;
    }

    Lexem(const SpecialLexem &s, int64_t pos)
      :
      position(pos),
      type(Type::Special)
    {
      special = s;
    }

    bool isOk() const
    {
      return type != Type::None && type != Type::Error && type != Type::Special;
    }

    bool isEmpty() const
    {
      return type == Type::None;
    }

    bool isError() const
    {
      return type == Type::Error;
    }

    bool isSpecial() const
    {
      return type == Type::Special;
    }

    bool isOperand() const
    {
      return type == Type::Constant || type == Type::Variable;
    }

    bool isOperation() const
    {
      return type == Type::Operation;
    }

    bool isBracket(bool open) const
    {
      return type == Type::Bracket && bracket.isOpen == open;
    }

    bool isConstant() const
    {
      return type == Type::Constant;
    }

    bool isVariable() const
    {
      return type == Type::Variable;
    }

    void info()
    {
      switch (type)
      {
      case Lexem::Type::None:
        std::cout << "None";
        break;
      case Lexem::Type::Error:
        error.info();
        break;
      case Lexem::Type::Constant:
        constant.info();
        break;
      case Lexem::Type::Variable:
        variable.info();
        break;
      case Lexem::Type::Bracket:
        bracket.info();
        break;
      case Lexem::Type::Operation:
        operation.info();
        break;
      case Lexem::Type::Special:
        special.info();
        break;      
      default:
        break;
      }
      
      std::cout << " pos = " << position;
    }
  };

  Lexem NextLexem(std::istream &stream, const Lexem &previousLexem, const std::vector<std::string> &functions);
}

