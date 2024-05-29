#ifndef GRID_H
#define GRID_H

#include <QPoint>
#include <QSizeF>
#include <QVector>
#include <memory>
#include <QPointF>
#include "particle.h"

using std::shared_ptr;

class Grid
{
/**
  Cells are identified by an id. The bottom left cell's id is 0, and the top right cell has the maximum id.
**/
public:
    Grid(QPoint _nb_cells, const QSizeF& _world_size);
    void add_particle(QPointF pos);
    void update_pos_on_grid(shared_ptr<Particle> particle);

private:
    // These functions return the id of the cell directly (respectively) bellow, above, to the left and to the right
    // of the cell whose id is passed as parameter. If such a cell doesn't exist, returns -1.
    int down(int cell_id);
    int up(int cell_id);
    int left(int cell_id);
    int right(int cell_id);

private:
    QPoint nb_cells;
    const QSizeF& world_size;
    QVector<QVector<shared_ptr<Particle>>> particles; //an array containing the cells of the grid, containing pointers to the particles
};

#endif // GRID_H
