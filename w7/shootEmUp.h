#pragma once
#include <flecs.h>

void init_shoot_em_up(flecs::world &ecs);
void process_game(flecs::world &ecs);
void init_dungeon(flecs::world &ecs, char *tiles, size_t w, size_t h);

