#pragma once
#include <vector>
#include <unordered_map>
#include <string>

namespace goap
{
  using WorldState = std::vector<int8_t>;
  using WorldDesc = std::unordered_map<std::string, size_t>;
};

