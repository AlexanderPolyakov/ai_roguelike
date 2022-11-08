#pragma once
#include "goapWorldState.h"
#include <string>

namespace goap
{

  struct Action
  {
    std::string name = "";

    WorldState precondition;
    WorldState effect;

    std::vector<bool> setBitset; // if effect sets world state, or is it additive (true - sets, false - additive)

    float cost = 1.f;
  };

  Action create_action(const char *name, const WorldDesc &desc, float cost);
  void set_action_precond(Action &act, const WorldDesc &desc, const char *st_name, int8_t val);
  void set_action_effect(Action &act, const WorldDesc &desc, const char *st_name, int8_t val);
  void set_additive_action_effect(Action &act, const WorldDesc &desc, const char *st_name, int8_t val);
};

