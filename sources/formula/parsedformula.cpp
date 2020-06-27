#include "func/fpolynomial.h"
#include "vect/bvio.h"

#include "formula/lexem.h"
#include "formula/parsedformula.h"

#include <sstream>
#include <vector>
#include <stack>

namespace lexem
{
  bool IsOperationMissed(const Lexem &previous, const Lexem &next)
  {
    return (previous.isOperand() && next.isOperand()) ||
      (previous.isOperand() && next.isBracket(true)) ||
      (previous.isBracket(false) && next.isOperand()) ||
      (previous.isBracket(false) && next.isBracket(true));
  }

  void ParsedFormula::createError(const Lexem& lexem, const char *msg)
  {
    std::ostringstream sstream;
    sstream << "position " << lexem.position << ": " << msg;
    error = sstream.str();
  }

  bool AddOperation(ParsedFormula &result, Lexem &lexem, bf::bv64 &nOperands)
  {
    auto req = lexem.operation.isUnary() ? 0 : 1;
    
    if (nOperands <= (bf::bv64)req)
    {
      result.createError(lexem, "operand expected");
      return false;
    }

    if (lexem.operation.startRationalContext())
    {
      result.hasRationalContext = true;

      for (auto i = lexem.operation.lastArgumentPosition; i < result.lexems.size(); ++i)
      {
        if (result.lexems[i].isOperation())
        {
          result.lexems[i].operation.isRational = true;
        }

        if (result.lexems[i].isConstant())
        {
          if (!result.lexems[i].constant.special())
          {
            result.lexems[i].constant.isRational = true;
          }
          else
          {
            result.createError(lexem, "special constant in rational expression");
            return false;
          }
        }

        if (result.lexems[i].isVariable())
        {
          result.createError(lexem, "variable in rational expression");
          return false;
        }

      }
    }

    result.lexems.push_back(lexem);
    
    nOperands -= req;

    return true;
  }

  bool ProcessLexem(ParsedFormula &result, std::stack<Lexem> &stack, Lexem &lexem, bf::bv64 &nOperands)
  {
    if (lexem.isOperand())
    {
      result.lexems.push_back(lexem);

      if (lexem.isVariable())
      {
        result.addVariable(lexem.variable);
      }

      if (lexem.isConstant() && lexem.constant.constant > result.maxConstant)
      {
        result.maxConstant = lexem.constant.constant;
      }

      ++nOperands;
      
      return true;
    }

    if (lexem.isBracket(true))
    {
      stack.push(lexem);
      return true;
    }

    if (lexem.isBracket(false))
    {
      while (!stack.empty() && !stack.top().isBracket(true))
      {
        if (!AddOperation(result, stack.top(), nOperands))
        {
          return false;
        }
        
        stack.pop();
      }
      
      if (stack.empty())
      {
        result.createError(lexem, "'(' should be before ')'");
        return false;
      }
      
      stack.pop();
      
      return true;
    }

    if (lexem.isOperation())
    {
      if (lexem.operation.isUnary())
      {
        stack.push(lexem);
        
        return true;
      }

      while (!stack.empty() && stack.top().isOperation() &&
        stack.top().operation.morePriority(lexem.operation))
      {
        if (!AddOperation(result, stack.top(), nOperands))
        {
          return false;
        }
       
        stack.pop();
      }

      if (lexem.operation.startRationalContext())
      {
        lexem.operation.lastArgumentPosition = (bf::bv64)result.lexems.size();
      }

      stack.push(lexem);
      
      return true;
    }

    return true;
  }
  
  ParsedFormula ParseFormula(std::istream &stream, const std::vector<std::string> &functions, const std::vector<std::string>& constants, char delimiter)
  {
    ParsedFormula result;
    std::stack<Lexem> stack;
    bf::bv64 nOperands = 0;

    Lexem previousLexem;
    Lexem lexem = NextLexem(stream, previousLexem, functions, constants);

    while (lexem.isOk())
    {
      if (lexem.isOperation() 
        && (lexem.operation.type == OperationLexem::Type::Minus 
          || lexem.operation.type == OperationLexem::Type::Plus))
      {
        if (previousLexem.isEmpty() 
          || previousLexem.isOperation() 
          || previousLexem.isBracket(true))         
        {
          if (lexem.operation.type == OperationLexem::Type::Plus)
          {
            lexem = NextLexem(stream, previousLexem, functions, constants);
            continue;
          }

          lexem.operation.type = OperationLexem::Type::UMinus;
        }
      }
      
      if (IsOperationMissed(previousLexem, lexem))
      {
        auto mult = Lexem(OperationLexem::Type::Mul, lexem.position);

        if (!ProcessLexem(result, stack, mult, nOperands))
        {
          return result;
        }
      }

      if (!ProcessLexem(result, stack, lexem, nOperands))
      {
        return result;
      }
      
      previousLexem = lexem;
      lexem = NextLexem(stream, previousLexem, functions, constants);
    }

    if (lexem.isError() && lexem.error.symbol != delimiter)
    {
      result.createError(lexem, "lexem is wrong, start symbol ");
      result.error.append({ lexem.error.symbol });
      
      return result;
    }

    if (lexem.isSpecial())
    {
      result.endWithSpecial = true;
      result.specialConstant = lexem.special.c;
    }

    if (result.lexems.empty())
    {
      result.createError(lexem, "empty source");
      return result;
    }

    while (!stack.empty())
    {
      if (!AddOperation(result, stack.top(), nOperands))
      {
        return result;
      }
      
      stack.pop();
    }

    return result;
  }

  std::vector<char> ParsedFormula::Variables = {
  'x', 'y', 'z', 'u', 'v', 'w', 'X', 'Y', 'Z', 'U', 'V', 'W', 't',
  's', 'r', 'q', 'p', 'o', 'n', 'm', 'l', 'k', 'j', 'i', 'h', 'g',
  'f', 'e', 'd', 'c', 'b', 'a', 'T', 'S', 'R', 'Q', 'P', 'O', 'N',
  'M', 'L', 'K', 'J', 'I', 'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'
  };

  std::vector<unsigned int> ParsedFormula::InvertedVariables = { 
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    255, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 
    36, 35, 34, 33, 32, 9, 10, 11, 6, 7, 8, 255, 255, 255, 255, 255, 
    255, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 
    16, 15, 14, 13, 12, 3, 4, 5, 0, 1, 2, 255, 255, 255, 255, 255
  };
}
