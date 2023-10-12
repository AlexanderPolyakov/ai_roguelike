#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// TODO: make a lot of seprate files
struct Position;
struct MovePos;

struct MovePos
{
  int x = 0;
  int y = 0;

  MovePos &operator=(const Position &rhs);
};

struct Position
{
  int x = 0;
  int y = 0;

  Position &operator=(const MovePos &rhs);
};

inline Position &Position::operator=(const MovePos &rhs)
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

inline Position operator-(const Position &lhs, const Position &rhs)
{
  return Position{lhs.x - rhs.x, lhs.y - rhs.y};
}

inline MovePos &MovePos::operator=(const Position &rhs)
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

inline bool operator==(const Position &lhs, const Position &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator==(const Position &lhs, const MovePos &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator==(const MovePos &lhs, const MovePos &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator==(const MovePos &lhs, const Position &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!=(const Position &lhs, const Position &rhs) { return !(lhs == rhs); };


struct PatrolPos
{
  int x = 0;
  int y = 0;
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
  EA_PASS,
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
  bool passed = false;
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
