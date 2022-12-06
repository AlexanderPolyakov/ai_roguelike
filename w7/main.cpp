#include "raylib.h"
#include <flecs.h>
#include <algorithm>

#include "ecsTypes.h"
#include "shootEmUp.h"
#include "dungeonGen.h"

static void update_camera(flecs::world &ecs)
{
  static auto cameraQuery = ecs.query<Camera2D>();
  static auto playerQuery = ecs.query<const Position, const IsPlayer>();

  cameraQuery.each([&](Camera2D &cam)
  {
    playerQuery.each([&](const Position &pos, const IsPlayer &)
    {
      cam.target.x += (pos.x - cam.target.x) * 0.1f;
      cam.target.y += (pos.y - cam.target.y) * 0.1f;
    });
  });
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

  flecs::world ecs;
  {
    constexpr size_t dungWidth = 50;
    constexpr size_t dungHeight = 50;
    char *tiles = new char[dungWidth * dungHeight];
    gen_drunk_dungeon(tiles, dungWidth, dungHeight);
    init_dungeon(ecs, tiles, dungWidth, dungHeight);
  }
  init_shoot_em_up(ecs);

  Camera2D camera = { {0, 0}, {0, 0}, 0.f, 1.f };
  camera.target = Vector2{ 0.f, 0.f };
  camera.offset = Vector2{ width * 0.5f, height * 0.5f };
  camera.rotation = 0.f;
  camera.zoom = 1.f;
  ecs.entity("camera")
    .set(Camera2D{camera});

  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
  while (!WindowShouldClose())
  {
    static auto cameraQuery = ecs.query<Camera2D>();
    process_game(ecs);
    update_camera(ecs);

    BeginDrawing();
      ClearBackground(BLACK);
      cameraQuery.each([&](Camera2D &cam) { BeginMode2D(cam); });
        ecs.progress();
      EndMode2D();
      // Advance to next frame. Process submitted rendering primitives.
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
