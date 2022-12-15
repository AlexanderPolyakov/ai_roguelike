#pragma once
#include <cstddef> // size_t

void gen_drunk_dungeon(char *tiles, size_t w, size_t h,
                       const size_t num_iter, const size_t max_excavations);

void gen_inv_dungeon(char *tiles, size_t w, size_t h, const size_t num_iter, const size_t init_sz, const size_t max_steps);
void gen_inv_room_dungeon(char *tiles, size_t w, size_t h, const size_t num_iter, const size_t init_sz, const size_t max_steps);

void gen_cellular_dungeon(char *tiles, size_t w, size_t h, const float fillrate, const size_t num_iter);
void run_cellular(char *tiles, size_t w, size_t h, const size_t num_iter);
