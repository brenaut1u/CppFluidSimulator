#include "grid.h"
Grid::Grid(QPoint _nb_cells, const QSizeF& _world_size) : nb_cells(_nb_cells), world_size(_world_size) {
    particles = QVector<QVector<shared_ptr<Particle>>>(nb_cells.x() * nb_cells.y());
}

void Grid::add_particle(shared_ptr<Particle> particle) {
    int cell_id = cell_id_from_world_pos(particle->get_pos());
    particles[cell_id].append(particle);
}

void Grid::update_particles(float time_step) {
    //TODO: deal with the case when nb_cells' values are less than 3
    for (int i = 1; i < nb_cells.x() - 1; i++) {
        for (int j = 1; j < nb_cells.y() - 1; j++) {

            QPoint cells[9] = { {i-1, j-1},
                                {i  , j-1},
                                {i+1, j-1},
                                {i-1, j  },
                                {i  , j  },
                                {i+1, j  },
                                {i-1, j+1},
                                {i  , j+1},
                                {i+1, j+1} };

            for (int c1 = 0; c1 < 9; c1++) {
                // for each of the nine cells
                for (int p1 = 0; p1 < particles[cell_id_from_pos(cells[c1])].size(); p1++) {
                    // for each particle in the cell
                    shared_ptr<Particle> particle1 = particles[cell_id_from_pos(cells[c1])][p1];
                    particle1->update_forces(time_step);

                    for (int c2 = c1; c2 < 9; c2++) {
                        // for each of the cells that hasn't been explored yet
                        for (int p2 = 0; p2 < particles[cell_id_from_pos(cells[c2])].size(); p2++) {
                            // for each of the other particles that hasn't been tested yet
                            if (c2 > c1 || p2 > p1) {
                                shared_ptr<Particle> particle2 = particles[cell_id_from_pos(cells[c2])][p2];
                                particle1->test_collision(particle2);
                                update_pos_on_grid(particle2, cell_id_from_pos(cells[c2]), p2);
                            }
                        }
                    }
                    update_pos_on_grid(particle1, cell_id_from_pos(cells[c1]), p1);
                }
            }
        }
    }
}

void Grid::update_pos_on_grid(shared_ptr<Particle> particle, int old_cell_id, int i) {
    // Updates the grid so as to place the particle in the right cell.
    // i is the particle's index in the old cell.

    int new_cell_id = cell_id_from_world_pos(particle->get_pos());

    particles[old_cell_id].remove(i);
    particles[new_cell_id].append(particle);
}

int Grid::cell_id_from_world_pos(QPointF pos) {
    // Returns the id of the cell containing the world position

    float cells_width = world_size.width() / nb_cells.x();
    float cells_height = world_size.height() / nb_cells.y();
    int cell_id = int(pos.y() / cells_height) * nb_cells.x() + int(pos.x() / cells_width);
    return cell_id;
}
