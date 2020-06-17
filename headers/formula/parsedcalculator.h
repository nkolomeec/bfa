#pragma once

#include "formula/parsedformula.h"
#include "integer/rational.h"

#include <stack>

namespace lexem
{
  template<typename T>
  struct ParsedCalculator : public lexem::ParsedFormula
  {
    T value;

    ParsedCalculator(const ParsedFormula &f)
      :
      ParsedFormula(f)
    {
    }

    virtual void plus(T &a, const T& b) = 0;

    virtual void minus(T& a, const T& b) = 0;

    virtual void minus(T& a) = 0;

    virtual void mult(T& a, const T& b) = 0;

    virtual bool div(T& a, const T& b)
    {
      return false;
    }

    virtual bool pow(T& a, const integer::Rational &b)
    {
      return false;
    }

    virtual bool function(T& a, bf::bv8 functionId)
    {
      return false;
    }

    virtual T convertConstant(bf::bv64 a) = 0;

    virtual T convertVariable(unsigned int globalId) = 0;

    bool calculate()
    {
      std::stack<T> stack;
      std::stack<integer::Rational> rationalStack;

      for (auto &l : lexems)
      {
        if (l.isOperand())
        {
          if (l.isConstant())
          {
            if (l.constant.isRational)
            {
              rationalStack.push(integer::Rational((int64_t)l.constant.constant));
            }
            else
            {
              stack.push(convertConstant(l.constant.constant));
            }
          }

          if (l.isVariable())
          {
            stack.push(convertVariable(this->variableId(l.variable)));
          }

          continue;
        }

        if (!l.isOperation())
        {
          return false;
        }

        if (l.operation.isRational)
        {
          if (rationalStack.size() <= (l.operation.isUnary() ? 0 : 1))
          {
            return false;
          }

          if (l.operation.isUnary())
          {
            switch (l.operation.type)
            {
              case lexem::OperationLexem::Type::UMinus:
                rationalStack.top().minus();
                break;
              default:
                return false;
            }

            continue;
          }

          auto b = rationalStack.top();
          rationalStack.pop();

          switch (l.operation.type)
          {
          case lexem::OperationLexem::Type::Plus:
            rationalStack.top().plus(b);
            break;
          case lexem::OperationLexem::Type::Minus:
            rationalStack.top().minus(b);
            break;
          case lexem::OperationLexem::Type::Mul:
            rationalStack.top().mult(b);
            break;
          case lexem::OperationLexem::Type::Div:
            if (!rationalStack.top().div(b))
            {
              createError(l, "rational division is failed");
              return false;
            }
            break;
          case lexem::OperationLexem::Type::Pow:
            if (!rationalStack.top().pow(b))
            {
              createError(l, "rational exponentation is failed");
              return false;
            }
            break;
          default:
            return false;
          }

          continue;
        }

        if (l.operation.type == lexem::OperationLexem::Type::Pow)
        {
          if (stack.empty() || rationalStack.empty())
          {
            return false;
          }

          auto b = rationalStack.top();
          rationalStack.pop();

          if (!pow(stack.top(), b))
          {
            createError(l, "exponentation is failed");
            return false;
          }

          continue;
        }

        if (l.operation.isUnary())
        {
          if (stack.empty())
          {
            return false;
          }

          switch (l.operation.type)
          {
          case lexem::OperationLexem::Type::UMinus:
            minus(stack.top());
            break;
          case lexem::OperationLexem::Type::UFunction:
            if (!function(stack.top(), l.operation.functionId))
            {
              return false;
            }
            
            break;
          default:
            return false;
          }
          
          continue;
        }

        if (stack.size() < 2)
        {
          return false;
        }

        auto b = stack.top();
        stack.pop();

        switch (l.operation.type)
        {
        case lexem::OperationLexem::Type::Plus:
          plus(stack.top(), b);
          break;
        case lexem::OperationLexem::Type::Minus:
          minus(stack.top(), b);
          break;
        case lexem::OperationLexem::Type::Mul:
          mult(stack.top(), b);
          break;
        case lexem::OperationLexem::Type::Div:
          if (div(stack.top(), b))
          {
            error = "division is failed";
            return false;
          }
          break;
        default:
          return false;
        }
      }

      if (stack.empty())
      {
        return false;
      }

      value = stack.top();

      return true;
    }
  };
}

