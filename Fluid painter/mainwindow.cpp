#include <QTimer>
#include <QtMath>
#include <QString>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

inline const QSize window_size = QSize(1000, 800);
inline const QSizeF world_size =  QSizeF(10.0, 8.0);

inline constexpr float init_nb_particles = 10000;
inline constexpr float time_step = 0.01;
inline constexpr float init_g = 12;
inline constexpr float init_pressure_multiplier = 135;
inline constexpr float init_near_pressure_multiplier = 8.0;
inline constexpr float init_viscosity_multiplier = 8100;
inline constexpr float init_fluid_density = 120.0;
inline constexpr float init_collision_damping = 0.15;
inline constexpr float init_particle_radius = 0.03;
inline constexpr float init_particle_influence_radius = 0.25;
inline constexpr float init_interaction_radius = 1.0;
inline constexpr float init_interaction_strength = 50.0;
inline const QColor init_default_color = Qt::white;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    particle_system = new ParticleSystem(init_nb_particles,
                                         init_particle_radius,
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
                                         init_default_color,
                                         this);

    ui->mainLayout->addWidget(particle_system);
    particle_system->setFocus();
    resize(1500, 800);

    QObject::connect(ui->NumberParticlesSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_nb_particles);
    QObject::connect(ui->RadiusSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_radius);
    QObject::connect(ui->GravitySlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_gravity);
    QObject::connect(ui->PressureSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_pressure);
    QObject::connect(ui->NearPressureSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_near_pressure);
    QObject::connect(ui->ViscositySlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_viscosity);
    QObject::connect(ui->InfluenceRadiusSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_influence_radius);
    QObject::connect(ui->DensitySlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_density);
    QObject::connect(ui->CollisionDampinglSlider, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::set_collision_damping);

    QObject::connect(ui->ButtonPreview, &QPushButton::clicked, this, &MainWindow::start_preview);
    QObject::connect(ui->ButtonStopPreview, &QPushButton::clicked, this, &MainWindow::stop_preview);
    QObject::connect(ui->ButtonOpenImage, &QPushButton::clicked, this, &MainWindow::set_image);

    ui->labelNumberParticlesValue->setNum(init_nb_particles);
    ui->labelRadiusValue->setNum(init_particle_radius);
    ui->labelGravityValue->setNum(init_g);
    ui->labelPressureValue->setNum(init_pressure_multiplier);
    ui->labelNearPressureValue->setNum(init_near_pressure_multiplier);
    ui->labelViscosityValue->setNum(init_viscosity_multiplier);
    ui->labelInfluenceRadiusValue->setNum(init_particle_influence_radius);
    ui->labelDensityValue->setNum(init_fluid_density);
    ui->labelCollisionDampingValue->setNum(init_collision_damping);

    ui->NumberParticlesSlider->setValue(init_nb_particles);
    ui->RadiusSlider->setValue(500 * init_particle_radius);
    ui->GravitySlider->setValue(init_g * 5);
    ui->PressureSlider->setValue(10 * qLn(init_pressure_multiplier + 1));
    ui->NearPressureSlider->setValue(10 * qLn(init_near_pressure_multiplier + 1));
    ui->ViscositySlider->setValue(10 * qLn(init_viscosity_multiplier / 10 + 1));
    ui->InfluenceRadiusSlider->setValue(init_particle_influence_radius * 20);
    ui->DensitySlider->setValue(10 * qLn(init_fluid_density + 1));
    ui->CollisionDampinglSlider->setValue(100 * init_collision_damping);

    ui->ButtonStopPreview->setDisabled(true);
    ui->ButtonOpenImage->setDisabled(true);
    ui->ButtonPlay->setDisabled(true);

    auto timer = new QTimer(parent);
    QObject::connect(timer, SIGNAL(timeout()), particle_system, SLOT(update_view()));
    timer->start(2);
}

void MainWindow::set_nb_particles(int val) {
    particle_system->set_nb_particles(val);
    ui->labelNumberParticlesValue->setNum(val);
}

void MainWindow::set_radius(int val) {
    float radius = val / 500.0;
    particle_system->set_particles_radius(radius);
    ui->labelRadiusValue->setNum(radius);
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
    float viscosity_multiplier = (qExp(val / 10.0) - 1) * 10;
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

void MainWindow::set_collision_damping(int val) {
    float collision_damping = val / 100.0;
    particle_system->set_collision_damping(collision_damping);
    ui->labelCollisionDampingValue->setNum(collision_damping);
}

void MainWindow::start_preview() {
    ui->ButtonPreview->setDisabled(true);
    ui->ButtonStopPreview->setDisabled(false);
    ui->ButtonOpenImage->setDisabled(true);
    ui->ButtonPlay->setDisabled(true);
    ui->labelNumberFrames->setText("End frame:");
    particle_system->start_preview();
}

void MainWindow::stop_preview() {
    ui->ButtonPreview->setDisabled(false);
    ui->ButtonStopPreview->setDisabled(true);
    ui->ButtonOpenImage->setDisabled(false);
    ui->ButtonPlay->setDisabled(false);
    particle_system->stop_preview();
    ui->labelNumberFrames->setText(QString("End frame: %1").arg(particle_system->get_end_frame()));
}

void MainWindow::set_image() {
    QString filename = QFileDialog::getOpenFileName(this,
             tr("Open File"), "/home", tr("Images (*.png *.jpg)"));
    particle_system->set_image(filename);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete particle_system;
}

