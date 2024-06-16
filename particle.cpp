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

    color = speed_to_color(speed.length());
}

void Particle::resolve_world_border_collision() {
    if (pos.x() - radius < 0) {
        pos.setX(radius);
        speed.setX(-speed.x());
        speed *= grid->get_collision_damping();
    }
    else if (pos.x() + radius >= grid->world_size.width()) {
        pos.setX(grid->world_size.width() - radius);
        speed.setX(-speed.x());
        speed *= grid->get_collision_damping();
    }

    if (pos.y() - radius < 0) {
        pos.setY(radius);
        speed.setY(-speed.y());
        speed *= grid->get_collision_damping();
    }
    else if (pos.y() + radius >= grid->world_size.height()) {
        pos.setY(grid->world_size.height() - radius);
        speed.setY(-speed.y());
        speed *= grid->get_collision_damping();
    }
}

void Particle::update_predicted_pos(float time_step) {
    predicted_pos = pos + (speed * time_step).toPointF();

    if (predicted_pos.x() - radius < 0)
        predicted_pos.setX(radius);
    else if (predicted_pos.x() + radius >= grid->world_size.width())
        predicted_pos.setX(grid->world_size.width() - radius);

    if (predicted_pos.y() - radius < 0)
        predicted_pos.setY(radius);
    else if (predicted_pos.y() + radius >= grid->world_size.height())
        predicted_pos.setY(grid->world_size.height() - radius);
}

QColor blend_colors(QColor c1, QColor c2, float a) {
    if (a < 0) return c1;
    if (a > 1) return c2;

    return QColor((1 - a) * c1.red() + a * c2.red(),
                  (1 - a) * c1.green() + a * c2.green(),
                  (1 - a) * c1.blue() + a * c2.blue());
}

QColor speed_to_color(float speed) {
    speed = speed >= max_speed ? max_speed : speed; // when the speed is too high, the euclidean division doesn't behave well because we
                                                    // reach the upper limit for ints. So we set the speed at max_speed, because higher speeds
                                                    // will appear with the same color anyway.

    float d = speed / (max_speed / 4);
    int q = int(d);
    float r = d - q;

    if (q >= 4) return color_scale[4];

    return blend_colors(color_scale[q], color_scale[q + 1], r);
}
