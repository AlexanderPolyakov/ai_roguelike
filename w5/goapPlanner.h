#pragma once
#include <unordered_map>
#include <vector>
#include <string>

#include "goapWorldState.h"
#include "goapAction.h"

namespace goap
{

  struct Planner
  {
    WorldDesc wdesc;
    std::vector<Action> actions;
    std::unordered_map<std::string, size_t> actionNames;
  };

  Planner create_planner();

  using StateDesc = std::pair<const char*, int>;
  using Precond = std::vector<StateDesc>;
  using Effect = std::vector<StateDesc>;
  using WorldStateList = std::vector<StateDesc>;

  void add_action_to_planner(Planner &planner, const char *name, float cost, const Precond &precond,
                                                                             const Effect &effect,
                                                                             const Effect &additive_effect);

  void add_states_to_planner(Planner &planner, const std::vector<std::string> &state_names);
  WorldState produce_planner_worldstate(const Planner &planner, const WorldStateList &states);

  float get_action_cost(const Planner &planner, size_t act_id);

  std::vector<size_t> find_valid_state_transitions(const Planner &planner, const WorldState &from);
  WorldState apply_action(const Planner &planner, size_t act, const WorldState &from);

  struct PlanStep
  {
    size_t action;
    WorldState worldState;
  };

  float make_plan(const Planner &planner, const WorldState &from, const WorldState &to, std::vector<PlanStep> &plan);
  void print_plan(const Planner &planner, const WorldState &init, const std::vector<PlanStep> &plan);
};

