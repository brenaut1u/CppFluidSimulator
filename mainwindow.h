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
    void set_gravity(int val);
    void set_pressure(int val);
    void set_influence_radius(int val);
    void set_density(int val);

private:
    Ui::MainWindow* ui;
    ParticleSystem* particle_system;
};
#endif // MAINWINDOW_H
