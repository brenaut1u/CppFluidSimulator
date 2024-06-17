#include <QtMath>
#include <future>
#include <thread>
#include <mutex>
#include <vector>
#include "grid.h"

#include <QDebug>

inline constexpr float epsilon = 0.0001;
inline constexpr int nb_threads = 4;

std::mutex mutex_update_particles_pos_on_grid;

Grid::Grid(QPoint _nb_cells, const QSizeF& _world_size, shared_ptr<float> _g, shared_ptr<float> _collision_damping,
           shared_ptr<float> _fluid_density, shared_ptr<float> _pressure_multiplier, shared_ptr<float> _near_pressure_multiplier,
           shared_ptr<float> _viscosity_multiplier) :
                world_size(_world_size), nb_cells(_nb_cells), g(_g), collision_damping(_collision_damping),
                fluid_density(_fluid_density), pressure_multiplier(_pressure_multiplier), near_pressure_multiplier(_near_pressure_multiplier),
                viscosity_multiplier(_viscosity_multiplier)
{
    particles = QVector<QVector<shared_ptr<Particle>>>(nb_cells.x() * nb_cells.y());
    random = QRandomGenerator();
    Particle::reset_particle_count();
}

void Grid::add_particle(shared_ptr<Particle> particle) {
    int cell_id = cell_id_from_world_pos(particle->get_pos());
    particles[cell_id].append(particle);
}

void Grid::update_particles(float time_step, const Interaction& interaction) {
    // This function updates the particles' positions and physical states by iterating
    // over the grid. The grids are treated by groups of nine neighboring cells, which
    // allows to test collisions only with neighboring particles.

    std::vector<std::future<void>> threads_update_predicted_pos = std::vector<std::future<void>>(nb_threads);
    for (int i = 0; i < nb_threads; i++)
        threads_update_predicted_pos[i] = std::async(&Grid::update_predicted_pos, this, time_step,
                                                       i * particles.size() / nb_threads, (i + 1) * particles.size() / nb_threads);
    for (int i = 0; i < nb_threads; i++) threads_update_predicted_pos[i].get();



    std::vector<std::future<void>> threads_update_densities = std::vector<std::future<void>>(nb_threads);
    for (int i = 0; i < nb_threads; i += 2)
        threads_update_densities[i] = std::async(&Grid::update_densities, this,
                                                 i * nb_cells.x() / nb_threads, (i + 1) * nb_cells.x() / nb_threads);
    for (int i = 0; i < nb_threads; i += 2) threads_update_densities[i].get();

    for (int i = 1; i < nb_threads; i += 2)
        threads_update_densities[i] = std::async(&Grid::update_densities, this,
                                                 i * nb_cells.x() / nb_threads, (i + 1) * nb_cells.x() / nb_threads);
    for (int i = 1; i < nb_threads; i += 2) threads_update_densities[i].get();


    std::vector<std::future<void>> threads_update_particles_pos_and_speed = std::vector<std::future<void>>(nb_threads);
    for (int i = 0; i < nb_threads; i += 2)
        threads_update_particles_pos_and_speed[i] = std::async(&Grid::update_particles_pos_and_speed, this, time_step, interaction,
                                                 i * nb_cells.x() / nb_threads, (i + 1) * nb_cells.x() / nb_threads);
    for (int i = 0; i < nb_threads; i += 2) threads_update_particles_pos_and_speed[i].get();

    for (int i = 1; i < nb_threads; i += 2)
        threads_update_particles_pos_and_speed[i] = std::async(&Grid::update_particles_pos_and_speed, this, time_step, interaction,
                                                 i * nb_cells.x() / nb_threads, (i + 1) * nb_cells.x() / nb_threads);
    for (int i = 1; i < nb_threads; i += 2) threads_update_particles_pos_and_speed[i].get();


    //The simulation is no longer deterministic when we enable the mutex multithread below

//    std::vector<std::thread> threads_update_particles_pos_on_grid = std::vector<std::thread>(nb_threads);
//    for (int i = 0; i < nb_threads; i++)
//        threads_update_particles_pos_on_grid[i] = std::thread(&Grid::update_particles_pos_on_grid, this,
//                                                          i * particles.size() / nb_threads, (i + 1) * particles.size() / nb_threads);

//    for (int i = 0; i < nb_threads; i++) threads_update_particles_pos_on_grid[i].join();

    update_particles_pos_on_grid(0, particles.size());
}

