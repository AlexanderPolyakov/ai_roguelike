#include "dungeonGen.h"
#include "dungeonUtils.h"
#include <cstring> // memset
#include <cstdio> // printf
#include <random>
#include <chrono> // std::chrono
#include <functional> // std::bind
#include "math.h"
#include <limits>
#include "raylib.h"

Position gen_random_dir()
{
  constexpr Position dirs[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
  return dirs[GetRandomValue(0, 3)];
}

void gen_drunk_dungeon(char *tiles, const size_t w, const size_t h,
                       const size_t num_iter, const size_t max_excavations)
{
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

  std::vector<Position> startPos;
  for (size_t iter = 0; iter < num_iter; ++iter)
  {
    // select random point on map
    size_t x = rndWd();
    size_t y = rndHt();
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
      size_t dir = rndDir(); // 0 - right, 1 - up, 2 - left, 3 - down
      int newX = std::min(std::max(int(x) + dirs[dir][0], 1), int(w) - 2);
      int newY = std::min(std::max(int(y) + dirs[dir][1], 1), int(h) - 2);
      x = size_t(newX);
      y = size_t(newY);
    }
  }

  // construct a path from start pos to all other start poses
  for (size_t i = 0; i < startPos.size()-1; ++i)
  {
    const Position &spos = startPos[i];
    float closestDistSq = std::numeric_limits<float>::max();
    Position closestPos = spos;
    for (size_t j = i+1; j < startPos.size(); ++j)
    {
      const Position &epos = startPos[j];
      const float distSq = dist_sq(spos, epos);
      if (distSq < closestDistSq)
      {
        closestDistSq = distSq;
        closestPos = epos;
      }
    }
    Position pos = spos;
    while (dist_sq(pos, closestPos) > 0.f)
    {
      const Position delta = closestPos - pos;
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

void spill_drunk_water(char *tiles, const size_t w, const size_t h,
                       const size_t num_iter, const size_t max_spills)
{
  for (size_t iter = 0; iter < num_iter; ++iter)
  {
    Position p = dungeon::find_walkable_tile(tiles, w, h);
    // select random point on map
    size_t x = size_t(p.x);
    size_t y = size_t(p.y);
    size_t numSpills = 0;
    while (numSpills < max_spills)
    {
      if (tiles[y * w + x] == dungeon::floor)
      {
        numSpills++;
        tiles[y * w + x] = dungeon::water;
      }
      // choose random dir
      bool validDir = false;
      while (!validDir)
      {
        const Position dir = gen_random_dir(); // 0 - right, 1 - up, 2 - left, 3 - down
        int newX = std::min(std::max(int(x) + dir.x, 1), int(w) - 2);
        int newY = std::min(std::max(int(y) + dir.y, 1), int(h) - 2);
        if (tiles[size_t(newY) * w + size_t(newX)] != dungeon::wall)
        {
          x = size_t(newX);
          y = size_t(newY);
          validDir = true;
        }
      }
    }
  }
}

