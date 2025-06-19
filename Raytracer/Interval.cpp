#include "Interval.hpp"

const Interval Interval::empty = Interval(+infinity, -infinity); // Empty interval
const Interval Interval::universe = Interval(-infinity, +infinity); // Universe interval