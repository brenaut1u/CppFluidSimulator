#include <memory>
#include <QtMath>
#include <QMouseEvent>
#include "qpainter.h"
#include "particlesystem.h"

#include <QDebug>

using std::make_shared;
using std::shared_ptr;

ParticleSystem::ParticleSystem(int _nb_particles, float _particle_radius, float _particle_influence_radius, const QSize& _im_size,
                               QSizeF _world_size, float _time_step, float _g, float _collision_damping, float _fluid_density,
                               float _pressure_multiplier, float _interaction_radius, float _interaction_strength,
                               QWidget *parent) :
         QOpenGLWidget(parent), nb_particles(_nb_particles), particle_radius(_particle_radius), time_step(_time_step),
         im_size(_im_size), world_size(_world_size), interaction_radius(_interaction_radius), interaction_strength(_interaction_strength)
{
    particle_influence_radius = make_shared<float>(_particle_influence_radius);
    g = make_shared<float>(_g);
    collision_damping = make_shared<float>(_collision_damping);
    fluid_density = make_shared<float>(_fluid_density);
    pressure_multiplier = make_shared<float>(_pressure_multiplier);

    this->setMinimumSize(im_size.width(), im_size.height());

    grid = make_shared<Grid>(QPoint(world_size.width() / *particle_influence_radius,
                                    world_size.height() / *particle_influence_radius),
                             world_size,
                             g,
                             collision_damping,
                             fluid_density,
                             pressure_multiplier);

    int n = qSqrt(nb_particles);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            shared_ptr<Particle> particle = make_shared<Particle>(particle_radius, particle_influence_radius, QPointF(i * world_size.width() / n, j * world_size.height() / n), QVector2D(0.0, 0.0), grid);
            grid->add_particle(particle);
            particles.append(particle);
        }
    }

    interaction = {{0, 0}, 0, 0};
}

void ParticleSystem::update_physics() {
    grid->update_particles(time_step, interaction);
    update();
}

void ParticleSystem::set_particles_influence_radius(float _particle_influence_radius) {
    *particle_influence_radius = _particle_influence_radius;
    grid->change_grid(QPoint(world_size.width() / *particle_influence_radius,
                             world_size.height() / *particle_influence_radius));
}

void ParticleSystem::paintEvent(QPaintEvent* e) {
    QPainter p(this);

    // draw the background
    p.setBrush(QBrush(Qt::black));
    p.drawRect(0, 0, this->width(), this->height());

    // draw the particles
    p.setBrush(QBrush(Qt::red));
    int particle_draw_radius = particle_radius * im_size.width() / world_size.width();
    for (auto particle : particles) {
        p.drawEllipse(world_to_screen(particle->get_pos()), particle_draw_radius, particle_draw_radius);
    }

    // draw the interaction circle
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::white));
    int inter_radius = interaction.radius * im_size.width() / world_size.width();
    p.drawEllipse(world_to_screen(interaction.pos), inter_radius, inter_radius);
}

void ParticleSystem::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        interaction.pos = screen_to_world(event->pos());
        interaction.radius = interaction_radius;
        interaction.strength = -interaction_strength;
    }
    else if (event->button() == Qt::RightButton) {
        interaction.pos = screen_to_world(event->pos());
        interaction.radius = interaction_radius;
        interaction.strength = interaction_strength;
    }
}

void ParticleSystem::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        interaction.radius = 0;
        interaction.strength = 0;
    }
}

void ParticleSystem::mouseMoveEvent(QMouseEvent *event) {
    interaction.pos = screen_to_world(event->pos());
}
