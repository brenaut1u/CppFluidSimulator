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
    explicit ParticleSystem(int _nb_particles, float particle_radius, const QSize& _im_size, int _particle_draw_radius, QSizeF _world_size, float _time_step, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *e) override;

public slots:
    void update_physics();

private:
    QPoint world_to_screen(QPointF world_pos) {
        return QPoint(world_pos.x() * im_size.width() / world_size.width(),
                      im_size.height() -world_pos.y() * im_size.height() / world_size.height());
    }

private:
    shared_ptr<Grid> grid;
    int nb_particles;
    QSize im_size;
    int particle_draw_radius;
    QSizeF world_size;
    float time_step;
    QVector<shared_ptr<Particle>> particles;
    int counter;
};

#endif // PARTICLESYSTEM_H
