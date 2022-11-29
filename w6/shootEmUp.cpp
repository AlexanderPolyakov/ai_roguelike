#include <raylib.h>
#include "shootEmUp.h"
#include "ecsTypes.h"
#include "rlikeObjects.h"
#include "steering.h"

constexpr float tile_size = 64.f;

static void register_roguelike_systems(flecs::world &ecs)
{
  static auto playerPosQuery = ecs.query<const Position, const IsPlayer>();

  ecs.system<Velocity, const MoveSpeed, const IsPlayer>()
    .each([&](Velocity &vel, const MoveSpeed &ms, const IsPlayer)
    {
      bool left = IsKeyDown(KEY_LEFT);
      bool right = IsKeyDown(KEY_RIGHT);
      bool up = IsKeyDown(KEY_UP);
      bool down = IsKeyDown(KEY_DOWN);
      vel.x = ((left ? -1.f : 0.f) + (right ? 1.f : 0.f));
      vel.y = ((up ? -1.f : 0.f) + (down ? 1.f : 0.f));
      vel = Velocity{normalize(vel) * ms.speed};
    });
  ecs.system<Position, const Velocity>()
    .each([&](Position &pos, const Velocity &vel)
    {
      pos += vel * ecs.delta_time();
    });
  ecs.system<const Position, const Color>()
    .term<TextureSource>(flecs::Wildcard)
    .term<BackgroundTile>()
    .each([&](flecs::entity e, const Position &pos, const Color color)
    {
      const auto textureSrc = e.target<TextureSource>();
      DrawTextureQuad(*textureSrc.get<Texture2D>(),
          Vector2{1, 1}, Vector2{0, 0},
          Rectangle{float(pos.x), float(pos.y), tile_size, tile_size}, color);
    });
  ecs.system<const Position, const Color>()
    .term<TextureSource>(flecs::Wildcard)
    .term<BackgroundTile>().not_()
    .each([&](flecs::entity e, const Position &pos, const Color color)
    {
      const auto textureSrc = e.target<TextureSource>();
      DrawTextureQuad(*textureSrc.get<Texture2D>(),
          Vector2{1, 1}, Vector2{0, 0},
          Rectangle{float(pos.x), float(pos.y), tile_size, tile_size}, color);
    });

  ecs.system<Texture2D>()
    .each([&](Texture2D &tex)
    {
      SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    });

  ecs.system<MonsterSpawner>()
    .each([&](MonsterSpawner &ms)
    {
      playerPosQuery.each([&](const Position &pp, const IsPlayer &)
      {
        ms.timeToSpawn -= ecs.delta_time();
        while (ms.timeToSpawn < 0.f)
        {
          steer::Type st = steer::Type(GetRandomValue(0, steer::Type::Num - 1));
          const Color colors[steer::Type::Num] = {WHITE, RED, BLUE, GREEN};
          const float distances[steer::Type::Num] = {800.f, 800.f, 300.f, 300.f};
          const float dist = distances[st];
          constexpr int angRandMax = 1 << 16;
          const float angle = float(GetRandomValue(0, angRandMax)) / float(angRandMax) * PI * 2.f;
          Color col = colors[st];
          steer::create_steer_beh(create_monster(ecs,
              {pp.x + cosf(angle) * dist, pp.y + sinf(angle) * dist}, col, "minotaur_tex"), st);
          ms.timeToSpawn += ms.timeBetweenSpawns;
        }
      });
    });
  steer::register_systems(ecs);
}


void init_shoot_em_up(flecs::world &ecs)
{
  register_roguelike_systems(ecs);

  ecs.entity("swordsman_tex")
    .set(Texture2D{LoadTexture("assets/swordsman.png")});
  ecs.entity("minotaur_tex")
    .set(Texture2D{LoadTexture("assets/minotaur.png")});


  steer::create_seeker(create_monster(ecs, {+400, +400}, WHITE, "minotaur_tex"));
  steer::create_pursuer(create_monster(ecs, {-400, +400}, RED, "minotaur_tex"));
  steer::create_evader(create_monster(ecs, {-400, -400}, BLUE, "minotaur_tex"));
  steer::create_fleer(create_monster(ecs, {+400, -400}, GREEN, "minotaur_tex"));

  create_player(ecs, {0, 0}, "swordsman_tex");

  ecs.entity().set(MonsterSpawner{0.f, 0.1f});
}

void process_game(flecs::world &ecs)
{
}

