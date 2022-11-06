#include "dungeonGen.h"
#include "dungeonUtils.h"
#include <cstring> // memset
#include <cstdio> // printf
#include <random>
#include <chrono> // std::chrono
#include <functional> // std::bind
#include "ecsTypes.h"
#include "math.h"
#include <limits>


void gen_drunk_dungeon(char *tiles, size_t w, size_t h)
{
  //constexpr char wall = '#';
  //constexpr char flr = ' ';

  memset(tiles, dungeon::wall, w * h);

  // generator
  unsigned seed = unsigned(std::chrono::system_clock::now().time_since_epoch().count() % std::numeric_limits<int>::max());
  std::default_random_engine seedGenerator(seed);
  std::default_random_engine widthGenerator(seedGenerator());
  std::default_random_engine heightGenerator(seedGenerator());
  std::default_random_engine dirGenerator(seedGenerator());

  // distributions
  std::uniform_int_distribution<size_t> widthDist(1, w-2);
  std::uniform_int_distribution<size_t> heightDist(1, h-2);
  std::uniform_int_distribution<size_t> dirDist(0, 3);
  auto rndWd = std::bind(widthDist, widthGenerator);
  auto rndHt = std::bind(heightDist, heightGenerator);
  auto rndDir = std::bind(dirDist, dirGenerator);

  const int dirs[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

  constexpr size_t numIter = 4;
  constexpr size_t maxExcavations = 200;
  std::vector<Position> startPos;
  for (size_t iter = 0; iter < numIter; ++iter)
  {
    // select random point on map
    size_t x = rndWd();
    size_t y = rndHt();
    startPos.push_back({int(x), int(y)});
    size_t numExcavations = 0;
    while (numExcavations < maxExcavations)
    {
      if (tiles[y * w + x] == dungeon::wall)
      {
        numExcavations++;
        tiles[y * w + x] = dungeon::floor;
      }
      // choose random dir
      size_t dir = rndDir(); // 0 - right, 1 - up, 2 - left, 3 - down
      int newX = std::min(std::max(int(x) + dirs[dir][0], 1), int(w) - 2);
      int newY = std::min(std::max(int(y) + dirs[dir][1], 1), int(h) - 2);
      x = size_t(newX);
      y = size_t(newY);
    }
  }

  // construct a path from start pos to all other start poses
  for (const Position &spos : startPos)
    for (const Position &epos : startPos)
    {
      Position pos = spos;
      while (dist_sq(pos, epos) > 0.f)
      {
        const Position delta = epos - pos;
        if (abs(delta.x) > abs(delta.y))
          pos.x += delta.x > 0 ? 1 : -1;
        else
          pos.y += delta.y > 0 ? 1 : -1;
        tiles[size_t(pos.y) * w + size_t(pos.x)] = dungeon::floor;
      }
    }

  for (size_t y = 0; y < h; ++y)
    printf("%.*s\n", int(w), tiles + y * w);
}

