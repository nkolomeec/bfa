#pragma once

#include "formula/lexem.h" 

#include <iostream>
#include <vector>
#include <tuple>

namespace lexem
{
  struct ParsedFormula
  { 
    static std::vector<char> Variables;
    static std::vector<unsigned int> InvertedVariables;

    std::vector<Lexem> lexems;
    std::vector<std::tuple<bool, bool, unsigned int, unsigned int, unsigned int>> variablesMark;
    unsigned int nVariables;
    bf::bv64 maxConstant;
    bool hasRationalContext;
    bool endWithSpecial;
    bf::bv64 specialConstant;
    std::string error;

    ParsedFormula()
      :
      variablesMark(52),
      nVariables(0),
      maxConstant(0),
      hasRationalContext(false),
      endWithSpecial(false),
      specialConstant(0)
    {

    }

    bool isOk()
    {
      return error.empty();
    }

    int variableId(const VariableLexem &var)
    {
      auto &info = variablesMark[InvertedVariables[var.symbol]];

      if (!std::get<0>(info))
      {
        return -1;
      }

      if (!var.isNumbered())
      {
        return std::get<1>(info) ? std::get<2>(info) : -1;
      }

      if (var.number < std::get<3>(info) || var.number > std::get<4>(info))
      {
        return -1;
      }

      auto addition = std::get<1>(info) ? 1 : 0;

      return std::get<2>(info) + addition + var.number - std::get<3>(info);
    }

    bool addVariable(const VariableLexem &var)
    {
      int symbolNumber = InvertedVariables[var.symbol];
      auto &info = variablesMark[symbolNumber];

      auto newVariables = 0;

      if (!std::get<0>(info))
      {
        unsigned int max = nVariables;

        for (auto i = symbolNumber + 1; i < variablesMark.size(); ++i)
        {
          if (std::get<0>(variablesMark[i]))
          {
            max = std::get<2>(variablesMark[i]);
            break;
          }
        }

        std::get<0>(info) = true;
        std::get<1>(info) = false;
        std::get<2>(info) = max;
        std::get<3>(info) = 0;
        std::get<4>(info) = 0;
      }
 
      if (!var.isNumbered() && !std::get<1>(info))
      {
        std::get<1>(info) = true;

        newVariables = 1;
      }
      else if (var.isNumbered())
      {
        if (std::get<3>(info) == std::get<4>(info))
        {
          std::get<3>(info) = var.number;
          std::get<4>(info) = var.number + 1;

          newVariables = 1;
        }
        else if (var.number < std::get<3>(info))
        {
          newVariables = std::get<3>(info) - var.number;

          std::get<3>(info) = var.number;
        }
        else if (var.number >= std::get<4>(info))
        {
          newVariables = var.number + 1 - std::get<4>(info);

          std::get<4>(info) = var.number + 1;
        }
      }

      if (newVariables == 0)
      {
        return false;
      }

      for (auto i = symbolNumber + 1; i < variablesMark.size(); ++i)
      {
        if (std::get<0>(variablesMark[i]))
        {
          std::get<2>(variablesMark[i]) += newVariables;
        }
      }

      nVariables += newVariables;

      return true;
    }

    void createError(const Lexem& lexem, const char *msg);
  };

  ParsedFormula ParseFormula(std::istream &stream, const std::vector<std::string> &functions, const std::vector<std::string>& constants, char delimiter = '\0');
}

