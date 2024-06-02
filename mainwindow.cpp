#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    particle_system = new ParticleSystem(1, 0.5, QSize(1000, 800), QSizeF(10.0, 8.0), 0.1, this);
    ui->mainLayout->addWidget(particle_system);
    particle_system->setFocus();
    resize(1000, 800);

    auto timer = new QTimer(parent);
    QObject::connect(timer, SIGNAL(timeout()), particle_system, SLOT(update_physics()));
    timer->start(20);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete particle_system;
}

