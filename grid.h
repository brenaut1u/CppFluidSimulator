#ifndef GRID_H
#define GRID_H

#include <QPoint>
#include <QSizeF>
#include <QVector>
#include <memory>
#include <QPointF>
#include "particle.h"

using std::shared_ptr;

class Particle;

class Grid
{
/**
  Cells are identified by an id. The bottom left cell's id is 0, and the top right cell has the maximum id.
**/
public:
    Grid(QPoint _nb_cells, const QSizeF& _world_size);
    void add_particle(shared_ptr<Particle> particle);
    void update_particles(float time_step);

private:
    void update_pos_on_grid(shared_ptr<Particle> particle, int old_cell_id, int i);
    int cell_id_from_world_pos(QPointF pos);

    int cell_id_from_pos(QPoint pos) {
        // Returns the id of the cell at the given position in the grid (eg: third cell from left, first from bottom)
        return pos.y() * nb_cells.x() + pos.x();
    }

public:
    const QSizeF& world_size;

private:
    QPoint nb_cells;
    QVector<QVector<shared_ptr<Particle>>> particles; //an array containing the cells of the grid, containing pointers to the particles
};

#endif // GRID_H
