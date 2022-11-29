#pragma once
#include <flecs.h>

namespace steer
{
  enum Type
  {
    StSeeker = 0,
    StPursuer,
    StEvader,
    StFleer,
    Num
  };

  flecs::entity create_steer_beh(flecs::entity e, Type type);

  flecs::entity create_seeker(flecs::entity e);
  flecs::entity create_pursuer(flecs::entity e);
  flecs::entity create_evader(flecs::entity e);
  flecs::entity create_fleer(flecs::entity e);

  void register_systems(flecs::world &ecs);
};

