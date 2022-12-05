#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <math.h>

// TODO: make a lot of seprate files
struct Position;

struct Position
{
  float x = 0;
  float y = 0;
};

struct Velocity : public Position {};

struct SteerDir : public Position {};

inline Position operator-(const Position &lhs, const Position &rhs)
{
  return Position{lhs.x - rhs.x, lhs.y - rhs.y};
}

inline Position operator+(const Position &lhs, const Position &rhs)
{
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

inline Position &operator+=(Position &lhs, const Position &rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

inline Position operator*(const Position &lhs, const float scalar)
{
  return {lhs.x * scalar, lhs.y * scalar};
}

inline float safeinv(float v)
{
  return fabsf(v) > 1e-7f ? 1.f / v : v;
}

inline float length_sq(const Position &v)
{
  return v.x * v.x + v.y * v.y;
}

inline float length(const Position &v)
{
  return sqrtf(length_sq(v));
}

inline Position normalize(const Position &v)
{
  const float len = length(v);
  return v * safeinv(len);
}

inline Position truncate(const Position &v, float len)
{
  const float l = length(v);
  if (l > len)
    return v * (len / l);
  return v;
}


inline bool operator==(const Position &lhs, const Position &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!=(const Position &lhs, const Position &rhs) { return !(lhs == rhs); }


struct MoveSpeed
{
  float speed = 0.f;
};

struct Hitpoints
{
  float hitpoints = 10.f;
};

enum Actions
{
  EA_NOP = 0,
  EA_MOVE_START,
  EA_MOVE_LEFT = EA_MOVE_START,
  EA_MOVE_RIGHT,
  EA_MOVE_DOWN,
  EA_MOVE_UP,
  EA_MOVE_END,
  EA_ATTACK = EA_MOVE_END,
  EA_HEAL_SELF,
  EA_NUM
};

struct Action
{
  int action = 0;
};

struct NumActions
{
  int numActions = 1;
  int curActions = 0;
};

struct MeleeDamage
{
  float damage = 2.f;
};

struct HealAmount
{
  float amount = 0.f;
};

struct PowerupAmount
{
  float amount = 0.f;
};

struct PlayerInput
{
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
};

struct Symbol
{
  char symb;
};

struct IsPlayer {};

struct WorldInfoGatherer {};

struct Team
{
  int team = 0;
};

struct TextureSource {};

struct TurnCounter
{
  int count = 0;
};

struct ActionLog
{
  std::vector<std::string> log;
  size_t capacity = 5;
};

struct BackgroundTile {};

struct DungeonData
{
  std::vector<char> tiles; // for pathfinding
  size_t width;
  size_t height;
};

struct DijkstraMapData
{
  std::vector<float> map;
};

struct VisualiseMap {};

struct DmapWeights
{
  struct WtData
  {
    float mult = 1.f;
    float pow = 1.f;
  };
  std::unordered_map<std::string, WtData> weights;
};

struct Hive {};
