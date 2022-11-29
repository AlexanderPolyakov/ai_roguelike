#include "raylib.h"
#include <flecs.h>
#include <algorithm>

#include "ecsTypes.h"
#include "shootEmUp.h"

static void update_camera(Camera2D &cam, flecs::world &ecs)
{
  static auto playerQuery = ecs.query<const Position, const IsPlayer>();

  playerQuery.each([&](const Position &pos, const IsPlayer &)
  {
    cam.target.x += (pos.x - cam.target.x) * 0.1f;
    cam.target.y += (pos.y - cam.target.y) * 0.1f;
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
  init_shoot_em_up(ecs);

  Texture2D bgTex = LoadTexture("assets/background.png"); // TODO: move to ecs

  Camera2D camera = { {0, 0}, {0, 0}, 0.f, 1.f };
  camera.target = Vector2{ 0.f, 0.f };
  camera.offset = Vector2{ width * 0.5f, height * 0.5f };
  camera.rotation = 0.f;
  camera.zoom = 1.f;

  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
  while (!WindowShouldClose())
  {
    process_game(ecs);
    update_camera(camera, ecs);

    BeginDrawing();
      ClearBackground(BLACK);
      BeginMode2D(camera);
        //DrawTextureTiled(bgTex, {0, 0, 512, 512}, {0, 0, 10240, 10240}, {0, 0}, 0.f, 1.f, WHITE);
        constexpr int tiles = 20;
        DrawTextureQuad(bgTex, {tiles, tiles}, {0, 0},
            {-512 * tiles / 2, -512 * tiles / 2, 512 * tiles, 512 * tiles}, GRAY);
        ecs.progress();
      EndMode2D();
      // Advance to next frame. Process submitted rendering primitives.
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
