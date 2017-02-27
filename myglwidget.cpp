// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>
#include <glut.h>          //the GLUT graphics library
#include "myglwidget.h"
#include "visualization.cpp"
#include <simulation.cpp>              //the numerical simulation FFTW library

MyGLWidget::MyGLWidget(QWidget *parent)
{
    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    color_dir = 1;
    vec_scale = 1000;			//scaling of hedgehogs
    draw_smoke = true;           //draw the smoke or not
    draw_vecs = true;            //draw the vector field or not
    scalar_col = 0;           //method for scalar coloring
    DIM = 50;
    simulation.init_simulation(DIM);
    QTimer *timer = new QTimer;
    timer->start(1);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(do_one_simulation_step()));
}

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::initializeGL()
{
    qglClearColor(Qt::black);
}


void MyGLWidget::paintGL() //glutDisplayFunc(display);
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_TABLE);
    glEnable(GL_SMOOTH);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glClearColor(0.0,0.0,0.0,0.0);

    glMatrixMode(GL_MODELVIEW);

    int        i, j, idx; double px,py;
    fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh
    if (draw_vecs)
    {
        glBegin(GL_LINES);				//draw velocities
        for (i = 0; i < DIM; i++)
          for (j = 0; j < DIM; j++)
          {
            idx = (j * DIM) + i;
            direction_to_color(simulation.get_v().x.read(idx),simulation.get_v().y.read(idx),color_dir);
            glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
            glVertex2f((wn + (fftw_real)i * wn) + vec_scale * simulation.get_v().x.read(idx),
                       (hn + (fftw_real)j * hn) + vec_scale * simulation.get_v().y.read(idx));
          }
        glEnd();
    }
    if (draw_smoke)
    {
        int idx0, idx1, idx2, idx3;
        double px0, py0, px1, py1, px2, py2, px3, py3;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLES);
        for (j = 0; j < DIM - 1; j++)            //draw smoke
        {
            for (i = 0; i < DIM - 1; i++)
            {
                px0 = wn + (fftw_real)i * wn;
                py0 = hn + (fftw_real)j * hn;
                idx0 = (j * DIM) + i;


                px1 = wn + (fftw_real)i * wn;
                py1 = hn + (fftw_real)(j + 1) * hn;
                idx1 = ((j + 1) * DIM) + i;


                px2 = wn + (fftw_real)(i + 1) * wn;
                py2 = hn + (fftw_real)(j + 1) * hn;
                idx2 = ((j + 1) * DIM) + (i + 1);


                px3 = wn + (fftw_real)(i + 1) * wn;
                py3 = hn + (fftw_real)j * hn;
                idx3 = (j * DIM) + (i + 1);


                /*
                set_colormap(simulation.get_v()[idx0],scalar_col,simulation.get_vmin(),simulation.get_vmax());    glVertex2f(px0, py0);
                set_colormap(simulation.get_v()[idx1],scalar_col,simulation.get_vmin(),simulation.get_vmax());    glVertex2f(px1, py1);
                set_colormap(simulation.get_v()[idx2],scalar_col,simulation.get_vmin(),simulation.get_vmax());    glVertex2f(px2, py2);


                set_colormap(simulation.get_v()[idx0],scalar_col,simulation.get_vmin(),simulation.get_vmax());    glVertex2f(px0, py0);
                set_colormap(simulation.get_v()[idx2],scalar_col,simulation.get_vmin(),simulation.get_vmax());    glVertex2f(px2, py2);
                set_colormap(simulation.get_v()[idx3],scalar_col,simulation.get_vmin(),simulation.get_vmax());    glVertex2f(px3, py3);
                */
                set_colormap(simulation.get_rho().read(idx0),scalar_col);    glVertex2f(px0, py0);
                set_colormap(simulation.get_rho().read(idx1),scalar_col);    glVertex2f(px1, py1);
                set_colormap(simulation.get_rho().read(idx2),scalar_col);    glVertex2f(px2, py2);


                set_colormap(simulation.get_rho().read(idx0),scalar_col);    glVertex2f(px0, py0);
                set_colormap(simulation.get_rho().read(idx2),scalar_col);    glVertex2f(px2, py2);
                set_colormap(simulation.get_rho().read(idx3),scalar_col);    glVertex2f(px3, py3);

            }
        }
        glEnd();
    }

    glFlush();
}

