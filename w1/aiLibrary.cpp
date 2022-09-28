#include "aiLibrary.h"
#include "flecs_inc.h"
#include "ecsTypes.h"
#include <bx/rng.h>
#include <cfloat>
#include <cmath>
#include <iostream>


static bx::RngShr3 rng;

class StateMachineState : public State
{
    std::unique_ptr<StateMachine> m_state_machine;
public:
    StateMachineState(std::unique_ptr<StateMachine>&& sm): m_state_machine(std::move(sm)) {}
    void enter() const override {} 
    void exit() const override {} 
    void act(float/* dt*/, flecs::world & ecs, flecs::entity entity) const override {
        m_state_machine->act(0.f, ecs, entity); // needed as m_state_machine is not in ecs
    }
};

class AttackEnemyState : public State
{
public:
  virtual ~AttackEnemyState() = default;
  void enter() const override {}
  void exit() const override {}
  void act(float/* dt*/, flecs::world &/*ecs*/, flecs::entity /*entity*/) const override {}
};

template<typename T>
T sqr(T a){ return a*a; }

template<typename T, typename U>
static float dist_sq(const T &lhs, const U &rhs) { return float(sqr(lhs.x - rhs.x) + sqr(lhs.y - rhs.y)); }

template<typename T, typename U>
static float dist(const T &lhs, const U &rhs) { return sqrtf(dist_sq(lhs, rhs)); }

template<typename T, typename U>
static int move_towards(const T &from, const U &to)
{
  int deltaX = to.x - from.x;
  int deltaY = to.y - from.y;
  if (abs(deltaX) > abs(deltaY))
    return deltaX > 0 ? EA_MOVE_RIGHT : EA_MOVE_LEFT;
  return deltaY > 0 ? EA_MOVE_UP : EA_MOVE_DOWN;
}

static int inverse_move(int move)
{
  return move == EA_MOVE_LEFT ? EA_MOVE_RIGHT :
         move == EA_MOVE_RIGHT ? EA_MOVE_LEFT :
         move == EA_MOVE_UP ? EA_MOVE_DOWN :
         move == EA_MOVE_DOWN ? EA_MOVE_UP : move;
}


template<typename Callable>
static void on_closest_enemy_pos(flecs::world &ecs, flecs::entity entity, Callable c)
{
  static auto enemiesQuery = ecs.query<const Position, const Team>();
  entity.set([&](const Position &pos, const Team &t, Action &a)
  {
    flecs::entity closestEnemy;
    float closestDist = FLT_MAX;
    Position closestPos;
    enemiesQuery.each([&](flecs::entity enemy, const Position &epos, const Team &et)
    {
      if (t.team == et.team)
        return;
      float curDist = dist(epos, pos);
      if (curDist < closestDist)
      {
        closestDist = curDist;
        closestPos = epos;
        closestEnemy = enemy;
      }
    });
    if (ecs.is_valid(closestEnemy))
      c(a, pos, closestPos);
  });
}

class SleepState : public State
{
public:
  void enter() const override { std::cout << "Sleeping...\n"; }
  void exit() const override {}
  void act(float/* dt*/, flecs::world &/*ecs*/, flecs::entity entity) const override {
    entity.set([&](SleepinessLevel& sleep, Action& a){
        sleep.value += 15.0f;
        a.action = EA_SLEEP;
        std::cout << "Sleeping...\n";
    });
  }
};
class SearchHomeState : public State
{
public:
  void enter() const override {}
  void exit() const override {std::cout << "no longer searching home\n";}
  void act(float/* dt*/, flecs::world &/*ecs*/, flecs::entity entity) const override {
    entity.set([&](Position& pos, HomePosition& hpos, Action& a){
      a.action = move_towards(pos, hpos);
      std::cout << "Searching home...\n";
    });
  }
};
class Eat : public State
{
public:
  void enter() const override {
    std::cout << "Eating!\n";
  }
  void exit() const override {}
  void act(float/* dt*/, flecs::world &/*ecs*/, flecs::entity entity) const override {
    entity.set([&](HungerLevel& hunger, Action& a){
        hunger.value += 15.0f;
        a.action = EA_EAT;
        std::cout << "Saturation: " << hunger.value << '\n';
    });
  }
};

class SearchFood : public State
{
public:
  void enter() const override {}
  void exit() const override {std::cout << "no longer searching food\n";}
  void act(float/* dt*/, flecs::world &/*ecs*/, flecs::entity entity) const override {
    entity.set([&](Position& pos, FoodSourceTarget& hpos, Action& a){
      a.action = move_towards(pos, hpos);
      std::cout << "Searching food: " << pos.x << ' ' << pos.y 
        << " to " << hpos.x << ' ' << hpos.y 
        << " action: " << a.action << '\n';
    });
  }
};

class HealState : public State
{
public:
  virtual ~HealState() = default;
  void enter() const override {}
  void exit() const override {}
  void act(float/* dt*/, flecs::world&, flecs::entity entity) const override
  {
    entity.set([&](const TickCount& count, Hitpoints& hitpoints, HealCooldown& cooldown, Color& color, Action& a)
    {
        a.action = EA_NOP;
        color.color = 0xffff0000 + int((hitpoints.hitpoints / 100.0f) * 255.0f);
        if (cooldown.next <= count.count) 
        {
            a.action = EA_HEAL;
        }
    });
  }
};

