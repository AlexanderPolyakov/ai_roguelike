#pragma once

template<typename T>
T square(T v) { return v * v; }

struct Position
{
  int x = 0;
  int y = 0;
};

inline bool operator==(const Position &lhs, const Position &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!=(const Position &lhs, const Position &rhs) { return !(lhs == rhs); }


inline Position operator-(const Position &lhs, const Position &rhs)
{
  return Position{lhs.x - rhs.x, lhs.y - rhs.y};
}

template<typename T, typename U>
inline float dist_sq(const T &lhs, const U &rhs) { return float(square(lhs.x - rhs.x) + square(lhs.y - rhs.y)); }

