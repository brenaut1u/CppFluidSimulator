#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <QOpenGLWidget>
#include <QPoint>
#include <QSize>
#include <QSizeF>
#include <QVector2D>
#include "grid.h"

inline constexpr float g = 9.81;
inline constexpr QVector2D gravity = QVector2D(0.0, -g);

class ParticleSystem : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit ParticleSystem(int _nb_particles, double particle_size, QSize _im_size, QSizeF _world_size, double _time_step, QWidget *parent = nullptr);
    void update_physics();
    void paintEvent(QPaintEvent *e) override;

signals:

private:
    QPoint world_to_screen(QPointF world_pos);

private:
    Grid grid;
    int nb_particles;
    QSize im_size;
    QSizeF world_size;
    QImage image;
    double time_step;
};

#endif // PARTICLESYSTEM_H
