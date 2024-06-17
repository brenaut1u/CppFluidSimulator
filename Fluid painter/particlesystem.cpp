#include <memory>
#include <QtMath>
#include <QMouseEvent>
#include <QColor>
#include <QFile>
#include "qpainter.h"
#include "particlesystem.h"

#include <QDebug>

inline constexpr float particles_init_spacing = 2.5;
inline constexpr float particles_init_speed = 5;
inline constexpr float image_default_scale = 0.75;
inline const QColor image_rec_color = QColor(255, 255, 255, 100);
inline const QColor image_rec_border_color = Qt::white;
inline constexpr float image_rec_border_thickness = 5;

using std::make_shared;
using std::shared_ptr;
using std::make_unique;

ParticleSystem::ParticleSystem(int _nb_particles, float _particle_radius, float _particle_influence_radius, const QSize& _im_size,
                               QSizeF _world_size, float _time_step, float _g, float _collision_damping, float _fluid_density,
                               float _pressure_multiplier, float _near_pressure_multiplier, float _viscosity_multiplier,
                               float _interaction_radius, float _interaction_strength,
                               QColor _particle_default_color, QWidget *parent) :
         QOpenGLWidget(parent), nb_particles(_nb_particles), time_step(_time_step),
         im_size(_im_size), world_size(_world_size), interaction_radius(_interaction_radius), interaction_strength(_interaction_strength),
         particle_default_color(_particle_default_color)
{
    particle_radius = make_shared<float>(_particle_radius);
    particle_influence_radius = make_shared<float>(_particle_influence_radius);
    g = make_shared<float>(_g);
    collision_damping = make_shared<float>(_collision_damping);
    fluid_density = make_shared<float>(_fluid_density);
    pressure_multiplier = make_shared<float>(_pressure_multiplier);
    near_pressure_multiplier = make_shared<float>(_near_pressure_multiplier);
    viscosity_multiplier = make_shared<float>(_viscosity_multiplier);

    this->setMinimumSize(im_size.width(), im_size.height());

    interaction = {{0, 0}, 0, 0};

    particles = QVector<shared_ptr<Particle>>();

    grid = make_shared<Grid>(QPoint(world_size.width() / *particle_influence_radius,
                                    world_size.height() / *particle_influence_radius),
                             world_size,
                             g,
                             collision_damping,
                             fluid_density,
                             pressure_multiplier,
                             near_pressure_multiplier,
                             viscosity_multiplier);

    colors = QVector<QColor>(nb_particles, particle_default_color);
}

void ParticleSystem::update_view() {
    if (playing) {
        update_physics();
        if (frame == end_frame) {
            emit animation_done();
            frame = 0;
            playing = false;
            recording = false;
            video_writer->close();
        }
    }
}

void ParticleSystem::update_physics() {
    if (end_frame == -1 || frame < end_frame) {
        if (particles.size() < nb_particles && frame % int(particles_init_speed * *particle_influence_radius) == 0) {
            create_particles();
        }

        grid->update_particles(time_step, interaction);
        update();

        frame++;
    }
}

void ParticleSystem::start_preview() {
    reset_particles();
    reset_colors_and_image();
    end_frame = -1;
    frame = 0;
    playing = true;
    video_writer = make_unique<QAviWriter>("demo.avi", im_size, 24, "MJPG");
    video_writer->open();
}

void ParticleSystem::stop_preview() {
    playing = false;
    end_frame = frame;
}

void ParticleSystem::start_animation() {
    reset_particles();
    frame = 0;
    playing = true;
    recording = true;
}

void ParticleSystem::create_particles() {
    int n = (world_size.height() / 10.0) / (particles_init_spacing * *particle_radius);

    for (int j = 0; j < n && particles.size() <= nb_particles - 2; j++) {
        shared_ptr<Particle> particle_left = make_shared<Particle>(particle_radius, particle_influence_radius,
                                                              QPointF(*particle_radius, world_size.height() - (j + 1) * (particles_init_spacing * *particle_radius)),
                                                              QVector2D(particles_init_speed, 0.0),
                                                              colors.at(particles.size()), grid);
        grid->add_particle(particle_left);
        particles.append(particle_left);

        shared_ptr<Particle> particle_right = make_shared<Particle>(particle_radius, particle_influence_radius,
                                                              QPointF(world_size.width() - *particle_radius, world_size.height() - (j + 1) * (particles_init_spacing * *particle_radius)),
                                                              QVector2D(-particles_init_speed, 0.0),
                                                              colors.at(particles.size()), grid);

        grid->add_particle(particle_right);
        particles.append(particle_right);
    }
}

void ParticleSystem::reset_particles() {
    particles = QVector<shared_ptr<Particle>>();

    grid = make_shared<Grid>(QPoint(world_size.width() / *particle_influence_radius,
                                    world_size.height() / *particle_influence_radius),
                             world_size,
                             g,
                             collision_damping,
                             fluid_density,
                             pressure_multiplier,
                             near_pressure_multiplier,
                             viscosity_multiplier);
}

void ParticleSystem::reset_colors_and_image() {
    colors = QVector<QColor>(nb_particles, particle_default_color);
    image = nullptr;
}

void ParticleSystem::set_particles_influence_radius(float _particle_influence_radius) {
    *particle_influence_radius = _particle_influence_radius;
    grid->change_grid(QPoint(world_size.width() / *particle_influence_radius,
                             world_size.height() / *particle_influence_radius));
}

