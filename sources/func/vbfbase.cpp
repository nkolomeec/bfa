#include "func/vbfbase.h"

namespace bf 
{
  std::vector<BFBase> VBFBase::split() const
  {
    std::vector<BFBase> result(m());

    for (auto i = 0; i < m(); ++i)
    {
      result[i] = BFBase(n());
    }

    for (auto x : dom())
    {
      auto val = get(x);

      for (auto i = 0; i < m(); ++i)
      {
        result[i].set(x, bf::get(val, i));
      }
    }

    return result;
  }
  
  VBFBase VBFBase::join(const std::vector<BFBase> &coordinates)
  {
    bv32 m = (bv32)coordinates.size();

    assert(m > 0);

    auto n = coordinates[0].n();

    for (const auto &f : coordinates)
    {
      assert(f.n() == n);
    }

    VBFBase result(n, m);
    
    for (auto x : coordinates[0].dom())
    {
      bv32 val = 0;

      for (bv8 i = 0; i < m; ++i)
      {
        bf::set(val, i, coordinates[i].get(x));
      }

      result.set(x, val);
    }

    return result;
  }

  VBFBase VBFBase::MobiusTransform() const
  {
    auto splitted = split();

    std::vector<BFBase> transforms;

    for (const auto &coordinate : splitted)
    {
      transforms.push_back(coordinate.MobiusTransform());
    }
    
    return join(transforms);
  }
}
