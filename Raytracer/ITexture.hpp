#pragma once

#include <glm/glm.hpp>

class ITexture {
public:
    virtual ~ITexture() = default;
    // Returns the color at the given UV coordinates
    virtual glm::dvec3 color_value(double u, double v, const glm::dvec3& point) const = 0;
};