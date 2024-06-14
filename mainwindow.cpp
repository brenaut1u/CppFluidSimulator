#include <QTimer>
#include <QtMath>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

inline constexpr float nb_particles = 2000;
inline const QSize window_size = QSize(1000, 800);
inline const QSizeF world_size =  QSizeF(10.0, 8.0);
inline constexpr float time_step = 0.01;
inline constexpr float init_g = 0.0;
inline constexpr float init_pressure_multiplier = 0.0;
inline constexpr float init_fluid_density = 0.0;
inline constexpr float init_collision_damping = 0.85;
inline constexpr float particle_radius = 0.05;
inline constexpr float init_particle_influence_radius = 0.25;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    particle_system = new ParticleSystem(nb_particles,
                                         particle_radius,
                                         init_particle_influence_radius,
                                         window_size,
                                         world_size,
                                         time_step,
                                         init_g,
                                         init_collision_damping,
                                         init_fluid_density,
                                         init_pressure_multiplier,
                                         this);

    ui->mainLayout->addWidget(particle_system);
    particle_system->setFocus();
    resize(1000, 800);

    QObject::connect(ui->GravitySlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_gravity);
    QObject::connect(ui->PressureSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_pressure);
    QObject::connect(ui->InfluenceRadiusSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_influence_radius);
    QObject::connect(ui->DensitySlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_density);

    ui->labelGravityValue->setNum(init_g);
    ui->labelPressureValue->setNum(init_pressure_multiplier);
    ui->labelInfluenceRadiusValue->setNum(init_particle_influence_radius);
    ui->labelDensityValue->setNum(init_fluid_density);

    auto timer = new QTimer(parent);
    QObject::connect(timer, SIGNAL(timeout()), particle_system, SLOT(update_physics()));
    timer->start(2);
}

void MainWindow::set_gravity(int val) {
    float g = val / 20.0;
    particle_system->set_g(g);
    ui->labelGravityValue->setNum(g);
}

void MainWindow::set_pressure(int val) {
    float pressure_multiplier = qExp(val / 10.0) - 1;
    particle_system->set_pressure_multiplier(pressure_multiplier);
    ui->labelPressureValue->setNum(pressure_multiplier);
}

void MainWindow::set_influence_radius(int val) {
    float new_influence_radius = val / 20.0;
    particle_system->set_particles_influence_radius(new_influence_radius);
    ui->labelInfluenceRadiusValue->setNum(new_influence_radius);
}

void MainWindow::set_density(int val) {
    float fluid_density = qExp(val / 10.0) - 1;
    particle_system->set_fluid_density(fluid_density);
    ui->labelDensityValue->setNum(fluid_density);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete particle_system;
}

