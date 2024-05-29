#include "particlesystem.h"

ParticleSystem::ParticleSystem(int _nb_particles, double particle_size, QSize _im_size, QSizeF _world_size, double _time_step, QWidget *parent) :
    nb_particles(_nb_particles), im_size(_im_size), world_size(_world_size), time_step(_time_step)
{

}

void ParticleSystem::update_physics() {

}

void ParticleSystem::paintEvent(QPaintEvent *e) {

}

QPoint ParticleSystem::world_to_screen(QPointF world_pos) {

}
