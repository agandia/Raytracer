#pragma once

#include <algorithm>
#include "Constants.hpp"

class Interval {
public:
  double min, max;

  Interval() : min(+infinity), max(-infinity) {} // Default interval is empty
  Interval(double min, double max) : min(min), max(max) {}
  Interval(const Interval& a, const Interval& b) {
    // Create the interval tightly enclosing the two input intervals.
    min = a.min <= b.min ? a.min : b.min;
    max = a.max >= b.max ? a.max : b.max;
  }

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

  Interval expand(double delta) {
    const double padding = delta / 2.0;
    return Interval(min - padding, max + padding);
  }

  static const Interval empty, universe;
};