class MoveToEnemyState : public State
{
public:
  virtual ~MoveToEnemyState() = default;
  void enter() const override {}
  void exit() const override {}
  void act(float/* dt*/, flecs::world &ecs, flecs::entity entity) const override
  {
    on_closest_enemy_pos(ecs, entity, [&](Action &a, const Position &pos, const Position &enemy_pos)
    {
      a.action = move_towards(pos, enemy_pos);
    });
  }
};

class FleeFromEnemyState : public State
{
public:
  virtual ~FleeFromEnemyState() = default;
  FleeFromEnemyState() {}
  void enter() const override {}
  void exit() const override {}
  void act(float/* dt*/, flecs::world &ecs, flecs::entity entity) const override
  {
    on_closest_enemy_pos(ecs, entity, [&](Action &a, const Position &pos, const Position &enemy_pos)
    {
      a.action = inverse_move(move_towards(pos, enemy_pos));
    });
  }
};

class PatrolState : public State
{
  float patrolDist;
public:
  virtual ~PatrolState() = default;
  PatrolState(float dist) : patrolDist(dist) {}
  void enter() const override {}
  void exit() const override {}
  void act(float/* dt*/, flecs::world &, flecs::entity entity) const override
  {
    entity.set([&](const Position &pos, const PatrolPos &ppos, Action &a)
    {
      if (dist(pos, ppos) > patrolDist)
        a.action = move_towards(pos, ppos); // do a recovery walk
      else
      {
        // do a random walk
        a.action = EA_MOVE_START + (rng.gen() % (EA_MOVE_END - EA_MOVE_START));
      }
    });
  }
};

class FollowState : public State
{
  float followDist;
public:
  virtual ~FollowState() = default;
  FollowState(float dist) : followDist(dist) {}
  void enter() const override {}
  void exit() const override {}
  void act(float/* dt*/, flecs::world &, flecs::entity entity) const override
  {
    entity.set([&](const Position &pos, const FollowTarget &target, Action &a)
    {
      auto fpos = *target.target.get<Position>();
      if (dist(pos, fpos) > followDist)
        a.action = move_towards(pos, fpos); // do a recovery walk
      else
      {
        // do a random walk
        a.action = EA_MOVE_START + (rng.gen() % (EA_MOVE_END - EA_MOVE_START));
      }
    });
  }
};

class NopState : public State
{
public:
  virtual ~NopState() = default;
  void enter() const override {}
  void exit() const override {}
  void act(float/* dt*/, flecs::world &, flecs::entity) const override {}
};

template <typename Component>
class CloseToPointTransition : public StateTransition
{
    float triggerDist;
public:
    CloseToPointTransition(float dist) : triggerDist(dist) {}
    bool isAvailable(flecs::world&, flecs::entity entity) const override
    {
        bool reached = false;
        entity.get([&](const Component &fpos, const Position& pos)
        {
            auto dx = pos.x - fpos.x;
            auto dy = pos.y - fpos.y;
            reached = triggerDist * triggerDist > dx*dx + dy*dy;
        });
        return reached;
    }
};

class FoodCloseTransition : public CloseToPointTransition<FoodSourceTarget> {
    using CloseToPointTransition::CloseToPointTransition;
};
class HomeCloseTransition : public CloseToPointTransition<HomePosition> {
    using CloseToPointTransition::CloseToPointTransition;
};

template<typename Component>
class CompareThreshold : public StateTransition {
protected:
    float threshold;
public:
    CompareThreshold(float threshold) : threshold(threshold) {}
    bool isAvailable(flecs::world&, flecs::entity entity) const override {
        bool result = false;
        entity.get([&](const Component& cmp){
            result = cmp.value < threshold;
        });
        return result;
    }
};

class HungryTransition : public CompareThreshold<HungerLevel> {
    using CompareThreshold::CompareThreshold;
    bool isAvailable(flecs::world& w, flecs::entity entity) const override {
        auto res = this->CompareThreshold::isAvailable(w, entity);
        std::cout << "Hungry transition: " << threshold << " res: " << res << '\n';
        return res;
    }
};
class SleepyTransition : public CompareThreshold<SleepinessLevel> {
    using CompareThreshold::CompareThreshold;
    bool isAvailable(flecs::world& w, flecs::entity entity) const override {
        auto res = this->CompareThreshold::isAvailable(w, entity);
        std::cout << "Sleepy transition: " << threshold << " res: " << res << '\n';
        return res;
    }
};

class EnemyAvailableTransition : public StateTransition
{
  float triggerDist;
public:
  EnemyAvailableTransition(float in_dist) : triggerDist(in_dist) {}
  bool isAvailable(flecs::world &ecs, flecs::entity entity) const override
  {
    static auto enemiesQuery = ecs.query<const Position, const Team>();
    bool enemiesFound = false;
    entity.get([&](const Position &pos, const Team &t)
    {
      enemiesQuery.each([&](flecs::entity, const Position &epos, const Team &et)
      {
        if (t.team == et.team)
          return;
        float curDist = dist(epos, pos);
        enemiesFound |= curDist <= triggerDist;
      });
    });
    return enemiesFound;
  }
};

