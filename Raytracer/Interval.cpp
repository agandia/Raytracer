#include "Interval.hpp"

const Interval Interval::empty = Interval(+infinity, -infinity); // Empty interval
const Interval Interval::universe = Interval(-infinity, +infinity); // Universe interval


Interval operator+(const Interval& ival, double displacement) {
  return Interval(ival.min + displacement, ival.max + displacement);
}

Interval operator+(double displacement, const Interval& ival) {
  return ival + displacement;
}