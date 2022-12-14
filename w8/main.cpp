#include "raylib.h"
#include <algorithm>

#include "dungeonGen.h"

void draw_map(const char *tiles, size_t w, size_t h)
{
  constexpr size_t tile_size = 6;
  for (size_t y = 0; y < h; ++y)
    for (size_t x = 0; x < w; ++x)
    {
      Color color = tiles[y * w + x] == '#' ? GetColor(0x111111ff) : GetColor(0xaaaaaaff);
      DrawRectangle(y * tile_size, x * tile_size, tile_size, tile_size, color);
    }
}

int main(int /*argc*/, const char ** /*argv*/)
{
  int width = 1920;
  int height = 1080;
  InitWindow(width, height, "w6 AI MIPT");

  const int scrWidth = GetMonitorWidth(0);
  const int scrHeight = GetMonitorHeight(0);
  if (scrWidth < width || scrHeight < height)
  {
    width = std::min(scrWidth, width);
    height = std::min(scrHeight - 150, height);
    SetWindowSize(width, height);
  }

  constexpr size_t dungWidth = 130;
  constexpr size_t dungHeight = 130;
  char *tiles = new char[dungWidth * dungHeight];
  gen_drunk_dungeon(tiles, dungWidth, dungHeight, 1, 1000);

  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
  while (!WindowShouldClose())
  {
    if (IsKeyPressed(KEY_SPACE))
      gen_drunk_dungeon(tiles, dungWidth, dungHeight, 1, 1000);
    BeginDrawing();
      ClearBackground(BLACK);
      draw_map(tiles, dungWidth, dungHeight);
      // Advance to next frame. Process submitted rendering primitives.
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