void Grid::update_particles_pos_and_speed(float time_step, const Interaction& interaction, int start_cell_pos_x, int end_cell_pos_x) {
    // Updates the particles forces, position and speed, including the interaction force (when the user clicks on the particle system)

    const QVector2D gravity = QVector2D(0, -(*g));

    for (int i = start_cell_pos_x; i < qMin(end_cell_pos_x, nb_cells.x()); i++) {
        for (int j = 0; j < nb_cells.y(); j++) {
            for (auto particle : particles[cell_id_from_grid_pos({i, j})]) {
                QVector<QVector2D> forces = QVector<QVector2D>();
                forces.append(gravity);
                forces.append(calculate_pressure_force(particle) / particle->get_density());
                forces.append(calculate_viscosity_force(particle));
                forces.append(interaction_force(particle, interaction));
                particle->update_forces(forces);

                particle->update_pos_and_speed(time_step);
            }
        }
    }
}

void Grid::update_predicted_pos(float time_step, int start_cell_id, int end_cell_id) {
    for (int i = start_cell_id; i < qMin(end_cell_id, particles.size()); i++) {
        for (auto particle : particles[i]) {
            particle->update_predicted_pos(time_step);
        }
    }
}

void Grid::update_densities(int start_cell_pos_x, int end_cell_pos_x) {
    for (int i = start_cell_pos_x; i < qMin(end_cell_pos_x, nb_cells.x()); i++) {
        for (int j = 0; j < nb_cells.y(); j++) {
            for (auto particle : particles[cell_id_from_grid_pos({i, j})]) {
                particle->update_density(calculate_density(particle));
            }
        }
    }
}

