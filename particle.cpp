#include "physics.h"
#include "particle.h"

#include <QDebug>

void Particle::update_forces(float time_step) {
    speed += gravity * time_step;
    speed = (grid->calculate_pressure_force(pos) / density) * time_step;

    pos += (speed * time_step).toPointF();

    resolve_world_border_collision();
}

void Particle::resolve_world_border_collision() {
    if (pos.x() - radius < 0) {
        pos.setX(radius);
        speed.setX(-speed.x());
        speed *= collision_damping;
    }
    else if (pos.x() + radius >= grid->world_size.width()) {
        pos.setX(grid->world_size.width() - radius);
        speed.setX(-speed.x());
        speed *= collision_damping;
    }

    if (pos.y() - radius < 0) {
        pos.setY(radius);
        speed.setY(-speed.y());
        speed *= collision_damping;
    }
    else if (pos.y() + radius >= grid->world_size.height()) {
        pos.setY(grid->world_size.height() - radius);
        speed.setY(-speed.y());
        speed *= collision_damping;
    }
}

void Particle::test_collision(shared_ptr<Particle> other) {

}
