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
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_color_legend_vals()));
    timer->start(100);
    QTimer *timer2 = new QTimer(this);
    timer2->setSingleShot(true);
    connect(timer2, SIGNAL(timeout()), this, SLOT(update_color_legend()));
    timer2->start(250);
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
    update_color_legend();
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

void Window::on_numberColors_valueChanged(int arg1)
{
    ui->myGLWidget->setNColors(arg1);
    update_color_legend();
}


void Window::on_radio_hedv_velocity_clicked()
{
    ui->myGLWidget->hedgehogVector(DATA_VELOCITY);
}

void Window::on_radio_hedv_forcefield_clicked()
{
    ui->myGLWidget->hedgehogVector(DATA_FORCEFIELD);
}

void Window::on_radio_hed_density_clicked()
{
    ui->myGLWidget->hedgehogScalar(DATA_DENSITY);
}

void Window::on_radio_hed_velocity_clicked()
{
    ui->myGLWidget->hedgehogScalar(DATA_VELOCITY);
}

void Window::on_radio_hed_forcefield_clicked()
{
    ui->myGLWidget->hedgehogScalar(DATA_FORCEFIELD);
}

void Window::update_color_legend_vals()
{
    ui->min_legend->setText(QString::number(ui->myGLWidget->get_min()));
    ui->max_legend->setText(QString::number(ui->myGLWidget->get_max()));
}


void Window::update_color_legend()
{
    update_color_legend_vals();
    QImage legend(1,400,QImage::Format_ARGB32);
    for(int j = 0; j< legend.height(); j++)
    {
        QColor color(ui->myGLWidget->color_legend(j,399,0));
        legend.setPixel(0,399-j,color.rgba());
    }

    QPixmap pix = QPixmap::fromImage(legend);
    int w = ui->legend->width();
    int h = ui->legend->height();
    ui->legend->setPixmap(pix.scaled(w,h));
}

void Window::on_Col_hed_valueChanged(int arg1)
{
    ui->myGLWidget->setCOL(arg1);
}

void Window::on_Row_hed_valueChanged(int arg1)
{
    ui->myGLWidget->setROW(arg1);
}

void Window::on_horizontalSlider_valueChanged(int value)
{
    ui->myGLWidget->setRandomness(value);
}

void Window::on_radio_hedv_grad_den_clicked()
{
    ui->myGLWidget->hedgehogVector(DATA_GRADIENT_DENSITY);
}

void Window::on_radio_hedv_grad_vel_clicked()
{
    ui->myGLWidget->hedgehogVector(DATA_GRADIENT_VELOCITY);
}

void Window::on_huevalue_valueChanged(int value)
{
    update_color_legend();
    ui->myGLWidget->setHueShift((float)value/ui->huevalue->maximum());
}

void Window::on_saturationvalue_valueChanged(int value)
{
    update_color_legend();
    ui->myGLWidget->setSaturationShift(2*((float)value/ui->saturationvalue->maximum()) -1);
}

void Window::on_resethuesat_clicked()
{
    ui->huevalue->setValue(0);
    ui->saturationvalue->setValue(ui->saturationvalue->maximum()/2);
}

void Window::on_radio_never_streamline_clicked()
{
    ui->myGLWidget->drawStreamlines("Never");
}

void Window::on_radio_frozen_streamline_clicked()
{
    ui->myGLWidget->drawStreamlines("Frozen");
}

void Window::on_radio_always_streamline_clicked()
{
    ui->myGLWidget->drawStreamlines("Always");
}

void Window::on_NumberofStreamLines_valueChanged(int arg1)
{
    ui->myGLWidget->setnumStreamline(arg1);
}

void Window::on_reposition_Streamlines_clicked()
{
    ui->myGLWidget->update_streamline_matrix();
}

void Window::on_separate_streamlines_clicked(bool checked)
{
    ui->myGLWidget->setSeparateStreamlines(checked);
}

void Window::on_streamline_lenght_valueChanged(int value)
{
    ui->myGLWidget->setStreamlineLenght(value);
}

void Window::on_checkBox_clicked(bool checked)
{
    ui->myGLWidget->drawSlices(checked);
}

void Window::on_AlphaSlider_valueChanged(int value)
{
    ui->myGLWidget->setAlpha((float)value/(float)ui->AlphaSlider->maximum());
}

void Window::on_checkBox_2_clicked(bool checked)
{
    ui->myGLWidget->setDataAlpha(checked);
}

void Window::on_rotation_valueChanged(int value)
{
    ui->myGLWidget->setRotationAngle(value);
}

void Window::on_slice_timestep_valueChanged(int value)
{
    ui->myGLWidget->setSliceTimestep(value);
}


void Window::on_drawStreamsurface_clicked(bool checked)
{
    ui->myGLWidget->drawStreamSurface(checked);
}

void Window::on_draw_new_streamline_clicked()
{
    ui->drawStreamsurface->setChecked(true);
    ui->myGLWidget->drawStreamSurface(true);
    ui->myGLWidget->newStreamSurface();
}

void Window::on_numStreamSurface_valueChanged(int value)
{
    ui->myGLWidget->Surfacenum(value);
}

void Window::on_totalStreamsurfaceTimestep_valueChanged(int value)
{
    ui->myGLWidget->SurfacetotalTimestep(value);
}

void Window::on_timestepBetweensurfaces_valueChanged(int value)
{
    ui->myGLWidget->SurfaceBetweenTimestep(value);
}

void Window::on_sizestepsurface_valueChanged(int value)
{
    ui->myGLWidget->setDT(2*(float)value/(float)ui->sizestepsurface->maximum());
}

void Window::on_x1_valueChanged(int value)
{
    ui->myGLWidget->setx1((float)value/10.f);
}

void Window::on_y1_valueChanged(int value)
{
    ui->myGLWidget->sety1((float)value/10.f);
}

void Window::on_x2_valueChanged(int value)
{
    ui->myGLWidget->setx2((float)value/10.f);
}

void Window::on_y2_valueChanged(int value)
{
    ui->myGLWidget->sety2((float)value/10.f);
}
