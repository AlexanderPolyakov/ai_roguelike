#pragma once
#include <cstddef> // size_t

void gen_drunk_dungeon(char *tiles, size_t w, size_t h,
                       const size_t num_iter, const size_t max_excavations);
