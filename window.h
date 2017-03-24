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

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
