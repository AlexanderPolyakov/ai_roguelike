#pragma once

#include <cmath>

struct IVec2
{
  int x, y;
};

inline bool operator==(const IVec2 &lhs, const IVec2 &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!=(const IVec2 &lhs, const IVec2 &rhs) { return !(lhs == rhs); }

inline IVec2 operator-(const IVec2 &lhs, const IVec2 &rhs)
{
  return IVec2{lhs.x - rhs.x, lhs.y - rhs.y};
}

template<typename T>
inline T sqr(T a){ return a*a; }

template<typename T, typename U>
inline float dist_sq(const T &lhs, const U &rhs) { return float(sqr(lhs.x - rhs.x) + sqr(lhs.y - rhs.y)); }

template<typename T, typename U>
inline float dist(const T &lhs, const U &rhs) { return sqrtf(dist_sq(lhs, rhs)); }

