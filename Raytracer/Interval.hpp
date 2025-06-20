#pragma once

#include <algorithm>
#include "Constants.hpp"

class Interval {
public:
  double min, max;

  Interval() : min(+infinity), max(-infinity) {} // Default interval is empty
  Interval(double min, double max) : min(min), max(max) {}

  double size() const {
    return max - min;
  }

  bool contains(double value) const {
    return min <= value && value <= max;
  }

  bool surrounds(double value) const {
    return min < value && value < max;
  }

  double clamp(double value) const {
    return std::max(min, std::min(max, value));
  }

  static const Interval empty, universe;
};