class HitpointsLessThanTransition : public StateTransition
{
  float threshold;
public:
  HitpointsLessThanTransition(float in_thres) : threshold(in_thres) {}
  bool isAvailable(flecs::world &, flecs::entity entity) const override
  {
    bool hitpointsThresholdReached = false;
    entity.get([&](const Hitpoints &hp)
    {
      hitpointsThresholdReached |= hp.hitpoints < threshold;
    });
    return hitpointsThresholdReached;
  }
};

class FolloweeCloseTransition : public StateTransition
{
    float threshold;
public:
    FolloweeCloseTransition(float in_thres) : threshold(in_thres) {}
    bool isAvailable(flecs::world &, flecs::entity entity) const override
    {
    bool reached = false;
    entity.get([&](const FollowTarget &target, const Position& pos)
    {
        target.target.get([&](const Position &fpos)
        {
            auto dx = pos.x - fpos.x;
            auto dy = pos.y - fpos.y;
            reached = threshold * threshold > dx*dx + dy*dy;
        });
    });
    return reached;
    }
};

class FolloweeHitpointsLessThanTransition : public StateTransition
{
  float threshold;
public:
  FolloweeHitpointsLessThanTransition(float in_thres) : threshold(in_thres) {}
  bool isAvailable(flecs::world &, flecs::entity entity) const override
  {
    bool hitpointsThresholdReached = false;
    entity.get([&](const FollowTarget &target)
    {
        target.target.get([&](const Hitpoints &hp)
        {
          hitpointsThresholdReached |= hp.hitpoints < threshold;
        });
    });
    return hitpointsThresholdReached;
  }
};

class EnemyReachableTransition : public StateTransition
{
public:
  bool isAvailable(flecs::world &, flecs::entity) const override
  {
    return false;
  }
};

class NegateTransition : public StateTransition
{
  const StateTransition *transition; // we own it
public:
  NegateTransition(const StateTransition *in_trans) : transition(in_trans) {}
  ~NegateTransition() override { delete transition; }

  bool isAvailable(flecs::world &ecs, flecs::entity entity) const override
  {
    return !transition->isAvailable(ecs, entity);
  }
};

class AndTransition : public StateTransition
{
  const StateTransition *lhs; // we own it
  const StateTransition *rhs; // we own it
public:
  AndTransition(const StateTransition *in_lhs, const StateTransition *in_rhs) : lhs(in_lhs), rhs(in_rhs) {}
  ~AndTransition() override
  {
    delete lhs;
    delete rhs;
  }

  bool isAvailable(flecs::world &ecs, flecs::entity entity) const override
  {
    return lhs->isAvailable(ecs, entity) && rhs->isAvailable(ecs, entity);
  }
};


// states
State *create_sleep_state()
{
    return new SleepState();
}
State *create_search_home_state()
{
    return new SearchHomeState();
}
State *create_eat_state()
{
    return new Eat();
}
State *create_search_food_state()
{
    return new SearchFood();
}
State *create_sm_state(std::unique_ptr<StateMachine>&& sm) 
{
    return new StateMachineState(std::move(sm));
}
State *create_follow_state(float followDist)
{
  return new FollowState(followDist);
}
State *create_heal_state()
{
  return new HealState();
}
State *create_attack_enemy_state()
{
  return new AttackEnemyState();
}
State *create_move_to_enemy_state()
{
  return new MoveToEnemyState();
}

State *create_flee_from_enemy_state()
{
  return new FleeFromEnemyState();
}


State *create_patrol_state(float patrol_dist)
{
  return new PatrolState(patrol_dist);
}

State *create_nop_state()
{
  return new NopState();
}

// transitions
StateTransition *create_sleepy_transition(float thres)
{
    return new SleepyTransition(thres);
}
StateTransition *create_hungry_transition(float thres)
{
    return new HungryTransition(thres);
}
StateTransition *create_home_close_transition(float thres)
{
  return new HomeCloseTransition(thres);
}
StateTransition *create_food_close_transition(float thres)
{
  return new FoodCloseTransition(thres);
}
StateTransition *create_followee_close_transition(float thres)
{
  return new FolloweeCloseTransition(thres);
}
StateTransition *create_followee_hitpoints_less_than_transition(float thres)
{
  return new FolloweeHitpointsLessThanTransition(thres);
}
StateTransition *create_enemy_available_transition(float dist)
{
  return new EnemyAvailableTransition(dist);
}

StateTransition *create_enemy_reachable_transition()
{
  return new EnemyReachableTransition();
}

StateTransition *create_hitpoints_less_than_transition(float thres)
{
  return new HitpointsLessThanTransition(thres);
}

StateTransition *create_negate_transition(StateTransition *in)
{
  return new NegateTransition(in);
}
StateTransition *create_and_transition(StateTransition *lhs, StateTransition *rhs)
{
  return new AndTransition(lhs, rhs);
}

