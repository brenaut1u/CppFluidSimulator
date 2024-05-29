#include "grid.h"

Grid::Grid(QPoint _nb_cells, const QSizeF& _world_size) : nb_cells(_nb_cells), world_size(_world_size) {
    particles = QVector<QVector<shared_ptr<Particle>>>(nb_cells.x() * nb_cells.y());
}

int Grid::down(int cell_id) {
    if (cell_id == -1) return -1;
    if (cell_id >= nb_cells.x()) {
        return cell_id - nb_cells.x();
    }
    else {
        return -1;
    }
}

int Grid::up(int cell_id) {
    if (cell_id == -1) return -1;
    if (cell_id < (nb_cells.y() - 1) * nb_cells.x()) {
        return cell_id + nb_cells.x();
    }
    else {
        return -1;
    }
}

int Grid::left(int cell_id) {
    if (cell_id == -1) return -1;
    if (cell_id % nb_cells.x() > 0) {
        return cell_id - 1;
    }
    else {
        return -1;
    }
}

int Grid::right(int cell_id) {
    if (cell_id == -1) return -1;
    if (cell_id % nb_cells.x() < nb_cells.x() - 1) {
        return cell_id + 1;
    }
    else {
        return -1;
    }
}
