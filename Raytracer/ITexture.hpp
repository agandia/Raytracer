#pragma once

#include <glm/glm.hpp>

class ITexture {
public:
    virtual ~ITexture() = default;
    // Returns the color at the given UV coordinates
    virtual glm::dvec3 color_value(double u, double v, const glm::dvec3& point) const = 0;
};

class SolidColorTexture : public ITexture {

public:
    SolidColorTexture(const glm::dvec3& albedo) : albedo_(albedo) {}
    SolidColorTexture(double r, double g, double b) : SolidColorTexture(glm::dvec3(r, g, b)) {}

    glm::dvec3 color_value(double u, double v, const glm::dvec3& point) const override {
        return albedo_;
    }

private:
    glm::dvec3 albedo_;
};