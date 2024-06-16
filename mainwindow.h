#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "particlesystem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void set_gravity(int val);
    void set_pressure(int val);
    void set_near_pressure(int val);
    void set_viscosity(int val);
    void set_influence_radius(int val);
    void set_density(int val);
    void set_interaction_radius(int val);
    void set_interaction_strength(int val);
    void set_collision_damping(int val);

private:
    Ui::MainWindow* ui;
    ParticleSystem* particle_system;
};
#endif // MAINWINDOW_H
