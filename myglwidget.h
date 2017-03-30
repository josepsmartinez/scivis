// myglwidget.h

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <rfftw.h>              //the numerical simulation FFTW library
#include "simulation.h"
#include <vector>

const int DATA_DENSITY=0;   //possible data types
const int DATA_VELOCITY=1;
const int DATA_FORCEFIELD=2;

const int HEDGEHOG_LINE=0;   //possible hedgehog types
const int HEDGEHOG_CONE=1;
const int HEDGEHOG_ARROW=2;

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();
signals:

public slots:
    void do_one_simulation_step(bool update);
    void do_one_simulation_step();

    void scalarColoring(QString scalartype);

    void colorDir(bool color_dir_bool);

    void showAnimation(bool new_frozen);

    void drawMatter(bool);

    void drawHedgehogs(bool);

    void hedgehogType(QString new_hedgehog_type);

    void hedgehogScalar(int new_h_scalar);

    void hedgehogVector(int new_h_vector);

    void setClamp(bool new_clamp);

    void setClampMax(double);
    void setClampMin(double);

    void setNColors(int);

    void changeData(QString datatype);

    void timestep(int position);

    void hedgehogScaling(int position);

    void fluidViscosity(int position);

    void drawArrow(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn);

    void drawCone(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn);

    QColor color_legend(float value, fftw_real max, fftw_real min);

    float get_max();
    float get_min();


protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mouseMoveEvent(QMouseEvent *event);


signals:

private:
    QTimer timer;
    int   winWidth, winHeight;      //size of the graphics window, in pixels
    int   color_dir;            //set direction color-coding type
    float vec_scale;			//scaling of hedgehogs
    bool   draw_smoke;           //draw the smoke or not
    bool   draw_vecs;            //draw the vector field or not
    int hedgehog_type;
    int hedgehog_scalar;
    int hedgehog_vector;
    bool clamp;
    float clamp_max, clamp_min;
    float max,min;
    int n_colors;
    int data_type; // data used
    int   scalar_col;           //method for scalar coloring
    int DIM;
    Simulation simulation;
    fftw_real  cell_width;
    fftw_real  cell_height;

    Field* scalar_draw;
    Field* scalar_draw_hedgehog;
    vectorialField* vectorial_draw;
};

#endif // MYGLWIDGET_H
