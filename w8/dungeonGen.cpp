#include "dungeonGen.h"
#include "dungeonUtils.h"
#include <cstring> // memset
#include <raylib.h>
#include <algorithm>
#include <vector>
#include "math.h"
#include <limits>

void gen_drunk_dungeon(char *tiles, size_t w, size_t h,
                       const size_t num_iter, const size_t max_excavations)
{
  memset(tiles, dungeon::wall, w * h);

  const int dirs[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

  std::vector<IVec2> startPos;
  for (size_t iter = 0; iter < num_iter; ++iter)
  {
    // select random point on map
    size_t x = GetRandomValue(1,w-2);
    size_t y = GetRandomValue(1, h-2);
    startPos.push_back({int(x), int(y)});
    size_t numExcavations = 0;
    while (numExcavations < max_excavations)
    {
      if (tiles[y * w + x] == dungeon::wall)
      {
        numExcavations++;
        tiles[y * w + x] = dungeon::floor;
      }
      // choose random dir
      size_t dir = GetRandomValue(0, 3); // 0 - right, 1 - up, 2 - left, 3 - down
      int newX = std::min(std::max(int(x) + dirs[dir][0], 1), int(w) - 2);
      int newY = std::min(std::max(int(y) + dirs[dir][1], 1), int(h) - 2);
      x = size_t(newX);
      y = size_t(newY);
    }
  }

  // construct a path from start pos to all other start poses
  for (const IVec2 &spos : startPos)
    for (const IVec2 &epos : startPos)
    {
      IVec2 pos = spos;
      while (dist_sq(pos, epos) > 0.f)
      {
        const IVec2 delta = epos - pos;
        if (abs(delta.x) > abs(delta.y))
          pos.x += delta.x > 0 ? 1 : -1;
        else
          pos.y += delta.y > 0 ? 1 : -1;
        tiles[size_t(pos.y) * w + size_t(pos.x)] = dungeon::floor;
      }
    }
}

