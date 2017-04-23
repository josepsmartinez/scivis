// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QSlider>

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT


public:
    explicit Window(QWidget *parent = 0);
    ~Window();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:

    void on_simulationTimestepSlider_valueChanged(int value);

    void on_scalarColoring_activated(const QString &arg1);

    void on_hedgehogScalingSlider_valueChanged(int value);

    void on_freezeCheckbox_clicked(bool checked);

    void on_fluidViscositySlider_valueChanged(int value);

    void on_drawingMatterCheckbox_clicked(bool checked);

    void on_drawingHedgehogsCheckbox_clicked(bool checked);

    void on_hedgehogColorDir_clicked(bool checked);

    void on_Dataset_activated(const QString &arg1);

    void on_scale_clicked(bool checked);

    void on_clamp_clicked(bool checked);

    void on_clamp_min_valueChanged(double arg1);

    void on_clamp_max_valueChanged(double arg1);

    void on_HedgehogType_activated(const QString &arg1);

    void on_numberColors_valueChanged(int arg1);   

    void on_radio_hedv_velocity_clicked();

    void on_radio_hedv_forcefield_clicked();

    void on_radio_hed_density_clicked();

    void on_radio_hed_velocity_clicked();

    void on_radio_hed_forcefield_clicked();

    void update_color_legend();
    void update_color_legend_vals();

    void on_Col_hed_valueChanged(int arg1);

    void on_Row_hed_valueChanged(int arg1);

    void on_horizontalSlider_valueChanged(int value);

    void on_radio_hedv_grad_den_clicked();

    void on_radio_hedv_grad_vel_clicked();

    void on_huevalue_valueChanged(int value);

    void on_saturationvalue_valueChanged(int value);

    void on_resethuesat_clicked();

    void on_radio_never_streamline_clicked();

    void on_radio_frozen_streamline_clicked();

    void on_radio_always_streamline_clicked();

    void on_NumberofStreamLines_valueChanged(int arg1);

    void on_reposition_Streamlines_clicked();

    void on_separate_streamlines_clicked(bool checked);

    void on_streamline_lenght_valueChanged(int value);

    void on_checkBox_clicked(bool checked);

    void on_AlphaSlider_valueChanged(int value);

    void on_checkBox_2_clicked(bool checked);

    void on_rotation_valueChanged(int value);

    void on_slice_timestep_valueChanged(int value);


    void on_drawStreamsurface_clicked(bool checked);

public:
    Ui::Window *ui;
};

#endif // WINDOW_H