void ParticleSystem::set_image(QString filename) {
    if (QFile::exists(filename)) {
        image = make_unique<QImage>(filename);

        float im_width;
        float im_height;
        if (image->size().width() > image->size().height()) {
            im_width = image_default_scale * im_size.width();
            im_height = im_width * ((float) image->size().height() / image->size().width());
        }
        else {
            im_height = image_default_scale * im_size.height();
            im_width = im_height * ((float) image->size().width() / image->size().height());
        }

        image_rec = QRectF(im_size.width() / 2 - im_width / 2,
                           im_size.height() / 2 - im_height / 2,
                           im_width,
                           im_height);
    }
}

void ParticleSystem::set_particles_colors_image() {
    for (auto particle : particles) {
        QPoint pos = world_to_screen(particle->get_pos());

        // could'nt use image_rec.contains(pos) because it includes the borders
        if (image != nullptr
            && pos.x() > image_rec.x() && pos.x() < image_rec.x() + image_rec.width()
            && pos.y() > image_rec.y() && pos.y() < image_rec.y() + image_rec.height()) {

            QPoint pixel_pos = pos - image_rec.topLeft().toPoint();
            pixel_pos.setX(pixel_pos.x() * image->width() / image_rec.width());
            pixel_pos.setY(pixel_pos.y() * image->height() / image_rec.height());

            QColor c = image->pixelColor(pixel_pos);
            particle->set_color(c);
            colors[particle->get_id()] = c;
        }
        else {
            particle->set_color(particle_default_color);
            colors[particle->get_id()] = particle_default_color;
        }
    }
}

void ParticleSystem::paintEvent(QPaintEvent* e) {
    //QPainter p(this);

    QPixmap pixmap(im_size);
    QPainter p(&pixmap);

    // draw the background
    p.setBrush(QBrush(Qt::black));
    p.drawRect(0, 0, this->width(), this->height());

    // draw the image rectangle
    if (image != nullptr && !playing) {
        //p.drawImage(image_rec, *image);
        p.setBrush(QBrush(image_rec_color));
        p.setPen(QPen(image_rec_border_color, image_rec_border_thickness));
        p.drawRect(image_rec);
        set_particles_colors_image();
    }

    // draw the particles
    int particle_draw_radius = *particle_radius * im_size.width() / world_size.width();
    for (auto particle : particles) {
        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(particle->get_color()));
        p.drawEllipse(world_to_screen(particle->get_pos()), particle_draw_radius, particle_draw_radius);
    }

    QImage im = pixmap.toImage();
    QPainter(this).drawImage(QPoint(0, 0), im);

    if (recording) {
        video_writer->addFrame(im);
    }
}

void ParticleSystem::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (image != nullptr && !playing) {
            QPoint pos = event->pos();
            if (qAbs(pos.y() - image_rec.y()) <= image_rec_border_thickness) {
                if (qAbs(pos.x() - image_rec.x()) <= image_rec_border_thickness) {
                    move_rec_mode = "top left corner";
                }
                else if (qAbs(pos.x() - (image_rec.x() + image_rec.width())) <= image_rec_border_thickness) {
                    move_rec_mode = "top right corner";
                }
                else if (pos.x() >= image_rec.x() && pos.x() <= image_rec.x() + image_rec.width()) {
                    move_rec_mode = "top border";
                }
            }
            else if (qAbs(pos.y() - (image_rec.y() + image_rec.height())) <= image_rec_border_thickness) {
                if (qAbs(pos.x() - image_rec.x()) <= image_rec_border_thickness) {
                    move_rec_mode = "bottom left corner";
                }
                else if (qAbs(pos.x() - (image_rec.x() + image_rec.width())) <= image_rec_border_thickness) {
                    move_rec_mode = "bottom right corner";
                }
                else if (pos.x() >= image_rec.x() && pos.x() <= image_rec.x() + image_rec.width()) {
                    move_rec_mode = "bottom border";
                }
            }
            else if (pos.y() >= image_rec.y() && pos.y() <= image_rec.y() + image_rec.height()) {
                if (qAbs(pos.x() - image_rec.x()) <= image_rec_border_thickness) {
                    move_rec_mode = "left border";
                }
                else if (qAbs(pos.x() - (image_rec.x() + image_rec.width())) <= image_rec_border_thickness) {
                    move_rec_mode = "right border";
                }
                else if (pos.x() >= image_rec.x() && pos.x() <= image_rec.x() + image_rec.width()) {
                    move_rec_mode = "whole rec";
                }
            }

            click_pos = pos;
            click_image_rec_pos = image_rec.topLeft();
        }
    }
}

void ParticleSystem::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        move_rec_mode = "";
    }
}

void ParticleSystem::mouseMoveEvent(QMouseEvent *event) {
    if (move_rec_mode == "whole rec") {
        QPoint delta_pos = event->pos() - click_pos;
        image_rec.moveTo(click_image_rec_pos + delta_pos);
    }
    else if (move_rec_mode == "top border") image_rec.setTop(event->pos().y());

    else if (move_rec_mode == "right border") image_rec.setRight(event->pos().x());
    else if (move_rec_mode == "bottom border") image_rec.setBottom(event->pos().y());
    else if (move_rec_mode == "left border") image_rec.setLeft(event->pos().x());
    else if (move_rec_mode == "top left corner") image_rec.setTopLeft(event->pos());
    else if (move_rec_mode == "top right corner") image_rec.setTopRight(event->pos());
    else if (move_rec_mode == "bottom right corner") image_rec.setBottomRight(event->pos());
    else if (move_rec_mode == "bottom left corner") image_rec.setBottomLeft(event->pos());
    update();
}
