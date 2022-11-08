#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <flecs.h>
#include "ecsTypes.h"

template<typename DataType>
class NamedDataPool
{
public:
  size_t regName(const std::string &name)
  {
    const auto itf = nameIndices.find(name);
    if (itf != nameIndices.end())
      return itf->second;

    size_t idx = data.size();
    nameIndices.emplace(name, idx);
    data.emplace_back(DataType());
    return idx;
  }

  void set(size_t idx, const DataType &in_data)
  {
    data[idx] = in_data;
  }

  DataType get(size_t idx) const
  {
    return data[idx];
  }
private:
  std::unordered_map<std::string, size_t> nameIndices;
  std::vector<DataType> data;
};

class Blackboard : public NamedDataPool<float>,
                   public NamedDataPool<int>,
                   public NamedDataPool<flecs::entity>,
                   public NamedDataPool<Position>
{
public:
  template<typename DataType>
  size_t regName(const std::string &name)
  {
    return NamedDataPool<DataType>::regName(name);
  }

  template<typename DataType>
  void set(size_t idx, const DataType &in_data)
  {
    NamedDataPool<DataType>::set(idx, in_data);
  }

  template<typename DataType>
  DataType get(size_t idx) const
  {
    return NamedDataPool<DataType>::get(idx);
  }

  // not perf optimized
  template<typename DataType>
  DataType get(const char *name)
  {
    size_t idx = regName<DataType>(name);
    return NamedDataPool<DataType>::get(idx);
  }
};

