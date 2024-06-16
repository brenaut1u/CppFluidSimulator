#include <QTimer>
#include <QtMath>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

inline constexpr float nb_particles = 4000;
inline const QSize window_size = QSize(1000, 800);
inline const QSizeF world_size =  QSizeF(10.0, 8.0);
inline constexpr float time_step = 0.01;
inline constexpr float init_g = 0.0;
inline constexpr float init_pressure_multiplier = 0.0;
inline constexpr float init_near_pressure_multiplier = 0.0;
inline constexpr float init_viscosity_multiplier = 0.0;
inline constexpr float init_fluid_density = 20.0;
inline constexpr float init_collision_damping = 0.15;
inline constexpr float particle_radius = 0.03;
inline constexpr float init_particle_influence_radius = 0.25;
inline constexpr float init_interaction_radius = 1.0;
inline constexpr float init_interaction_strength = 50.0;

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
                                         init_near_pressure_multiplier,
                                         init_viscosity_multiplier,
                                         init_interaction_radius,
                                         init_interaction_strength,
                                         this);

    ui->mainLayout->addWidget(particle_system);
    particle_system->setFocus();
    resize(1000, 800);

    QObject::connect(ui->GravitySlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_gravity);
    QObject::connect(ui->PressureSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_pressure);
    QObject::connect(ui->NearPressureSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_near_pressure);
    QObject::connect(ui->ViscositySlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_viscosity);
    QObject::connect(ui->InfluenceRadiusSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_influence_radius);
    QObject::connect(ui->DensitySlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_density);
    QObject::connect(ui->InteractionRadiusSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_interaction_radius);
    QObject::connect(ui->InteractionStrengthSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_interaction_strength);
    QObject::connect(ui->CollisionDampinglSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_collision_damping);

    ui->labelGravityValue->setNum(init_g);
    ui->labelPressureValue->setNum(init_pressure_multiplier);
    ui->labelNearPressureValue->setNum(init_near_pressure_multiplier);
    ui->labelViscosityValue->setNum(init_viscosity_multiplier);
    ui->labelInfluenceRadiusValue->setNum(init_particle_influence_radius);
    ui->labelDensityValue->setNum(init_fluid_density);
    ui->labelInteractionRadiusValue->setNum(init_interaction_radius);
    ui->labelInteractionStrengthValue->setNum(init_interaction_strength);
    ui->labelCollisionDampingValue->setNum(init_collision_damping);

    ui->GravitySlider->setValue(init_g * 5);
    ui->PressureSlider->setValue(10 * qLn(init_pressure_multiplier + 1));
    ui->NearPressureSlider->setValue(10 * qLn(init_near_pressure_multiplier + 1));
    ui->ViscositySlider->setValue(10 * qLn(init_viscosity_multiplier + 1));
    ui->InfluenceRadiusSlider->setValue(init_particle_influence_radius * 20);
    ui->DensitySlider->setValue(10 * qLn(init_fluid_density + 1));
    ui->InteractionRadiusSlider->setValue(init_interaction_radius * 20);
    ui->InteractionStrengthSlider->setValue(10 * qLn(init_interaction_strength + 1));
    ui->CollisionDampinglSlider->setValue(100 * init_collision_damping);

    auto timer = new QTimer(parent);
    QObject::connect(timer, SIGNAL(timeout()), particle_system, SLOT(update_physics()));
    timer->start(2);
}

void MainWindow::set_gravity(int val) {
    float g = val / 5.0;
    particle_system->set_g(g);
    ui->labelGravityValue->setNum(g);
}

void MainWindow::set_pressure(int val) {
    float pressure_multiplier = qExp(val / 10.0) - 1;
    particle_system->set_pressure_multiplier(pressure_multiplier);
    ui->labelPressureValue->setNum(pressure_multiplier);
}

void MainWindow::set_near_pressure(int val) {
    float near_pressure_multiplier = qExp(val / 10.0) - 1;
    particle_system->set_near_pressure_multiplier(near_pressure_multiplier);
    ui->labelNearPressureValue->setNum(near_pressure_multiplier);
}

void MainWindow::set_viscosity(int val) {
    float viscosity_multiplier = qExp(val / 10.0) - 1;
    particle_system->set_viscosity_multiplier(viscosity_multiplier);
    ui->labelViscosityValue->setNum(viscosity_multiplier);
}

void MainWindow::set_influence_radius(int val) {
    float influence_radius = val / 20.0;
    particle_system->set_particles_influence_radius(influence_radius);
    ui->labelInfluenceRadiusValue->setNum(influence_radius);
}

void MainWindow::set_density(int val) {
    float fluid_density = qExp(val / 10.0) - 1;
    particle_system->set_fluid_density(fluid_density);
    ui->labelDensityValue->setNum(fluid_density);
}

void MainWindow::set_interaction_radius(int val) {
    float interaction_radius = val / 20.0;
    particle_system->set_interaction_radius(interaction_radius);
    ui->labelInteractionRadiusValue->setNum(interaction_radius);
}

void MainWindow::set_interaction_strength(int val) {
    float interaction_strength = qExp(val / 10.0) - 1;
    particle_system->set_interaction_strength(interaction_strength);
    ui->labelInteractionStrengthValue->setNum(interaction_strength);
}

void MainWindow::set_collision_damping(int val) {
    float collision_damping = val / 100.0;
    particle_system->set_collision_damping(collision_damping);
    ui->labelCollisionDampingValue->setNum(collision_damping);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete particle_system;
}

