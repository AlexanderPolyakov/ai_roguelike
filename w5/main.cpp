//
#include "raylib.h"
#include <flecs.h>
#include <algorithm>
#include "ecsTypes.h"
#include "roguelike.h"
#include "dungeonGen.h"
#include "goapPlanner.h"

enum EnemyDist
{
  DistMelee = 0,
  DistRanged,
  DistFar
};

enum HealthState
{
  Dead = 0,
  Injured,
  Healthy
};

static void debug_enemy_planner()
{
  goap::Planner pl = goap::create_planner();

  goap::add_states_to_planner(pl,
      {"enemy_vis",
       "enemy_alive",
       "have_melee",
       "have_ranged",
       "enemy_dist",
       "health_state"});

  goap::add_action_to_planner(pl, "wander", 1,
      {{"health_state", Healthy}},
      {{"enemy_vis", 1}},
      {});

  goap::add_action_to_planner(pl, "approach_enemy", 1,
      {{"health_state", Healthy}},
      {},
      {{"enemy_dist", -1}});

  goap::add_action_to_planner(pl, "flee_enemy", 1,
      {{"health_state", Healthy}},
      {},
      {{"enemy_dist", +1}});

  goap::add_action_to_planner(pl, "find_melee", 1,
      {{"have_melee", 0}, {"health_state", Healthy}},
      {{"have_melee", 1}},
      {});

  goap::add_action_to_planner(pl, "find_ranged", 1,
      {{"have_ranged", 0}, {"health_state", Healthy}},
      {{"have_ranged", 1}},
      {});

  goap::add_action_to_planner(pl, "patch_up", 1,
      {{"health_state", Injured}},
      {},
      {{"health_state", +1}});

  goap::add_action_to_planner(pl, "attack_enemy", 1,
      {{"enemy_vis", 1}, {"enemy_alive", 1}, {"have_melee", 1}, {"enemy_dist", DistMelee}, {"health_state", Healthy}},
      {{"enemy_alive", 0}},
      {{"health_state", -1}});

  goap::add_action_to_planner(pl, "shoot_enemy", 1,
      {{"enemy_vis", 1}, {"enemy_alive", 1}, {"have_ranged", 1}, {"enemy_dist", DistRanged}, {"health_state", Healthy}},
      {{"enemy_alive", 0}},
      {});

  goap::WorldState ws = goap::produce_planner_worldstate(pl,
      {{"enemy_vis", 0},
       {"enemy_alive", 1},
       {"have_melee", 0},
       {"have_ranged", 0},
       {"enemy_dist", DistFar},
       {"health_state", Healthy}});

  goap::WorldState goal = goap::produce_planner_worldstate(pl,
      {{"enemy_alive", 0}, {"health_state", Healthy}});

  std::vector<goap::PlanStep> plan;
  goap::make_plan(pl, ws, goal, plan);
  goap::print_plan(pl, ws, plan);
}

static void debug_looter_planner()
{
  goap::Planner pl = goap::create_planner();

  goap::add_states_to_planner(pl,
      {"enemy_vis",
       "loot_vis",
       "num_loot",
       "have_melee",
       "have_ranged",
       "enemy_dist",
       "health_state",
       "escaped"});

  goap::add_action_to_planner(pl, "open_room", 1,
      {{"health_state", Healthy}},
      {{"enemy_vis", 1}, {"loot_vis", 1}/*, {"enemy_dist", 2}*/},
      {});

  goap::add_action_to_planner(pl, "loot", 1,
      {{"health_state", Healthy}, {"loot_vis", 1}, {"enemy_vis", 0}},
      {{"loot_vis", 0}},
      {{"num_loot", +1}});

  goap::add_action_to_planner(pl, "approach_enemy", 1,
      {{"health_state", Healthy}},
      {},
      {{"enemy_dist", -1}});

  goap::add_action_to_planner(pl, "flee_enemy", 1,
      {{"health_state", Healthy}},
      {},
      {{"enemy_dist", +1}});

  goap::add_action_to_planner(pl, "find_melee", 1,
      {{"have_melee", 0}, {"health_state", Healthy}},
      {{"have_melee", 1}},
      {});

  goap::add_action_to_planner(pl, "find_ranged", 1,
      {{"have_ranged", 0}, {"health_state", Healthy}},
      {{"have_ranged", 1}},
      {});

  goap::add_action_to_planner(pl, "patch_up", 1,
      {{"health_state", Injured}},
      {},
      {{"health_state", +1}});

  goap::add_action_to_planner(pl, "attack_enemy", 1,
      {{"enemy_vis", 1}, {"have_melee", 1}, {"enemy_dist", DistMelee}, {"health_state", Healthy}},
      {{"enemy_vis", 0}},
      {{"health_state", -1}});

  goap::add_action_to_planner(pl, "shoot_enemy", 1,
      {{"enemy_vis", 1}, {"have_ranged", 1}, {"enemy_dist", DistRanged}, {"health_state", Healthy}},
      {{"enemy_vis", 0}},
      {{"health_state", -1}});

  goap::add_action_to_planner(pl, "escape", 1,
      {{"health_state", Healthy}, {"num_loot", 5}},
      {{"escaped", 1}},
      {});

  goap::WorldState ws = goap::produce_planner_worldstate(pl,
      {{"enemy_vis", 0},
       {"loot_vis", 0},
       {"num_loot", 0},
       {"have_melee", 1},
       {"have_ranged", 0},
       {"enemy_dist", DistFar},
       {"health_state", Healthy},
       {"escaped", 0}});

  goap::WorldState goal = goap::produce_planner_worldstate(pl,
      {{"num_loot", 5}, {"escaped", 1}, {"health_state", Healthy}});

  std::vector<goap::PlanStep> plan;
  goap::make_plan(pl, ws, goal, plan);
  goap::print_plan(pl, ws, plan);
}


static void update_camera(Camera2D &cam, flecs::world &ecs)
{
  static auto playerQuery = ecs.query<const Position, const IsPlayer>();

  playerQuery.each([&](const Position &pos, const IsPlayer &)
  {
    cam.target.x += (pos.x * tile_size - cam.target.x) * 0.1f;
    cam.target.y += (pos.y * tile_size - cam.target.y) * 0.1f;
  });
}

int main(int /*argc*/, const char ** /*argv*/)
{
  int width = 1920;
  int height = 1080;
  InitWindow(width, height, "w3 AI MIPT");

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
  init_roguelike(ecs);
  debug_enemy_planner();
  debug_looter_planner();

  Camera2D camera = { {0, 0}, {0, 0}, 0.f, 1.f };
  camera.target = Vector2{ 0.f, 0.f };
  camera.offset = Vector2{ width * 0.5f, height * 0.5f };
  camera.rotation = 0.f;
  camera.zoom = 0.125f;

  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
  while (!WindowShouldClose())
  {
    process_turn(ecs);
    update_camera(camera, ecs);

    BeginDrawing();
      ClearBackground(BLACK);
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
