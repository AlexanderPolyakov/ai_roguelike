#include "goapAction.h"

goap::Action goap::create_action(const char *name, const WorldDesc &desc, float cost)
{
  Action res;
  res.name = name;
  res.cost = cost;
  for (size_t i = 0; i < desc.size(); ++i)
  {
    res.precondition.push_back(-1);
    res.effect.push_back(-1);
    res.setBitset.push_back(true);
  }
  return res;
}

void goap::set_action_precond(Action &act, const WorldDesc &desc, const char *st_name, int8_t val)
{
  auto itf = desc.find(st_name);
  if (itf == desc.end())
    return; // TODO: Assert
  act.precondition[itf->second] = val;
}

void goap::set_action_effect(Action &act, const WorldDesc &desc, const char *st_name, int8_t val)
{
  auto itf = desc.find(st_name);
  if (itf == desc.end())
    return; // TODO: Assert
  act.effect[itf->second] = val;
}

void goap::set_additive_action_effect(Action &act, const WorldDesc &desc, const char *st_name, int8_t val)
{
  auto itf = desc.find(st_name);
  if (itf == desc.end())
    return; // TODO: Assert
  act.effect[itf->second] = val;
  act.setBitset[itf->second] = false;
}

