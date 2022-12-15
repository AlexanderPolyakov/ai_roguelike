#include "dungeonGen.h"
#include "dungeonUtils.h"
#include <cstring> // memset
#include <raylib.h>
#include <algorithm>
#include <vector>
#include <random>
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
      int newX = (int(x) + dirs[dir][0] + w) % w;
      int newY = (int(y) + dirs[dir][1] + h) % h;
      x = size_t(newX);
      y = size_t(newY);
    }
  }

  // construct a path from start pos to all other start poses
  for (size_t i = 0; i < startPos.size()-1; ++i)
  {
    const IVec2 &spos = startPos[i];
    float closestDistSq = std::numeric_limits<float>::max();
    IVec2 closestPos = spos;
    for (size_t j = i+1; j < startPos.size(); ++j)
    {
      const IVec2 &epos = startPos[j];
      const float distSq = dist_sq(spos, epos);
      if (distSq < closestDistSq)
      {
        closestDistSq = distSq;
        closestPos = epos;
      }
    }
    IVec2 pos = spos;
    while (dist_sq(pos, closestPos) > 0.f)
    {
      const IVec2 delta = closestPos - pos;
      if (abs(delta.x) > abs(delta.y))
        pos.x += delta.x > 0 ? 1 : -1;
      else
        pos.y += delta.y > 0 ? 1 : -1;
      tiles[size_t(pos.y) * w + size_t(pos.x)] = dungeon::floor;
    }
  }
}

void gen_inv_dungeon(char *tiles, size_t w, size_t h, const size_t max_excavations, const size_t init_sz, const size_t max_steps)
{
  memset(tiles, dungeon::wall, w * h);

  const int dirs[8][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1},
                          {1, 1}, {-1, 1}, {1, -1}, {-1, -1},};

  IVec2 spos{GetRandomValue(init_sz + 1, w - init_sz - 1), GetRandomValue(init_sz + 1, h - init_sz - 1)};
  for (size_t y = spos.y - init_sz; y < spos.y + init_sz; ++y)
    for (size_t x = spos.x - init_sz; x < spos.x + init_sz; ++x)
      tiles[y * w + x] = dungeon::floor;

  size_t numExcavations = 0;
  for (size_t i = 0; i < max_excavations; ++i)
  {
    bool shouldExcavate = false;
    while (!shouldExcavate)
    {
      size_t x = GetRandomValue(1,w-2);
      size_t y = GetRandomValue(1, h-2);
      const size_t dir = GetRandomValue(0, 7);
      for (size_t s = 0; s < max_steps && !shouldExcavate; ++s)
      {
        int newX = std::min(std::max(int(x) + dirs[dir][0], 1), int(w) - 2);
        int newY = std::min(std::max(int(y) + dirs[dir][1], 1), int(h) - 2);
        /*
        if (tiles[newY * w + newX] != dungeon::wall)
          break;
        */
        x = size_t(newX);
        y = size_t(newY);

        // check adjacent cells
        for (size_t yy = std::max(y, size_t(1)) - 1; yy < std::min(h - 2, y) + 2 && !shouldExcavate; ++yy)
          for (size_t xx = std::max(x, size_t(1)) - 1; xx < std::min(w - 2, x) + 2 && !shouldExcavate; ++xx)
            shouldExcavate |= tiles[yy * w + xx] != dungeon::wall;
      }
      if (shouldExcavate)
        tiles[y * w + x] = dungeon::floor;
    }
  }
}

