#include "dungeonUtils.h"
#include "raylib.h"

Position dungeon::find_walkable_tile(flecs::world &ecs)
{
  static auto dungeonDataQuery = ecs.query<const DungeonData>();

  Position res{0, 0};
  dungeonDataQuery.each([&](const DungeonData &dd)
  {
    // prebuild all walkable and get one of them
    std::vector<Position> posList;
    for (size_t y = 0; y < dd.height; ++y)
      for (size_t x = 0; x < dd.width; ++x)
        if (dd.tiles[y * dd.width + x] == dungeon::floor)
          posList.push_back(Position{int(x), int(y)});
    size_t rndIdx = size_t(GetRandomValue(0, int(posList.size()) - 1));
    res = posList[rndIdx];
  });
  return res;
}

bool dungeon::is_tile_walkable(flecs::world &ecs, Position pos)
{
  static auto dungeonDataQuery = ecs.query<const DungeonData>();

  bool res = false;
  dungeonDataQuery.each([&](const DungeonData &dd)
  {
    if (pos.x < 0 || pos.x >= int(dd.width) ||
        pos.y < 0 || pos.y >= int(dd.height))
      return;
    res = dd.tiles[size_t(pos.y) * dd.width + size_t(pos.x)] == dungeon::floor;
  });
  return res;
}

