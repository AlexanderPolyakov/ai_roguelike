#include "roguelike.h"
#include "ecsTypes.h"
#include "debug_draw_inc.h"
#include "stateMachine.h"
#include "aiLibrary.h"
#include "app.h"
#include <iostream>

//for scancodes
#include <GLFW/glfw3.h>
#include <memory>

static void add_patrol_attack_flee_sm(flecs::entity entity)
{
  entity.get([](StateMachine &sm)
  {
    int patrol = sm.addState(create_patrol_state(3.f));
    int moveToEnemy = sm.addState(create_move_to_enemy_state());
    int fleeFromEnemy = sm.addState(create_flee_from_enemy_state());

    sm.addTransition(create_enemy_available_transition(3.f), patrol, moveToEnemy);
    sm.addTransition(create_negate_transition(create_enemy_available_transition(5.f)), moveToEnemy, patrol);

    sm.addTransition(create_and_transition(create_hitpoints_less_than_transition(60.f), create_enemy_available_transition(5.f)),
                     moveToEnemy, fleeFromEnemy);
    sm.addTransition(create_and_transition(create_hitpoints_less_than_transition(60.f), create_enemy_available_transition(3.f)),
                     patrol, fleeFromEnemy);

    sm.addTransition(create_negate_transition(create_enemy_available_transition(7.f)), fleeFromEnemy, patrol);
  });
}

void construct_patrol_flee_sm(StateMachine& sm, float patrol_length = 3.0f) {
    int patrol = sm.addState(create_patrol_state(patrol_length));
    int fleeFromEnemy = sm.addState(create_flee_from_enemy_state());

    sm.addTransition(create_enemy_available_transition(patrol_length), patrol, fleeFromEnemy);
    sm.addTransition(create_negate_transition(create_enemy_available_transition(5.f)), fleeFromEnemy, patrol);
}

static void add_patrol_flee_sm(flecs::entity entity)
{
  entity.get(construct_patrol_flee_sm);
}

static void add_attack_sm(flecs::entity entity)
{
  entity.get([](StateMachine &sm)
  {
    sm.addState(create_move_to_enemy_state());
  });
}

void add_berserker_sm(flecs::entity entity)
{
  entity.get([](StateMachine &sm)
  {
    int patrol = sm.addState(create_patrol_state(3.f));
    int moveToEnemy = sm.addState(create_move_to_enemy_state());
    int moveToEnemyInfinite = sm.addState(create_move_to_enemy_state());

    sm.addTransition(create_enemy_available_transition(3.f), patrol, moveToEnemy);
    sm.addTransition(create_negate_transition(create_enemy_available_transition(5.f)), moveToEnemy, patrol);

    sm.addTransition(create_hitpoints_less_than_transition(60.f), patrol, moveToEnemyInfinite);
  });
}

void add_heal_sm(flecs::entity entity)
{
  entity.get([](StateMachine &sm)
  {
    int patrol = sm.addState(create_patrol_state(3.f));
    int moveToEnemy = sm.addState(create_move_to_enemy_state());
    int heal = sm.addState(create_heal_state());

    sm.addTransition(create_enemy_available_transition(3.f), patrol, moveToEnemy);
    sm.addTransition(create_negate_transition(create_enemy_available_transition(5.f)), moveToEnemy, patrol);

    sm.addTransition(create_hitpoints_less_than_transition(60.f), patrol, heal);
    sm.addTransition(create_hitpoints_less_than_transition(60.f), moveToEnemy, heal);

    sm.addTransition(create_negate_transition(create_hitpoints_less_than_transition(100.f)), heal, patrol);
  });
}

void add_squire_sm(flecs::entity entity)
{
  entity.get([](StateMachine &sm)
  {
    int follow = sm.addState(create_follow_state(2.f));
    int moveToEnemy = sm.addState(create_move_to_enemy_state());
    int heal = sm.addState(create_heal_state());

    sm.addTransition(create_enemy_available_transition(3.f), follow, moveToEnemy);
    sm.addTransition(create_negate_transition(create_enemy_available_transition(4.f)), moveToEnemy, follow);

    sm.addTransition(create_and_transition(create_followee_hitpoints_less_than_transition(60.f), create_followee_close_transition(4.0f)), follow, heal);

    sm.addTransition(create_negate_transition(create_followee_hitpoints_less_than_transition(100.f)), heal, follow);
    sm.addTransition(create_negate_transition(create_followee_close_transition(4.f)), heal, follow);
  });
}

