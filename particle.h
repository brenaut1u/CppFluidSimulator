#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector2D>
#include <QPointF>
#include <memory>
#include "grid.h"

using std::shared_ptr;

class Grid;

class Particle
{
public:
    Particle(float _radius, QPointF _pos, QVector2D _speed, shared_ptr<Grid> _grid) :
        radius(_radius), pos(_pos), speed(_speed), grid(_grid) {}
    void update_forces(float time_step);
    void test_collision(shared_ptr<Particle> other);
    QPointF get_pos() const {return pos;}

private:
    void resolve_world_border_collision(); // when the particle gets outside of the screen

private:
    float radius;
    QPointF pos;
    QVector2D speed;
    shared_ptr<Grid> grid;
};

#endif // PARTICLE_H
