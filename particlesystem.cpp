#include <memory>
#include "qpainter.h"
#include "particlesystem.h"

#include <QDebug>

using std::make_shared;
using std::shared_ptr;

ParticleSystem::ParticleSystem(int _nb_particles, float particle_radius, const QSize& _im_size, int _particle_draw_radius, QSizeF _world_size, float _time_step, QWidget *parent) :
    QOpenGLWidget(parent), nb_particles(_nb_particles), im_size(_im_size), particle_draw_radius(_particle_draw_radius), world_size(_world_size), time_step(_time_step)
{
    grid = make_shared<Grid>(QPoint(3, 3), world_size);

    shared_ptr<Particle> particle = make_shared<Particle>(particle_radius, QPointF(world_size.width(), world_size.height()) / 2.0, QVector2D(0.0, 0.0), grid);
    grid->add_particle(particle);
    particles.append(particle);
}

void ParticleSystem::update_physics() {
    grid->update_particles(time_step);
    update();
}

void ParticleSystem::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    p.setBrush(QBrush(Qt::black));
    p.drawRect(0, 0, this->width(), this->height());
    p.setBrush(QBrush(Qt::blue));
    for (auto particle : particles) {
        p.drawEllipse(world_to_screen(particle->get_pos()), particle_draw_radius, particle_draw_radius);
    }
}
