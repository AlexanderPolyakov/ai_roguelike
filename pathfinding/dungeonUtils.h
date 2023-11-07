#pragma once
#include "math.h"
#include <cstddef>

namespace dungeon
{
  constexpr char wall =  '#';
  constexpr char floor = ' ';
  constexpr char water = 'o';

  Position find_walkable_tile(const char *dungeon, const size_t width, const size_t height);
}