auto create_hunger_sm() {
    auto res = std::make_unique<StateMachine>();
    auto search = res->addState(create_search_food_state());
    auto eat = res->addState(create_eat_state());

    res->addTransition(create_food_close_transition(1.0f), search, eat);
    res->addTransition(create_negate_transition(create_followee_close_transition(2.0f)), eat, search);
    return res;
}

auto create_sleep_sm() {
    auto res = std::make_unique<StateMachine>();
    auto search = res->addState(create_search_home_state());
    auto sleep = res->addState(create_sleep_state());

    res->addTransition(create_home_close_transition(1.0f), search, sleep);
    res->addTransition(create_negate_transition(create_home_close_transition(1.1f)), sleep, search);
    return res;
}

auto create_wander_sm() {
    auto res = std::make_unique<StateMachine>();
    construct_patrol_flee_sm(*res, 30.0f);
    return res;
}

void add_villager_sm(flecs::entity entity)
{
  entity.get([](StateMachine &sm)
  {
    int wander = sm.addState(create_sm_state(create_wander_sm()));
    int hunger = sm.addState(create_sm_state(create_hunger_sm()));
    int sleep = sm.addState(create_sm_state(create_sleep_sm()));

    sm.addTransition(create_hungry_transition(20.0f), wander, hunger);
    sm.addTransition(create_negate_transition(create_hungry_transition(100.0f)), hunger, wander);
    sm.addTransition(create_sleepy_transition(20.0f), wander, sleep);
    sm.addTransition(create_negate_transition(create_sleepy_transition(80.0f)), sleep, wander);

    sm.addTransition(create_enemy_available_transition(3.0f), hunger, wander);
    sm.addTransition(create_enemy_available_transition(3.0f), sleep, wander);
  });
}

static flecs::entity create_monster(flecs::world &ecs, int x, int y, uint32_t color)
{
  return ecs.entity()
    .set(Position{x, y})
    .set(MovePos{x, y})
    .set(PatrolPos{x, y})
    .set(Hitpoints{100.f})
    .set(Action{EA_NOP})
    .set(Color{color})
    .set(StateMachine{})
    .set(Team{1})
    .set(NumActions{1, 0})
    .set(MeleeDamage{20.f});
}

static flecs::entity create_healing_monster(flecs::world &ecs, int x, int y, uint32_t color = 0xffff00ff)
{
  auto base = create_monster(ecs, x, y, color);
  return base
    .set(HealCooldown{4})
    .set(HealTarget{base}) // will it work??
    .set(TickCount{})
    .set(HealAmount{10.f});
}

static flecs::entity create_squire(flecs::world &ecs, int x, int y, uint32_t color, flecs::entity followee)
{
    return create_healing_monster(ecs, x, y, color)
        .set(FollowTarget{followee})
        .set(Team{0})
        .set(HealCooldown{10})
        .set(HealAmount{20.f})
        .set(HealTarget{followee});
}

static flecs::entity create_villager(flecs::world &ecs, int x, int y, uint32_t color, int food_x, int food_y)
{
  return ecs.entity()
    .set(MeleeDamage{0.f})
    .set(Position{x, y})
    .set(MovePos{x, y})
    .set(Hitpoints{80.f})
    .set(Action{EA_NOP})
    .set(Color{color})
    .set(StateMachine{})
    .set(Team{0})
    .set(FoodSourceTarget{food_x, food_y})
    .set(HungerLevel{50.0f})
    .set(SleepinessLevel{50.0f})
    .set(HomePosition{x, y})
    .set(NumActions{1, 0});
}

static auto create_player(flecs::world &ecs, int x, int y)
{
  return ecs.entity("player")
    .set(Position{x, y})
    .set(MovePos{x, y})
    .set(Hitpoints{100.f})
    .set(Color{0xffeeeeee})
    .set(Action{EA_NOP})
    .add<IsPlayer>()
    .set(Team{0})
    .set(PlayerInput{})
    .set(NumActions{2, 0})
    .set(MeleeDamage{50.f});
}

static void create_heal(flecs::world &ecs, int x, int y, float amount)
{
  ecs.entity()
    .set(Position{x, y})
    .set(HealAmount{amount})
    .set(Color{0xff4444ff});
}

