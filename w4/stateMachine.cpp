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
  if (curStateIdx < states.size())
  {
    for (const std::pair<StateTransition*, int> &transition : transitions[curStateIdx])
      if (transition.first->isAvailable(ecs, entity))
      {
        states[curStateIdx]->exit();
        curStateIdx = size_t(transition.second);
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
  size_t idx = states.size();
  states.push_back(st);
  transitions.push_back(std::vector<std::pair<StateTransition*, int>>());
  return int(idx);
}

void StateMachine::addTransition(StateTransition *trans, int from, int to)
{
  transitions[size_t(from)].push_back(std::make_pair(trans, to));
}

