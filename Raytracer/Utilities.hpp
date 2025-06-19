#pragma once
#include "glm/glm.hpp"

void write_color(std::ostream &out, const glm::vec3 color) {
    out << static_cast<int>(255.999f * color.r) << ' '
        << static_cast<int>(255.999f * color.g) << ' '
        << static_cast<int>(255.999f * color.b) << '\n';
}