void MyGLWidget::resizeGL(int width, int height)
{
    glViewport(0.0f, 0.0f, (GLfloat)width, (GLfloat)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)width, 0.0, (GLdouble)height,0,50);
    winWidth = width; winHeight = height;
}


void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int mx = event->x();
    int my = event->y();
    simulation.drag(mx,my, DIM, winWidth, winHeight); // Works for Niek
}

void MyGLWidget::do_one_simulation_step(bool update)
{
    if (!simulation.get_frozen())
    {
        simulation.set_forces(DIM);
        simulation.solve(DIM, simulation.get_v().x.field, simulation.get_v().y.field, simulation.get_vx0(),
                         simulation.get_vy0(), simulation.get_visc(), simulation.get_dt());
        simulation.diffuse_matter(DIM, simulation.get_v().x.field, simulation.get_v().y.field,
                                  simulation.get_rho(), simulation.get_rho0(), simulation.get_dt());

    }
    if(update){
        updateGL();
    }
}

void MyGLWidget::do_one_simulation_step()
{
    do_one_simulation_step(true);
}

void MyGLWidget::showAnimation(bool new_frozen)
{
    // ! because if the checkbox = true, frozen should be set to false
    simulation.set_frozen(!new_frozen);
}

void MyGLWidget::drawMatter(bool new_draw_smoke)
{
    draw_smoke = new_draw_smoke;
}

void MyGLWidget::drawHedgehogs(bool new_draw_vecs)
{
    draw_vecs = new_draw_vecs;
}

void MyGLWidget::timestep(int position)
{
    // dt start = 0.4
    //      case 't': simulation.set_dt(simulation.get_dt() - 0.001); break;
    //      case 'T': simulation.set_dt(simulation.get_dt() + 0.001); break;
    static int last_pos_timestep = 500;				//remembers last slider location, statics only get initialized once, after that they keep the new value
    double new_pos = position - last_pos_timestep;
    double old_dt = simulation.get_dt();
    double new_dt = old_dt + new_pos * 0.001; //easier to debug on separate line
    if (new_dt < 0){
        new_dt = 0;
    }
    simulation.set_dt(new_dt);
    last_pos_timestep = position;
}

void MyGLWidget::hedgehogScaling(int position)
{
    // vec_scale = 1000;
    //  	  case 'S': vec_scale *= 1.2; break;
    //        case 's': vec_scale *= 0.8; break;
    // The scaling goes exponential with keyboard, but with slide can just do linear
    vec_scale = position*2;
}

void MyGLWidget::fluidViscosity(int position)
{
    // visc = 0.001
    //      case 'V': simulation.set_visc(simulation.get_visc()*5); break;
    //      case 'v': simulation.set_visc(simulation.get_visc()*0.2); break;
    // The scaling goes exponential with keyboard, but with slide can just do linear
    float visc =  position*0.00001;
    simulation.set_visc(visc);
}

void MyGLWidget::colorDir(bool color_dir_bool){
    if (color_dir_bool) {
       color_dir = 1;
    }
    else {
       color_dir = 0;
    }
}

void MyGLWidget::scalarColoring(QString scalartype){
    if (scalartype == "rainbow") {
       scalar_col = COLOR_RAINBOW;
    }
    else if (scalartype == "black&white") {
       scalar_col = COLOR_BLACKWHITE;
    }
    else if (scalartype == "rainbow2") {
        scalar_col = COLOR_RAINBOW2;
    }
}