void Grid::update_particles_pos_on_grid(int start_cell_id, int end_cell_id) {
    // Updates the grid so as to place all the particles in the right cell.

    std::lock_guard<std::mutex> guard(mutex_update_particles_pos_on_grid);

    for (int i = start_cell_id; i < qMin(end_cell_id, particles.size()); i++) {
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

pair<float, float> Grid::calculate_density(const shared_ptr<Particle>& particle) {
    float density = 0;
    float near_density = 0;

    QPointF pos = particle->get_predicted_pos();
    QVector<QPoint> cells = get_neighbor_cells(cell_id_from_world_pos(pos));
    for (QPoint cell : cells) {
        for (auto particle2 : particles[cell_id_from_grid_pos(cell)]) {
            float distance = QVector2D(particle2->get_predicted_pos() - pos).length();
            float influence = density_smoothing_kernel(particle->get_influence_radius(), distance);
            density += influence;

            float near_influence = near_density_smoothing_kernel(particle->get_influence_radius(), distance);
            near_density += near_influence;
        }
    }

    // In some cases, when the predicted position is too far away from the current position, the density calculated above remains zero.
    // So we need to prevent this, because it would cause divisions by zero.
    density = density != 0 ? density : density_smoothing_kernel(particle->get_influence_radius(), 0);
    near_density = near_density != 0 ? near_density : near_density_smoothing_kernel(particle->get_influence_radius(), 0);

    return {density, near_density};
}

pair<float, float> Grid::density_to_pressure(float density, float near_density) {
    float density_error = density - *fluid_density;
    float pressure = density_error * *pressure_multiplier;
    float near_pressure = near_density * *near_pressure_multiplier;
    return {pressure, near_pressure};
}

QVector2D Grid::calculate_pressure_force(const shared_ptr<Particle>& particle) {
    QVector2D pressure_force = QVector2D(0, 0);

    QPointF pos = particle->get_predicted_pos();
    float density = particle->get_density();
    float near_density = particle->get_near_density();

    QVector<QPoint> cells = get_neighbor_cells(cell_id_from_world_pos(pos));
    for (QPoint cell : cells) {
        for (auto particle2 : particles[cell_id_from_grid_pos(cell)]) {
            if (particle2->get_id() != particle->get_id()) {
                QVector2D dir = QVector2D(particle2->get_predicted_pos() - pos);

                float slope = density_smoothing_kernel_derivative(particle->get_influence_radius(), dir.length());

                while (dir.length() <= epsilon) {
                    dir = QVector2D(random.generateDouble() * 2 - 1, random.generateDouble() * 2 - 1);
                }

                float density2 = particle2->get_density();
                float near_density2 = particle2->get_near_density();

                auto [pressure, near_pressure] = density_to_pressure(density, near_density);
                auto [pressure2, near_pressure2] = density_to_pressure(density2, near_density2);

                pressure_force += 0.5 * (pressure + pressure2) * dir.normalized() * slope / density
                                 + 0.5 * (near_pressure + near_pressure2) * dir.normalized() * slope / near_density;
            }
        }
    }

    return pressure_force;
}

QVector2D Grid::calculate_viscosity_force(const shared_ptr<Particle>& particle) {
    QVector2D viscosity_force = QVector2D(0, 0);

    QPointF pos = particle->get_predicted_pos();

    QVector<QPoint> cells = get_neighbor_cells(cell_id_from_world_pos(pos));
    for (QPoint cell : cells) {
        for (auto particle2 : particles[cell_id_from_grid_pos(cell)]) {
            if (particle2->get_id() != particle->get_id()) {
                float dst = QVector2D(particle2->get_pos() - pos).length();
                float influence = viscosity_smoothing_kernel(particle->get_influence_radius(), dst);
                viscosity_force += (particle2->get_speed() - particle->get_speed()) * influence;
            }
        }
    }

    return viscosity_force * *viscosity_multiplier;
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

float density_smoothing_kernel(float influence_radius, float distance) {
    if (distance >= influence_radius) return 0;
    float volume = M_PI * qPow(influence_radius, 4) / 6;
    return (influence_radius - distance) * (influence_radius - distance) / volume;
}

float density_smoothing_kernel_derivative(float influence_radius, float distance) {
    if (distance >= influence_radius) return 0;
    float scale = 12 / (M_PI * qPow(influence_radius, 4));
    return -scale * (influence_radius - distance);
}

float near_density_smoothing_kernel(float influence_radius, float distance) {
    if (distance >= influence_radius) return 0;
    float volume = M_PI * qPow(influence_radius, 5) / 10;
    return qPow((influence_radius - distance), 3) / volume;
}

float near_density_smoothing_kernel_derivative(float influence_radius, float distance) {
    if (distance >= influence_radius) return 0;
    float scale = 30 / (M_PI * qPow(influence_radius, 5));
    return -scale * (influence_radius - distance) * (influence_radius - distance);
}

float viscosity_smoothing_kernel(float influence_radius, float distance) {
    if (distance >= influence_radius) return 0;
    float value = influence_radius * influence_radius - distance * distance;
    return qPow(value, 3);
}

QVector2D interaction_force(shared_ptr<Particle> particle, Interaction interaction) {
    QVector2D interac_force = {0, 0};

    QVector2D offset = QVector2D((interaction.pos - particle->get_pos()));
    float dst = offset.length();

    if (dst < interaction.radius) {
        QVector2D dirToInputPoint = offset.normalized();
        float centerT = 1 - dst / interaction.radius;
        interac_force += (dirToInputPoint * interaction.strength - particle->get_speed()) * centerT;
    }

    return interac_force;
}
