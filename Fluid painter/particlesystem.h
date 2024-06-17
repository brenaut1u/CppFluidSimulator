#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <QOpenGLWidget>
#include <QPoint>
#include <QSize>
#include <QSizeF>
#include <QImage>
#include <QRectF>
#include <QString>
#include <memory>
#include "grid.h"
#include "interaction.h"

using std::shared_ptr;
using std::unique_ptr;

class ParticleSystem : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit ParticleSystem(int _nb_particles, float _particle_radius, float _particle_influence_radius, const QSize& _im_size,
                                            QSizeF _world_size, float _time_step, float _g, float _collision_damping, float _fluid_density,
                                            float _pressure_multiplier, float _near_pressure_multiplier, float _viscosity_multiplier,
                                            float _interaction_radius, float _interaction_strength,
                                            QColor _particle_default_color, QWidget *parent = nullptr);

    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void create_particles();

    void reset_particles();
    void reset_colors_and_image();

    void start_preview();
    void stop_preview();

    void set_nb_particles(int _nb_particles) {nb_particles = _nb_particles;
                                             colors = QVector<QColor>(nb_particles, particle_default_color);}
    void set_particles_radius(float _particle_radius) {*particle_radius = _particle_radius;}
    void set_particles_influence_radius(float _particle_influence_radius);
    void set_g(float _g) {*g = _g;}
    void set_fluid_density(float _fluid_density) {*fluid_density = _fluid_density;}
    void set_pressure_multiplier(float _pressure_multiplier) {*pressure_multiplier = _pressure_multiplier;}
    void set_near_pressure_multiplier(float _near_pressure_multiplier) {*near_pressure_multiplier = _near_pressure_multiplier;}
    void set_viscosity_multiplier(float _viscosity_multiplier) {*viscosity_multiplier = _viscosity_multiplier;}
    void set_interaction_radius(float _interaction_radius) {interaction_radius = _interaction_radius;}
    void set_interaction_strength(float _interaction_strength) {interaction_strength = _interaction_strength;}
    void set_collision_damping(float _collision_damping) {*collision_damping = _collision_damping;}

    void update_physics();

    int get_end_frame() const {return end_frame;}

    void set_image(QString filename);

public slots:
    void update_view();

private:
    QPoint world_to_screen(QPointF world_pos) {
        return QPoint(world_pos.x() * im_size.width() / world_size.width(),
                      im_size.height() - world_pos.y() * im_size.height() / world_size.height());
    }

    QPointF screen_to_world(QPoint screen_pos) {
        return QPointF(screen_pos.x() * world_size.width() / im_size.width(),
                       (im_size.height() - screen_pos.y()) * world_size.height() / im_size.height());
    }

    void set_particles_colors_image();

private:
    int nb_particles;
    shared_ptr<float> particle_radius;
    shared_ptr<float> particle_influence_radius;
    shared_ptr<float> g;
    shared_ptr<float> collision_damping;
    shared_ptr<float> fluid_density;
    shared_ptr<float> pressure_multiplier;
    shared_ptr<float> near_pressure_multiplier;
    shared_ptr<float> viscosity_multiplier;
    float time_step;

    QSize im_size;
    QSizeF world_size;
    shared_ptr<Grid> grid;
    QVector<shared_ptr<Particle>> particles;

    QColor particle_default_color;

    Interaction interaction;
    float interaction_radius = 0;
    float interaction_strength = 0;

    bool playing = false;
    int frame = 0;
    int end_frame = -1;

    QVector<QColor> colors;
    unique_ptr<QImage> image = nullptr;
    QRectF image_rec;
    QString move_rec_mode = "";
    QPoint click_pos;
    QPointF click_image_rec_pos; // image_rec position at click time (used to move the rec)
};

#endif // PARTICLESYSTEM_H
