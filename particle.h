#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector2D>
#include <QPointF>
#include <QVector>
#include <memory>
#include "grid.h"

using std::shared_ptr;

class Grid;

class Particle
{
public:
    Particle(float _radius, shared_ptr<float> _influence_radius, QPointF _pos, QVector2D _speed, shared_ptr<Grid> _grid);

    void update_pos_and_speed(float time_step);
    void update_forces(QVector<QVector2D>& _forces) {forces = std::move(_forces);}
    QPointF get_pos() const {return pos;}
    QPointF get_predicted_pos() const {return predicted_pos;}
    float get_influence_radius() const {return *influence_radius;}
    float get_density() const {return density;}
    void update_density(float _density) {density = _density;}
    void update_predicted_pos(float time_step);

private:
    void resolve_world_border_collision(); // when the particle gets outside of the screen

private:
    float radius;
    shared_ptr<float> influence_radius;
    QPointF pos;
    QPointF predicted_pos;
    QVector2D speed;
    QVector<QVector2D> forces;
    float density;
    shared_ptr<Grid> grid;
};

#endif // PARTICLE_H
