#ifndef PHYSICSCONSTANTS_H
#define PHYSICSCONSTANTS_H

#include <QVector2D>
#include <QtMath>

inline constexpr float g = 1.0;
inline constexpr QVector2D gravity = QVector2D(0, -g);
inline constexpr float collision_damping = 0.85;
inline constexpr float fluid_density = 1;
inline constexpr float pressure_multiplier = 1;

// These two functions are used to calculate the density
inline float smoothing_kernel(float influence_radius, float distance) {
    float volume = M_PI * qPow(influence_radius, 8) / 4;
    float f = qMax((float) 0, influence_radius * influence_radius - distance * distance);
    return qPow(f, 3) / volume;
}

inline float smoothing_kernel_derivative(float influence_radius, float distance) {
    if (distance >= influence_radius) return 0;
    float f = influence_radius * influence_radius - distance * distance;
    float scale = -24 / (M_PI * qPow(influence_radius, 8));
    return scale * distance * f * f;
}

inline float density_to_pressure(float density) {
    float density_error = density - fluid_density;
    float pressure = density_error * pressure_multiplier;
    return pressure;
}

#endif // PHYSICSCONSTANTS_H
