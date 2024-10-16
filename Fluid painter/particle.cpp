#include "particle.h"

#include <QDebug>

int Particle::particles_count = 0;

void Particle::update_density(pair<float, float> densities) {
    density = std::get<0>(densities);
    near_density = std::get<1>(densities);
}

void Particle::update_pos_and_speed(float time_step) {
    for (QVector2D force : forces) {
        speed += force * time_step;
    }

    pos += (speed * time_step).toPointF();

    resolve_world_border_collision();
}

void Particle::resolve_world_border_collision() {
    if (pos.x() - *radius < 0) {
        pos.setX(*radius);
        speed.setX(-speed.x());
        speed *= grid->get_collision_damping();
    }
    else if (pos.x() + *radius >= grid->world_size.width()) {
        pos.setX(grid->world_size.width() - *radius);
        speed.setX(-speed.x());
        speed *= grid->get_collision_damping();
    }

    if (pos.y() - *radius < 0) {
        pos.setY(*radius);
        speed.setY(-speed.y());
        speed *= grid->get_collision_damping();
    }
    else if (pos.y() + *radius >= grid->world_size.height()) {
        pos.setY(grid->world_size.height() - *radius);
        speed.setY(-speed.y());
        speed *= grid->get_collision_damping();
    }
}

void Particle::update_predicted_pos(float time_step) {
    predicted_pos = pos + (speed * time_step).toPointF();

    if (predicted_pos.x() - *radius < 0)
        predicted_pos.setX(*radius);
    else if (predicted_pos.x() + *radius >= grid->world_size.width())
        predicted_pos.setX(grid->world_size.width() - *radius);

    if (predicted_pos.y() - *radius < 0)
        predicted_pos.setY(*radius);
    else if (predicted_pos.y() + *radius >= grid->world_size.height())
        predicted_pos.setY(grid->world_size.height() - *radius);
}
