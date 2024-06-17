#ifndef INTERACTION_H
#define INTERACTION_H

#include <QPointF>

/**
 * This structure describes a user interaction (when the user clicks on the particle system to create an attractive or repulsive force)
 */

struct Interaction {
    QPointF pos; // world pos
    float radius;
    float strength;
};

#endif // INTERACTION_H
