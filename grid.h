#ifndef GRID_H
#define GRID_H

#include <QPoint>
#include <QSizeF>
#include <QVector>
#include <QVector2D>
#include <memory>
#include <QPointF>
#include "particle.h"

#include <QDebug>

using std::shared_ptr;

class Particle;

class Grid
{
/**
  Cells are identified by an id. The bottom left cell's id is 0, and the top right cell has the maximum id.
**/
public:
    Grid(QPoint _nb_cells, const QSizeF& _world_size, shared_ptr<float> _g, shared_ptr<float> _collision_damping,
         shared_ptr<float> _fluid_density, shared_ptr<float> _pressure_multiplier);

    void add_particle(shared_ptr<Particle> particle);
    void update_particles(float time_step);
    QVector2D calculate_pressure_force(Particle* particle);
    void change_grid(QPoint _nb_cells);

    float get_g() {return *g;}
    float get_collision_damping() {return *collision_damping;}

private:
    void update_particles_forces(float time_step);
    void update_particles_pos_on_grid();
    int cell_id_from_world_pos(QPointF pos);

    QVector<QPoint> get_neighbor_cells(QPoint pos);

    QVector<QPoint> get_neighbor_cells(int id) {
        return get_neighbor_cells(grid_pos_from_cell_id(id));
    }

    inline int cell_id_from_grid_pos(QPoint pos) {
        // Returns the id of the cell at the given position in the grid (eg: third cell from left, first from bottom)
        return pos.y() * nb_cells.x() + pos.x();
    }

    inline QPoint grid_pos_from_cell_id(int id) {
        return {id % nb_cells.x(), id / nb_cells.x()};
    }

    float calculate_density(QPointF pos);
    void update_densities();

public:
    const QSizeF& world_size;

private:
    QPoint nb_cells;
    QVector<QVector<shared_ptr<Particle>>> particles; //an array containing the cells of the grid, containing pointers to the particles

    shared_ptr<float> g;
    shared_ptr<float> collision_damping;
    shared_ptr<float> fluid_density;
    shared_ptr<float> pressure_multiplier;
};

// These two functions are used to calculate the density
float smoothing_kernel(float influence_radius, float distance);
float smoothing_kernel_derivative(float influence_radius, float distance);

float density_to_pressure(float density, float fluid_density, float pressure_multiplier);

#endif // GRID_H
