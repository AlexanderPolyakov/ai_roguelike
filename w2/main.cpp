// initial skeleton is a clone from https://github.com/jpcy/bgfx-minimal-example
//
#include "raylib.h"
#include <flecs.h>
#include "ecsTypes.h"
#include "roguelike.h"

static void update_camera(Camera2D &cam, flecs::world &ecs)
{
  static auto playerQuery = ecs.query<const Position, const IsPlayer>();

  playerQuery.each([&](const Position &pos, const IsPlayer &)
  {
    cam.target.x = pos.x;
    cam.target.y = pos.y;
  });
}

int main(int argc, const char **argv)
{
  int width = 1920;
  int height = 1080;
  InitWindow(width, height, "w2 AI MIPT");

  flecs::world ecs;

  init_roguelike(ecs);

  Camera2D camera = { 0 };
  camera.target = Vector2{ 0.f, 0.f };
  camera.offset = Vector2{ width * 0.5f, height * 0.5f };
  camera.rotation = 0.f;
  camera.zoom = 64.f;

  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
  while (!WindowShouldClose())
  {
    process_turn(ecs);
    update_camera(camera, ecs);

    BeginDrawing();
      ClearBackground(GetColor(0x052c46ff));
      BeginMode2D(camera);
        ecs.progress();
      EndMode2D();
      print_stats(ecs);
      // Advance to next frame. Process submitted rendering primitives.
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
