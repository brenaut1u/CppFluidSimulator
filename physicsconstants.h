#ifndef PHYSICSCONSTANTS_H
#define PHYSICSCONSTANTS_H

#include <QVector2D>

inline constexpr float g = 1.0;
inline constexpr QVector2D gravity = QVector2D(0, -g);
inline constexpr float collision_damping = 0.2;

#endif // PHYSICSCONSTANTS_H
