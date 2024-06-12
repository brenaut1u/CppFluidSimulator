#include <memory>
#include "qpainter.h"
#include "particlesystem.h"

#include <QDebug>

using std::make_shared;
using std::shared_ptr;

ParticleSystem::ParticleSystem(int _nb_particles, float _particle_radius, float _particle_influence_radius, const QSize& _im_size, QSizeF _world_size, float _time_step, QWidget *parent) :
    QOpenGLWidget(parent), nb_particles(_nb_particles), particle_radius(_particle_radius), particle_influence_radius(_particle_influence_radius), im_size(_im_size), world_size(_world_size), time_step(_time_step)
{
    this->setMinimumSize(im_size.width(), im_size.height());

    grid = make_shared<Grid>(QPoint(10, 10), world_size);

    for (int i = 0; i < nb_particles / 4; i++) {
        for (int j = 0; j < 4; j++) {
            shared_ptr<Particle> particle = make_shared<Particle>(particle_radius, particle_influence_radius, QPointF(world_size.width() - i * particle_radius * 2, world_size.height() / 2.0 - j * particle_radius * 2), QVector2D(0.0, 0.0), grid);
            grid->add_particle(particle);
            particles.append(particle);
        }
    }
}

void ParticleSystem::update_physics() {
    grid->update_particles(time_step);
    update();
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
}
