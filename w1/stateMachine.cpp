#include "stateMachine.h"

StateMachine::~StateMachine()
{
  for (State* state : states)
    delete state;
  states.clear();
  for (auto &transList : transitions)
    for (auto &transition : transList)
      delete transition.first;
  transitions.clear();
}

void StateMachine::act(float dt, flecs::world &ecs, flecs::entity entity)
{
  if (curStateIdx < int(states.size()))
  {
    for (const auto& [transition, index] : transitions[curStateIdx])
      if (transition->isAvailable(ecs, entity))
      {
        states[curStateIdx]->exit();
        curStateIdx = index;
        states[curStateIdx]->enter();
        break;
      }
    states[curStateIdx]->act(dt, ecs, entity);
  }
  else
    curStateIdx = 0;
}

int StateMachine::addState(State *st)
{
  int idx = int(states.size());
  states.push_back(st);
  transitions.emplace_back();
  return idx;
}

void StateMachine::addTransition(StateTransition *trans, int from, int to)
{
  transitions[from].push_back(std::pair(trans, to));
}

