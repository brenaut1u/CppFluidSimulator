#include <QtGlobal>
#include <QtMath>
#include "grid.h"

#include <QDebug>

Grid::Grid(QPoint _nb_cells, const QSizeF& _world_size, shared_ptr<float> _g, shared_ptr<float> _collision_damping,
           shared_ptr<float> _fluid_density, shared_ptr<float> _pressure_multiplier) : nb_cells(_nb_cells), world_size(_world_size), g(_g),
            collision_damping(_collision_damping), fluid_density(_fluid_density), pressure_multiplier(_pressure_multiplier)
{
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

QVector2D Grid::calculate_pressure_force(Particle* particle) {
    QVector2D pressure_force = QVector2D(0, 0);
    QPointF pos = particle->get_pos();
    float density = particle->get_density();

    QVector<QPoint> cells = get_neighbor_cells(cell_id_from_world_pos(pos));
    for (QPoint cell : cells) {
        for (auto particle2 : particles[cell_id_from_grid_pos(cell)]) {
            QVector2D dir = QVector2D(particle2->get_pos() - pos);

            //TODO: allow two particles to have the same position?
            if (dir != QVector2D(0.0, 0.0)) {
                float slope = smoothing_kernel_derivative(particle2->get_influence_radius(), dir.length());
                float density2 = particle2->get_density();
                float pressure = (density_to_pressure(density, *fluid_density, *pressure_multiplier)
                                  + density_to_pressure(density2, *fluid_density, *pressure_multiplier)) / 2.0;
                pressure_force += pressure * dir.normalized() * slope / density;
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

void Grid::change_grid(QPoint _nb_cells) {
    nb_cells = _nb_cells;
    auto new_particles = QVector<QVector<shared_ptr<Particle>>>(nb_cells.x() * nb_cells.y());

    for (auto cell : particles) {
        for (auto particle : cell) {
            int new_cell_id = cell_id_from_world_pos(particle->get_pos());
            new_particles[new_cell_id].append(particle);
        }
    }

    particles = new_particles;
}

float smoothing_kernel(float influence_radius, float distance) {
    if (distance >= influence_radius) return 0;
    float volume = M_PI * qPow(influence_radius, 4) / 6;
    return (influence_radius - distance) * (influence_radius - distance) / volume;
}

float smoothing_kernel_derivative(float influence_radius, float distance) {
    if (distance >= influence_radius) return 0;
    float scale = 12 / (M_PI * qPow(influence_radius, 4));
    return scale * (distance - influence_radius);
}

float density_to_pressure(float density, float fluid_density, float pressure_multiplier) {
    float density_error = density - fluid_density;
    float pressure = density_error * pressure_multiplier;
    return pressure;
}
