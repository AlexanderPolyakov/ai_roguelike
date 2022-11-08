#include "goapPlanner.h"

goap::Planner goap::create_planner()
{
  return Planner();
}

void goap::add_states_to_planner(Planner &planner, const std::vector<std::string> &state_names)
{
  for (const std::string &name : state_names)
    planner.wdesc.emplace(name, planner.wdesc.size());
}


void goap::add_action_to_planner(Planner &planner, const char *name, float cost, const Precond &precond,
                                                                                 const Effect &effect,
                                                                                 const Effect &additive_effect)
{
  Action act = create_action(name, planner.wdesc, cost);
  for (auto st : precond)
    set_action_precond(act, planner.wdesc, st.first, int8_t(st.second));
  for (auto st : effect)
    set_action_effect(act, planner.wdesc, st.first, int8_t(st.second));
  for (auto st : additive_effect)
    set_additive_action_effect(act, planner.wdesc, st.first, int8_t(st.second));

  planner.actionNames.emplace(name, planner.actions.size());
  planner.actions.emplace_back(act);
}

static void set_planner_worldstate(const goap::Planner &planner, goap::WorldState &st, const char *st_name, int8_t val)
{
  auto itf = planner.wdesc.find(st_name);
  if (itf == planner.wdesc.end())
    return;
  st[itf->second] = val;
}

goap::WorldState goap::produce_planner_worldstate(const Planner &planner, const WorldStateList &states)
{
  WorldState res;
  for (size_t i = 0; i < planner.wdesc.size(); ++i)
    res.emplace_back(int8_t(-1));
  for (auto st : states)
    set_planner_worldstate(planner, res, st.first, int8_t(st.second));
  return res;
}

float goap::get_action_cost(const Planner &planner, size_t act_id)
{
  return planner.actions[act_id].cost;
}

std::vector<size_t> goap::find_valid_state_transitions(const Planner &planner, const WorldState &from)
{
  std::vector<size_t> res;

  for (size_t i = 0; i < planner.actions.size(); ++i)
  {
    const Action &action = planner.actions[i];
    bool isValidAction = true;
    for (size_t j = 0; j < action.precondition.size() && isValidAction; ++j)
      isValidAction &= action.precondition[j] < 0 || from[j] == action.precondition[j];
    if (isValidAction)
      res.emplace_back(i);
  }
  return res;
}

goap::WorldState goap::apply_action(const Planner &planner, size_t act, const WorldState &from)
{
  WorldState res = from;
  const Action &action = planner.actions[act];
  for (size_t i = 0; i < action.effect.size(); ++i)
  {
    if (!action.setBitset[i])
      res[i] += action.effect[i];
    else if (action.effect[i] >= 0)
      res[i] = action.effect[i];
  }
  return res;
}

