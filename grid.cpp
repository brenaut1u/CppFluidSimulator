#include <QtGlobal>
#include "physics.h"
#include "grid.h"

#include <QDebug>

Grid::Grid(QPoint _nb_cells, const QSizeF& _world_size) : nb_cells(_nb_cells), world_size(_world_size) {
    particles = QVector<QVector<shared_ptr<Particle>>>(nb_cells.x() * nb_cells.y());
}

void Grid::add_particle(shared_ptr<Particle> particle) {
    int cell_id = cell_id_from_world_pos(particle->get_pos());
    particles[cell_id].append(particle);
}

void Grid::update_particles(float time_step) {
    // This function updates the particles' positions and physical states by iterating
    // over the grid. The grids are treated by groups of nine neighboring cells, which
    // allows to test collisions only with neighboring particles.

    update_densities();
    update_particles_forces(time_step);

    for (int i = 1; i < nb_cells.x() - 1; i++) {
        for (int j = 1; j < nb_cells.y() - 1; j++) {
            QVector<QPoint> cells = get_neighbor_cells({i, j});

            for (int c1 = 0; c1 < cells.size(); c1++) {
                // for each of the neighbor cells

                for (int p1 = 0; p1 < particles[cell_id_from_grid_pos(cells[c1])].size(); p1++) {
                    // for each particle in the cell

                    shared_ptr<Particle> particle1 = particles[cell_id_from_grid_pos(cells[c1])][p1];
                    //particle1->update_forces(time_step);

                    for (int c2 = c1; c2 < cells.size(); c2++) {
                        // for each of the neighbor cells that hasn't been explored yet

                        for (int p2 = 0; p2 < particles[cell_id_from_grid_pos(cells[c2])].size(); p2++) {
                            // for each of the other particles that hasn't been tested yet

                            if (c2 > c1 || p2 > p1) {
                                shared_ptr<Particle> particle2 = particles[cell_id_from_grid_pos(cells[c2])][p2];
                                particle1->test_collision(particle2);
                                //update_pos_on_grid(particle2, cell_id_from_grid_pos(cells[c2]), p2);
                            }
                        }
                    }
                    //update_pos_on_grid(particle1, cell_id_from_grid_pos(cells[c1]), p1);
                }
            }
        }
    }

    update_particles_pos_on_grid();
}

void Grid::update_particles_forces(float time_step) {
    for (auto cell : particles) {
        for (auto particle : cell) {
            particle->update_forces(time_step);
        }
    }
}

void Grid::update_particles_pos_on_grid() {
    // Updates the grid so as to place all the particles in the right cell.

    for (int i = 0; i < particles.size(); i++) {
        int j = 0;
        while (j < particles[i].size()) {
            auto particle = particles[i][j];
            int new_cell_id = cell_id_from_world_pos(particle->get_pos());

            if (new_cell_id != i) {
                particles[i].remove(j);
                particles[new_cell_id].append(particle);
            }
            else {
                j++;
            }
        }
    }
}

//void Grid::update_pos_on_grid(shared_ptr<Particle> particle, int old_cell_id, int i) {
//    // Updates the grid so as to place the particle in the right cell.
//    // i is the particle's index in the old cell.

//    int new_cell_id = cell_id_from_world_pos(particle->get_pos());

//    particles[old_cell_id].remove(i);
//    particles[new_cell_id].append(particle);
//}

int Grid::cell_id_from_world_pos(QPointF pos) {
    // Returns the id of the cell containing the world position

    float cells_width = world_size.width() / nb_cells.x();
    float cells_height = world_size.height() / nb_cells.y();
    int cell_id = int(pos.y() / cells_height) * nb_cells.x() + int(pos.x() / cells_width);
    return cell_id;
}

QVector<QPoint> Grid::get_neighbor_cells(QPoint pos) {
    // Returns the positions on the grid of the neighbor cells

    QVector<QPoint> cells = QVector<QPoint>();
    cells.append(pos);
    if (pos.x() > 0) {
        cells.append({pos.x() - 1, pos.y()});
        if (pos.y() > 0) {
            cells.append({pos.x() - 1, pos.y() - 1});
        }
        if (pos.y() < nb_cells.y() - 1) {
            cells.append({pos.x() - 1, pos.y() + 1});
        }
    }
    if (pos.x() < nb_cells.x() - 1) {
        cells.append({pos.x() + 1, pos.y()});
        if (pos.y() > 0) {
            cells.append({pos.x() + 1, pos.y() - 1});
        }
        if (pos.y() < nb_cells.y() - 1) {
            cells.append({pos.x() + 1, pos.y() + 1});
        }
    }
    if (pos.y() > 0) {
        cells.append({pos.x(), pos.y() - 1});
    }
    if (pos.y() < nb_cells.y() - 1) {
        cells.append({pos.x(), pos.y() + 1});
    }

    return cells;
}

float Grid::calculate_density(QPointF pos) {
    float density = 0;

    QVector<QPoint> cells = get_neighbor_cells(cell_id_from_world_pos(pos));
    for (QPoint cell : cells) {
        for (auto particle : particles[cell_id_from_grid_pos(cell)]) {
            float distance = QVector2D(particle->get_pos() - pos).length();
            float influence = smoothing_kernel(particle->get_influence_radius(), distance);
            density += influence;
        }
    }

    return density;
}

QVector2D Grid::calculate_pressure_force(QPointF pos) {
    QVector2D pressure_force = QVector2D(0, 0);

    QVector<QPoint> cells = get_neighbor_cells(cell_id_from_world_pos(pos));
    for (QPoint cell : cells) {
        for (auto particle : particles[cell_id_from_grid_pos(cell)]) {
            QVector2D dir = QVector2D(particle->get_pos() - pos);

            //TODO: allow two particles to have the same position?
            if (dir != QVector2D(0.0, 0.0)) {
                float slope = smoothing_kernel_derivative(particle->get_influence_radius(), dir.length());
                float density = particle->get_density();
                pressure_force += density_to_pressure(density) * dir.normalized() * slope / density;
            }
        }
    }

    return pressure_force;
}

void Grid::update_densities() {
    for (auto cell : particles) {
        for (auto particle : cell) {
            particle->update_density(calculate_density(particle->get_pos()));
        }
    }
}
