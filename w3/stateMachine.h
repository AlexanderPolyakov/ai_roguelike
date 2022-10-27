#pragma once
#include <vector>
#include <flecs.h>

class State
{
public:
  virtual ~State() {}
  virtual void enter() const = 0;
  virtual void exit() const = 0;
  virtual void act(float dt, flecs::world &ecs, flecs::entity entity) const = 0;
};

class StateTransition
{
public:
  virtual ~StateTransition() {}
  virtual bool isAvailable(flecs::world &ecs, flecs::entity entity) const = 0;
};

class StateMachine
{
  size_t curStateIdx = 0;
  std::vector<State*> states;
  std::vector<std::vector<std::pair<StateTransition*, int>>> transitions;
public:
  StateMachine() = default;
  StateMachine(const StateMachine &sm) = default;
  StateMachine(StateMachine &&sm) = default;

  ~StateMachine();

  StateMachine &operator=(const StateMachine &sm) = default;
  StateMachine &operator=(StateMachine &&sm) = default;


  void act(float dt, flecs::world &ecs, flecs::entity entity);

  int addState(State *st);
  void addTransition(StateTransition *trans, int from, int to);
};

