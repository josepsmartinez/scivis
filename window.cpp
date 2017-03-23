// window.cpp

#include <QtWidgets>
#include "window.h"
#include "ui_window.h"
#include "myglwidget.h"

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
}

Window::~Window()
{
    delete ui;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}


void Window::on_simulationTimestepSlider_valueChanged(int value)
{
    ui->myGLWidget->timestep(value);
}

void Window::on_scalarColoring_activated(const QString &arg1)
{
    ui->myGLWidget->scalarColoring(arg1);
}

void Window::on_hedgehogScalingSlider_valueChanged(int value)
{
    ui->myGLWidget->hedgehogScaling(value);
}

void Window::on_freezeCheckbox_clicked(bool checked)
{
    ui->myGLWidget->showAnimation(checked);
}

void Window::on_fluidViscositySlider_valueChanged(int value)
{
    ui->myGLWidget->fluidViscosity(value);
}

void Window::on_drawingMatterCheckbox_clicked(bool checked)
{
    ui->myGLWidget->drawMatter(checked);
}

void Window::on_drawingHedgehogsCheckbox_clicked(bool checked)
{
    ui->myGLWidget->drawHedgehogs(checked);
}

void Window::on_hedgehogColorDir_clicked(bool checked)
{
    ui->myGLWidget->colorDir(checked);
}

void Window::on_Dataset_activated(const QString &arg1)
{
    ui->myGLWidget->changeData(arg1);
}


void Window::on_scale_clicked(bool checked)
{
    ui->myGLWidget->setClamp(!checked);
}

void Window::on_clamp_clicked(bool checked)
{
    ui->myGLWidget->setClamp(checked);
}

void Window::on_clamp_min_valueChanged(double arg1)
{
    ui->myGLWidget->setClampMin(arg1);
    if(ui->clamp_max->value()<arg1)
    {
        ui->clamp_max->setValue(arg1);
        ui->myGLWidget->setClampMax(arg1);
    }
}

void Window::on_clamp_max_valueChanged(double arg1)
{
    ui->myGLWidget->setClampMax(arg1);
    if(ui->clamp_min->value()>arg1)
    {
        ui->clamp_min->setValue(arg1);
        ui->myGLWidget->setClampMin(arg1);
    }
}

void Window::on_HedgehogType_activated(const QString &arg1)
{
    ui->myGLWidget->hedgehogType(arg1);
}