static void create_powerup(flecs::world &ecs, int x, int y, float amount)
{
  ecs.entity()
    .set(Position{x, y})
    .set(PowerupAmount{amount})
    .set(Color{0xff00ffff});
}

static void register_roguelike_systems(flecs::world &ecs)
{
  ecs.system<PlayerInput, Action, const IsPlayer>()
    .each([&](PlayerInput &inp, Action &a, const IsPlayer)
    {
      bool left = app_keypressed(GLFW_KEY_LEFT);
      bool right = app_keypressed(GLFW_KEY_RIGHT);
      bool up = app_keypressed(GLFW_KEY_UP);
      bool down = app_keypressed(GLFW_KEY_DOWN);
      if (left && !inp.left)
        a.action = EA_MOVE_LEFT;
      if (right && !inp.right)
        a.action = EA_MOVE_RIGHT;
      if (up && !inp.up)
        a.action = EA_MOVE_UP;
      if (down && !inp.down)
        a.action = EA_MOVE_DOWN;
      inp.left = left;
      inp.right = right;
      inp.up = up;
      inp.down = down;
    });
  ecs.system<const Position, const Color>()
    .each([&](const Position &pos, const Color color)
    {
      DebugDrawEncoder dde;
      dde.begin(0);
      dde.push();
        dde.setColor(color.color);
        dde.drawQuad(bx::Vec3(0, 0, 1), bx::Vec3(pos.x, pos.y, 0.f), 1.f);
      dde.pop();
      dde.end();
    });

}


void init_roguelike(flecs::world &ecs)
{
  register_roguelike_systems(ecs);

  add_patrol_attack_flee_sm(create_monster(ecs, 5, 5, 0xffee00ee));
  add_patrol_attack_flee_sm(create_monster(ecs, 10, -5, 0xffee00ee));
  add_patrol_flee_sm(create_monster(ecs, -5, -5, 0xff111111));
  add_attack_sm(create_monster(ecs, -5, 5, 0xff00ff00));
  add_berserker_sm(create_monster(ecs, 6, 6, 0xffffffff));
  add_heal_sm(create_healing_monster(ecs, 7, 6));

  auto player = create_player(ecs, 0, 0);
  add_squire_sm(create_squire(ecs, 2, 2, 0xffeeee00, player));
  add_villager_sm(create_villager(ecs, -1, -2, 0xffee00ee, 2, 2));

  create_powerup(ecs, 7, 7, 10.f);
  create_powerup(ecs, 10, -6, 10.f);
  create_powerup(ecs, 10, -4, 10.f);

  create_heal(ecs, -5, -5, 50.f);
  create_heal(ecs, -5, 5, 50.f);
}

static bool is_player_acted(flecs::world &ecs)
{
  static auto processPlayer = ecs.query<const IsPlayer, const Action>();
  bool playerActed = false;
  processPlayer.each([&](const IsPlayer, const Action &a)
  {
    playerActed = a.action != EA_NOP;
  });
  return playerActed;
}

static bool upd_player_actions_count(flecs::world &ecs)
{
  static auto updPlayerActions = ecs.query<const IsPlayer, NumActions>();
  bool actionsReached = false;
  updPlayerActions.each([&](const IsPlayer, NumActions &na)
  {
    na.curActions = (na.curActions + 1) % na.numActions;
    actionsReached |= na.curActions == 0;
  });
  return actionsReached;
}

static Position move_pos(Position pos, int action)
{
  if (action == EA_MOVE_LEFT)
    pos.x--;
  else if (action == EA_MOVE_RIGHT)
    pos.x++;
  else if (action == EA_MOVE_UP)
    pos.y++;
  else if (action == EA_MOVE_DOWN)
    pos.y--;
  return pos;
}

