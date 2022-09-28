#pragma once

#include "stateMachine.h"
#include <memory>

// states
State *create_follow_state(float followDist);
State *create_attack_enemy_state();
State *create_move_to_enemy_state();
State *create_flee_from_enemy_state();
State *create_patrol_state(float patrol_dist);
State *create_nop_state();
State *create_heal_state();
State *create_sm_state(std::unique_ptr<StateMachine>&& sm);
State *create_eat_state();
State *create_search_food_state();
State *create_sleep_state();
State *create_search_home_state();

// transitions
StateTransition *create_followee_close_transition(float thres);
StateTransition *create_followee_hitpoints_less_than_transition(float thres);
StateTransition *create_enemy_available_transition(float dist);
StateTransition *create_enemy_reachable_transition();
StateTransition *create_hitpoints_less_than_transition(float thres);
StateTransition *create_negate_transition(StateTransition *in);
StateTransition *create_and_transition(StateTransition *lhs, StateTransition *rhs);
StateTransition *create_home_close_transition(float thres);
StateTransition *create_food_close_transition(float thres);
StateTransition *create_hungry_transition(float thres = 20.0f);
StateTransition *create_sleepy_transition(float thres = 20.0f);

