#include "steering.h"
#include "ecsTypes.h"

struct Seeker {};
struct Pursuer {};
struct Evader {};
struct Fleer {};
struct Separation {};

struct SteerAccel { float accel = 1.f; };

static flecs::entity create_separation(flecs::entity e)
{
  return e.add<Separation>();
}

static flecs::entity create_steerer(flecs::entity e)
{
  return create_separation(e.set(SteerDir{0.f, 0.f}).set(SteerAccel{1.f}));
}

flecs::entity steer::create_seeker(flecs::entity e)
{
  return create_steerer(e).add<Seeker>();
}

flecs::entity steer::create_pursuer(flecs::entity e)
{
  return create_steerer(e).add<Pursuer>();
}

flecs::entity steer::create_evader(flecs::entity e)
{
  return create_steerer(e).add<Evader>();
}

flecs::entity steer::create_fleer(flecs::entity e)
{
  return create_steerer(e).add<Fleer>();
}

typedef flecs::entity (*create_foo)(flecs::entity);

flecs::entity steer::create_steer_beh(flecs::entity e, Type type)
{
  create_foo steerFoo[Type::Num] =
  {
    create_seeker,
    create_pursuer,
    create_evader,
    create_fleer
  };
  return steerFoo[type](e);
}


void steer::register_systems(flecs::world &ecs)
{
  static auto playerPosQuery = ecs.query<const Position, const Velocity, const IsPlayer>();

  ecs.system<Velocity, const MoveSpeed, const SteerDir, const SteerAccel>()
    .each([&](Velocity &vel, const MoveSpeed &ms, const SteerDir &sd, const SteerAccel &sa)
    {
      vel = Velocity{truncate(vel + truncate(sd, ms.speed) * ecs.delta_time() * sa.accel, ms.speed)};
    });

  // reset steer dir
  ecs.system<SteerDir>().each([&](SteerDir &sd) { sd = {0.f, 0.f}; });

  // seeker
  ecs.system<SteerDir, const MoveSpeed, const Velocity, const Position, const Seeker>()
    .each([&](SteerDir &sd, const MoveSpeed &ms, const Velocity &vel,
              const Position &p, const Seeker &)
    {
      playerPosQuery.each([&](const Position &pp, const Velocity &, const IsPlayer &)
      {
        sd += SteerDir{normalize(pp - p) * ms.speed - vel};
      });
    });

  // fleer
  ecs.system<SteerDir, const MoveSpeed, const Velocity, const Position, const Fleer>()
    .each([&](SteerDir &sd, const MoveSpeed &ms, const Velocity &vel, const Position &p, const Fleer &)
    {
      playerPosQuery.each([&](const Position &pp, const Velocity &, const IsPlayer &)
      {
        sd += SteerDir{normalize(p - pp) * ms.speed - vel};
      });
    });

  // pursuer
  ecs.system<SteerDir, const MoveSpeed, const Velocity, const Position, const Pursuer>()
    .each([&](SteerDir &sd, const MoveSpeed &ms, const Velocity &vel, const Position &p, const Pursuer &)
    {
      playerPosQuery.each([&](const Position &pp, const Velocity &pvel, const IsPlayer &)
      {
        constexpr float predictTime = 4.f;
        const Position targetPos = pp + pvel * predictTime;
        sd += SteerDir{normalize(targetPos - p) * ms.speed - vel};
      });
    });

  // evader
  ecs.system<SteerDir, const MoveSpeed, const Velocity, const Position, const Evader>()
    .each([&](SteerDir &sd, const MoveSpeed &ms, const Velocity &vel, const Position &p, const Evader &)
    {
      playerPosQuery.each([&](const Position &pp, const Velocity &pvel,
                              const IsPlayer &)
      {
        constexpr float predictTime = 1.f;
        const Position targetPos = pp + pvel * predictTime;
        sd += SteerDir{normalize(p - targetPos) * ms.speed - vel};
      });
    });

  static auto otherPosQuery = ecs.query<const Position>();

  // separation is expensive!!!
  ecs.system<SteerDir, const Velocity, const MoveSpeed, const Position, const Separation>()
    .each([&](flecs::entity ent, SteerDir &sd, const Velocity &vel, const MoveSpeed &ms,
              const Position &p, const Separation &)
    {
      otherPosQuery.each([&](flecs::entity oe, const Position &op)
      {
        if (oe == ent)
          return;
        constexpr float thresDist = 60.f;
        constexpr float thresDistSq = thresDist * thresDist;
        const float distSq = length_sq(op - p);
        if (distSq > thresDistSq)
          return;
        sd += SteerDir{(p - op) * safeinv(distSq) * ms.speed * thresDist - vel};
      });
    });
}

