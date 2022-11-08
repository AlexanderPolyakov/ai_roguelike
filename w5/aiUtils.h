#pragma once
#include <flecs.h>
#include "blackboard.h"
#include <float.h>
#include "math.h"

template<typename T, typename U>
inline int move_towards(const T &from, const U &to)
{
  int deltaX = to.x - from.x;
  int deltaY = to.y - from.y;
  if (abs(deltaX) > abs(deltaY))
    return deltaX > 0 ? EA_MOVE_RIGHT : EA_MOVE_LEFT;
  return deltaY < 0 ? EA_MOVE_UP : EA_MOVE_DOWN;
}

inline int inverse_move(int move)
{
  return move == EA_MOVE_LEFT ? EA_MOVE_RIGHT :
         move == EA_MOVE_RIGHT ? EA_MOVE_LEFT :
         move == EA_MOVE_UP ? EA_MOVE_DOWN :
         move == EA_MOVE_DOWN ? EA_MOVE_UP : move;
}

template<typename Callable>
inline void on_closest_enemy_pos(flecs::world &ecs, flecs::entity entity, Callable c)
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

template<typename T>
inline size_t reg_entity_blackboard_var(flecs::entity entity, const char *bb_name)
{
  size_t res = size_t(-1);
  entity.set([&](Blackboard &bb)
  {
    res = bb.regName<T>(bb_name);
  });
  return res;
}

