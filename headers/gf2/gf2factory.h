#pragma once

#include "bv.h"
#include "gf2/gf2.h"

#include <memory>
#include <map>

namespace bf
{
  class GF2Factory 
  {
  public:
    static std::shared_ptr<const GF2> create(bv64 polynomial);
    static std::shared_ptr<const GF2> createDefault(int n);

    static std::shared_ptr<const GF2> createMapped(bv64 polynomial, bool store = true);
    static std::shared_ptr<const GF2> createMappedDefault(int n);

  private:
    static std::map<bv64, std::weak_ptr<const GF2>> _map;
  };
}
