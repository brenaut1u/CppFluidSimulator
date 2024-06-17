#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector2D>
#include <QPointF>
#include <QVector>
#include <QColor>
#include <memory>
#include <utility>
#include "grid.h"

using std::shared_ptr;
using std::pair;

class Grid;

class Particle
{
    /**
     * This class represents a single particle (a tiny piece of liquid).
     */

public:
    Particle(shared_ptr<float> _radius, shared_ptr<float> _influence_radius, QPointF _pos, QVector2D _speed, QColor _color, shared_ptr<Grid> _grid) :
            color(_color), radius(_radius), influence_radius(_influence_radius), pos(_pos), speed(_speed), grid(_grid) {id = particles_count++;}

    void update_pos_and_speed(float time_step);
    void update_forces(QVector<QVector2D>& _forces) {forces = std::move(_forces);}
    void update_density(pair<float, float> densities);
    void update_predicted_pos(float time_step);

    QPointF get_pos() const {return pos;}
    QVector2D get_speed() const {return speed;}
    QPointF get_predicted_pos() const {return predicted_pos;}
    float get_influence_radius() const {return *influence_radius;}
    float get_density() const {return density;}
    float get_near_density() const {return near_density;}

    int get_id() const {return id;}
    QColor get_color() const {return color;}
    void set_color(QColor _color) {color = _color;}

    static void reset_particle_count() {particles_count = 0;}

private:
    void resolve_world_border_collision(); // when the particle gets outside of the screen

private:
    static int particles_count;

    int id;
    QColor color;

    shared_ptr<float> radius;
    shared_ptr<float> influence_radius;
    QPointF pos;
    QPointF predicted_pos;
    QVector2D speed;
    QVector<QVector2D> forces;
    float density;
    float near_density;
    shared_ptr<Grid> grid;
};

#endif // PARTICLE_H
