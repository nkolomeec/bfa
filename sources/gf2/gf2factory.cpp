#pragma once

#include "bv.h"
#include "gf2/gf2factory.h"
#include "gf2/gf2table.h"
#include "gf2/bpolynomials.h"

namespace
{
  constexpr bf::bv64 PolynomialTable[33] = { 0,
    3, 7, 11, 19, 37, 67, 131, 283,
    515, 1033, 2053, 4105, 8219, 16417, 32771, 65579,
    131081, 262153, 524327, 1048585, 2097157, 4194307, 8388641, 16777243,
    33554441, 67108891, 134217767, 268435459, 536870917, 1073741827, BV64(2147483657), BV64(4294967437) 
  };
}

namespace bf
{
  std::shared_ptr<const GF2> GF2Factory::create(bv64 polynomial)
  {
    assert(1 < polynomial && size(polynomial) <= 32);

    auto result = std::make_shared<const GF2>(polynomial);

    return result->check() ? result : nullptr;
  }
  
  std::shared_ptr<const GF2> GF2Factory::createDefault(int n)
  {
    if (n < 1 || n > 32)
    {
      return nullptr;
    }

    return std::make_shared<const GF2>(PolynomialTable[n]);
  }

  std::shared_ptr<const GF2> GF2Factory::createMapped(bv64 polynomial, bool store)
  {
    if (polynomial < 2 || size(polynomial) > 32)
    {
      return nullptr;
    }

    auto elem = _map.find(polynomial);

    if (elem != _map.end())
    {
      auto ptr = elem->second.lock();

      if (ptr != nullptr)
      {
        return ptr;
      }
    }
    else if (!bpolynomials::isIrreducible(polynomial))
    {
      return nullptr;
    }

    auto result = std::shared_ptr<GF2Table>(new GF2Table(polynomial));

    if (store)
    {
      if (elem != _map.end())
      {
        elem->second = result;
      }
      else
      {
        _map[polynomial] = result;
      }
    }
    
    return result;
  }
  
  std::shared_ptr<const GF2> GF2Factory::createMappedDefault(int n)
  {
    if (n < 1 || n > 32)
    {
      return nullptr;
    }

    return createMapped(PolynomialTable[n], true);
  }

  std::map<bv64, std::weak_ptr<const GF2>> GF2Factory::_map{};
}
