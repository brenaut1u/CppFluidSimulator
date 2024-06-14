#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <QOpenGLWidget>
#include <QPoint>
#include <QSize>
#include <QSizeF>
#include <memory>
#include "grid.h"

using std::shared_ptr;

class ParticleSystem : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit ParticleSystem(int _nb_particles, float _particle_radius, float _particle_influence_radius, const QSize& _im_size,
                                            QSizeF _world_size, float _time_step, float _g, float _collision_damping, float _fluid_density,
                                            float _pressure_multiplier, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent* e) override;

public slots:
    void update_physics();
    void set_particles_influence_radius(float _particle_influence_radius);
    void set_g(float _g) {*g = _g;}
    void set_fluid_density(float _fluid_density) {*fluid_density = _fluid_density;}
    void set_pressure_multiplier(float _pressure_multiplier) {*pressure_multiplier = _pressure_multiplier;}

private:
    QPoint world_to_screen(QPointF world_pos) {
        return QPoint(world_pos.x() * im_size.width() / world_size.width(),
                      im_size.height() -world_pos.y() * im_size.height() / world_size.height());
    }

private:
    int nb_particles;
    float particle_radius;

    shared_ptr<float> particle_influence_radius;
    shared_ptr<float> g;
    shared_ptr<float> collision_damping;
    shared_ptr<float> fluid_density;
    shared_ptr<float> pressure_multiplier;
    float time_step;

    QSize im_size;
    QSizeF world_size;
    shared_ptr<Grid> grid;
    QVector<shared_ptr<Particle>> particles;
    int counter;
};

#endif // PARTICLESYSTEM_H
