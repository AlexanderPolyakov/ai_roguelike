#pragma once
#include <flecs.h>
#include "raylib.h"
#include "ecsTypes.h"

flecs::entity create_monster(flecs::world &ecs, Position pos, Color col, const char *texture_src);
void create_player(flecs::world &ecs, Position pos, const char *texture_src);

struct MonsterSpawner
{
  float timeToSpawn;
  float timeBetweenSpawns;
};