static void process_actions(flecs::world &ecs)
{
  static auto processActions = ecs.query<Action, Position, MovePos, const MeleeDamage, const Team>();
  static auto checkAttacks = ecs.query<const MovePos, Hitpoints, const Team>();
  static auto healQuery = ecs.query<const HealTarget, Hitpoints, const TickCount, HealCooldown, const HealAmount>();
  // Process all actions
  ecs.defer([&]
  {
    processActions.each([&](flecs::entity entity, Action &a, Position &pos, MovePos &mpos, const MeleeDamage &dmg, const Team &team)
    {
      Position nextPos = move_pos(pos, a.action);
      if (a.action == EA_HEAL) {
        auto target = *entity.get<HealTarget>();
        auto hp = target.target.get<Hitpoints>();
        if (hp == nullptr) {
            return;
        }

        std::cout << "Heal: " << target.target.get<Hitpoints>()->hitpoints << '\n';
        auto& amount = *entity.get<HealAmount>();
        auto& cooldown = *entity.get<HealCooldown>();
        auto& count = *entity.get<TickCount>();

        entity.set<HealCooldown>({cooldown.cooldown, cooldown.cooldown + count.count});
        target.target.set<Hitpoints>({hp->hitpoints + amount.amount});
      }
      bool blocked = false;
      checkAttacks.each([&](flecs::entity enemy, const MovePos &epos, Hitpoints &hp, const Team &enemy_team)
      {
        if (entity != enemy && epos == nextPos)
        {
          blocked = true;
          if (team.team != enemy_team.team)
            hp.hitpoints -= dmg.damage;
        }
      });
      if (blocked)
        a.action = EA_NOP;
      else
        mpos = nextPos;
    });
    // now move
    processActions.each([&](flecs::entity, Action &a, Position &pos, MovePos &mpos, const MeleeDamage &, const Team&)
    {
      pos = mpos;
      a.action = EA_NOP;
    });
  });

  static auto deleteAllDead = ecs.query<const Hitpoints>();
  ecs.defer([&]
  {
    deleteAllDead.each([&](flecs::entity entity, const Hitpoints &hp)
    {
      if (hp.hitpoints <= 0.f)
        entity.destruct();
    });
  });

  static auto playerPickup = ecs.query<const IsPlayer, const Position, Hitpoints, MeleeDamage>();
  static auto healPickup = ecs.query<const Position, const HealAmount>();
  static auto powerupPickup = ecs.query<const Position, const PowerupAmount>();
  ecs.defer([&]
  {
    playerPickup.each([&](const IsPlayer&, const Position &pos, Hitpoints &hp, MeleeDamage &dmg)
    {
      healPickup.each([&](flecs::entity entity, const Position &ppos, const HealAmount &amt)
      {
        if (pos == ppos)
        {
          hp.hitpoints += amt.amount;
          entity.destruct();
        }
      });
      powerupPickup.each([&](flecs::entity entity, const Position &ppos, const PowerupAmount &amt)
      {
        if (pos == ppos)
        {
          dmg.damage += amt.amount;
          entity.destruct();
        }
      });
    });
  });
}

void process_turn(flecs::world &ecs)
{
  static auto stateMachineAct = ecs.query<StateMachine>();
  static auto tickCountAct = ecs.query<TickCount>();
  static auto hungerAct = ecs.query<HungerLevel, Hitpoints>();
  static auto sleepinessAct = ecs.query<SleepinessLevel>();
  if (is_player_acted(ecs))
  {
    ecs.defer([&]
    {
      tickCountAct.each([&](TickCount &count)
      {
          ++count.count;
      });
      sleepinessAct.each([&](SleepinessLevel &sleepiness){
        sleepiness.value -= 0.7f;
      });
      hungerAct.each([&](HungerLevel &hunger, Hitpoints& hp)
      {
          if (hunger.value >= 0.0f) {
              hunger.value -= 1.0f;
          } else {
              hp.hitpoints -= 1.0f;
          }
      });
    });
    if (upd_player_actions_count(ecs))
    {
      // Plan action for NPCs
      ecs.defer([&]
      {
        stateMachineAct.each([&](flecs::entity e, StateMachine &sm)
        {
          sm.act(0.f, ecs, e);
        });
      });
    }
    process_actions(ecs);
  }
}

void print_stats(flecs::world &ecs)
{
  bgfx::dbgTextClear();
  static auto playerStatsQuery = ecs.query<const IsPlayer, const Hitpoints, const MeleeDamage>();
  playerStatsQuery.each([&](const IsPlayer &, const Hitpoints &hp, const MeleeDamage &dmg)
  {
    bgfx::dbgTextPrintf(0, 1, 0x0f, "hp: %d", (int)hp.hitpoints);
    bgfx::dbgTextPrintf(0, 2, 0x0f, "power: %d", (int)dmg.damage);
  });
}