void gen_inv_room_dungeon(char *tiles, size_t w, size_t h, const size_t max_excavations, const size_t init_sz, const size_t max_steps)
{
  memset(tiles, dungeon::wall, w * h);

  const int dirs[8][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1},
                          {1, 1}, {-1, 1}, {1, -1}, {-1, -1},};

  IVec2 spos{GetRandomValue(init_sz + 1, w - init_sz - 1), GetRandomValue(init_sz + 1, h - init_sz - 1)};
  for (size_t y = spos.y - init_sz; y < spos.y + init_sz; ++y)
    for (size_t x = spos.x - init_sz; x < spos.x + init_sz; ++x)
      tiles[y * w + x] = dungeon::floor;

  char rooms[][26] = {
"\
#####\
#   #\
#   #\
#   #\
#####\
",
"\
     \
 ### \
 # # \
 ### \
     \
",
"\
#####\
 ### \
 # # \
 # # \
     \
",
"\
#   #\
## ##\
## ##\
## ##\
#   #\
",
"\
#####\
#####\
     \
#####\
#####\
"
  };
  size_t numExcavations = 0;
  for (size_t i = 0; i < max_excavations; ++i)
  {
    bool shouldExcavate = false;
    while (!shouldExcavate)
    {
      size_t x = GetRandomValue(1,w-2);
      size_t y = GetRandomValue(1, h-2);
      size_t room = GetRandomValue(0, 4);
      const size_t dir = GetRandomValue(0, 7);
      for (size_t s = 0; s < max_steps && !shouldExcavate; ++s)
      {
        int newX = std::min(std::max(int(x) + dirs[dir][0], 1), int(w) - 2);
        int newY = std::min(std::max(int(y) + dirs[dir][1], 1), int(h) - 2);
        /*
        if (tiles[newY * w + newX] != dungeon::wall)
          break;
        */
        x = size_t(newX);
        y = size_t(newY);

        // check adjacent cells
        for (int yy = -2; yy <= 2; ++yy)
          for (int xx = -2; xx <= 2; ++xx)
          {
            if (rooms[room][(yy + 2) * 5 + xx + 2] == dungeon::wall)
              continue;
            if (y + yy < 0 || x + xx < 0 || y + yy >= h || x + xx >= w)
              continue;
            shouldExcavate |= tiles[(y + yy) * w + x + xx] != dungeon::wall;
          }
      }
      if (shouldExcavate)
      {
        for (int yy = -2; yy <= 2; ++yy)
          for (int xx = -2; xx <= 2; ++xx)
          {
            if (y + yy < 0 || x + xx < 0 || y + yy >= h || x + xx >= w)
              continue;
            size_t coord = (y + yy) * w + x + xx;
            if (tiles[coord] == dungeon::wall)
              tiles[coord] = rooms[room][(yy + 2) * 5 + xx + 2];
          }
      }
    }
  }
}


void run_cellular(char *tiles, size_t w, size_t h, const size_t num_iter)
{
  char *scratch = new char[w * h];
  memcpy(scratch, tiles, w * h);
  for (size_t iter = 0; iter < num_iter; ++iter)
  {
    bool hasChanges = false;
    for (int y = 0; y < h; ++y)
      for (int x = 0; x < w; ++x)
      {
        size_t numWalls1 = 0;
        size_t numWalls2 = 0;
        for (int yy = y - 1; yy < y + 2; ++yy)
          for (int xx = x - 1; xx < x + 2; ++xx)
            numWalls1 += yy < 0 || xx < 0 || yy >= h || xx >= w || tiles[yy * w + xx] == dungeon::wall;
        for (int yy = y - 2; yy < y + 3; ++yy)
          for (int xx = x - 2; xx < x + 3; ++xx)
            numWalls2 += yy < 0 || xx < 0 || yy >= h || xx >= w || tiles[yy * w + xx] == dungeon::wall;

        const bool shouldBeWall = numWalls1 >= 5 || numWalls2 < 1;
        const bool shouldFlip = shouldBeWall != (tiles[y * w + x] == dungeon::wall);
        if (shouldFlip)
          scratch[y * w + x] = shouldBeWall ? dungeon::wall : dungeon::floor;
        hasChanges |= shouldFlip;
      }
    memcpy(tiles, scratch, w * h);
    if (!hasChanges)
      break;
  }
  delete[] scratch;
}


void gen_cellular_dungeon(char *tiles, size_t w, size_t h, const float fillrate, const size_t num_iter)
{
  memset(tiles, dungeon::wall, w * h);

  // from https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
  std::random_device rd;  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(0.0, 1.0);
  for (size_t y = 0; y < h; ++y)
    for (size_t x = 0; x < w; ++x)
      tiles[y * w + x] = dis(gen) < fillrate ? dungeon::wall : dungeon::floor;

  run_cellular(tiles, w, h, num_iter);
}

