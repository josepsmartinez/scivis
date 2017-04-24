// myglwidget.h

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <rfftw.h>              //the numerical simulation FFTW library
#include "simulation.h"
#include "streamline.h"
#include "timebuffer.h"
#include "streamsurface.h"
#include <vector>


const int DATA_DENSITY=0;   //possible data types
const int DATA_VELOCITY=1;
const int DATA_FORCEFIELD=2;
const int DATA_GRADIENT_DENSITY=3;
const int DATA_GRADIENT_VELOCITY=4;

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

    void slice_to_position(int plane);

    void drawSlices(bool);

    void drawHedgehogs(bool);

    void drawStreamSurface(bool);

    void setSeparateStreamlines(bool);

    void setInterpolation(bool);

    void setDataAlpha(bool);

    void setGrid(bool);

    void hedgehogType(QString new_hedgehog_type);

    void hedgehogScalar(int new_h_scalar);

    void hedgehogVector(int new_h_vector);

    void setClamp(bool new_clamp);

    void setClampMax(double);
    void setClampMin(double);

    void setHueShift(float);
    void setSaturationShift(float);
    void setAlpha(float);

    void setStreamlineLenght(int);

    void setNColors(int);

    void setROW(int);
    void setCOL(int);

    void setRandomness(int);

    void setnumStreamline(int);

    void setRotationAngle(int);

    void setnumSlices(int);

    void setSliceTimestep(int);

    void resetSimulation();

    void changeData(QString datatype);

    void timestep(int position);

    void hedgehogScaling(int position);

    void fluidViscosity(int position);

    void drawLine(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn, int plane);

    void drawArrow(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn, int plane);

    void drawCone(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn, QColor color, int plane);

    QColor color_legend(float value, fftw_real max, fftw_real min);

    void drawStreamlines(QString draw);

    void newStreamSurface();

    void Surfacenum(int);
    void SurfacetotalTimestep(int);
    void SurfaceBetweenTimestep(int);


    float get_max();
    float get_min();

    void update_jitter_matrix();

    void update_streamline_matrix();

    void setDT(fftw_real);
    void setx1(float);
    void setx2(float);
    void sety1(float);
    void sety2(float);





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
    bool draw_slices;
    bool draw_streamline;
    bool draw_streamsurface;
    bool draw_frozenstreamline;
    bool separate_streamlines;
    int streamline_lenght;
    bool interpolate;
    int hedgehog_type;
    int hedgehog_scalar;
    int hedgehog_vector;
    bool clamp;
    float clamp_max, clamp_min;
    float hues, sats;
    float max,min;
    int n_colors;
    int data_type; // data used
    int   scalar_col;           //method for scalar coloring
    int DIM;
    int ROW;
    int COL;
    float randomness;
    std::vector<std::vector<float>> jitter_j;
    std::vector<std::vector<float>> jitter_i;
    int numStreamline;
    Simulation simulation;
    fftw_real  cell_width;
    fftw_real  cell_height;


    float alpha;
    bool data_alpha;
    int rotation_angle;
    int num_slices;
    int timesteps_each_slice;
    int timestep_count;


    vector<StreamLine*> streamlines;
    Field* scalar_draw;
    Field* scalar_draw_hedgehog;
    vectorialField* vectorial_draw;

    TimeBuffer<Field> rho_buffer;
    vector<TimeBuffer<vectorialField>> vec_buffers;

    int numStreamSurface;
    int timesteps_surface;
    int timesteps_between_surface;
    bool surface_ready;
    fftw_real dt;
    float initial_x1, initial_x2, initial_y1, initial_y2;
    bool grid;


    StreamSurface* surfaceclass;
    std::vector<Polyline3D> streamsurface;
    std::vector<std::vector<QColor>> streamsurface_color;
};

#endif // MYGLWIDGET_H
