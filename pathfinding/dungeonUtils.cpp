#include "dungeonUtils.h"
#include "raylib.h"
#include <vector>

Position dungeon::find_walkable_tile(const char *dungeon, const size_t width, const size_t height)
{
  Position res{0, 0};
  // prebuild all walkable and get one of them
  std::vector<Position> posList;
  for (size_t y = 0; y < height; ++y)
    for (size_t x = 0; x < width; ++x)
      if (dungeon[y * width + x] == dungeon::floor)
        posList.push_back(Position{int(x), int(y)});
  size_t rndIdx = size_t(GetRandomValue(0, int(posList.size()) - 1));
  res = posList[rndIdx];
  return res;
}

