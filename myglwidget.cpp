// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>
#include <glut.h>          //the GLUT graphics library
#include <math.h>
#include "myglwidget.h"
#include "window.h"
#include "visualization.cpp"
#include <simulation.cpp>              //the numerical simulation FFTW library

#include "streamline.h"

MyGLWidget::MyGLWidget(QWidget *parent)
{
    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    color_dir = 1;
    vec_scale = 1000;			//scaling of hedgehogs
    draw_smoke = true;           //draw the smoke or not
    draw_vecs = true;            //draw the vector field or not
    hedgehog_type = HEDGEHOG_LINE;
    hedgehog_scalar = DATA_DENSITY;
    hedgehog_vector = DATA_VELOCITY;
    clamp = false;
    clamp_max = 1;
    max = 1;
    min = 0;
    randomness = 0;
    clamp_min = 0;
    scalar_col = 0;           //method for scalar coloring
    n_colors = 64;
    data_type = DATA_DENSITY;
    DIM = 50;
    ROW = COL = 50;
    jitter_i.resize(DIM);
    jitter_j.resize(DIM);
    for (int i = 0; i < DIM; ++i){
        jitter_i[i].resize(DIM);
        jitter_j[i].resize(DIM);
    }
    simulation.init_simulation(DIM);
    QTimer *timer = new QTimer;
    timer->start(1);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(do_one_simulation_step()));

    scalar_draw = simulation.get_rho();
    scalar_draw_hedgehog = simulation.get_rho();
    vectorial_draw = simulation.get_v();

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
        fftw_real  wng = (fftw_real)winWidth / (fftw_real)(COL + 1);   // Grid cell width
        fftw_real  hng = (fftw_real)winHeight / (fftw_real)(ROW + 1);  // Grid cell heigh

        for (i = 0; i < COL; i++)
          for (j = 0; j < ROW; j++)
          {
            int interpolated_j = (float)j*DIM/ROW +0.5 + jitter_j[i][j]; // closest neighbor
            int interpolated_i = (float)i*DIM/COL +0.5 + jitter_i[i][j];
            idx = interpolated_j*DIM + interpolated_i; // normal grid
            if(clamp)
            {
                min = clamp_min;
                max = clamp_max;
            }else
            {
                max = scalar_draw_hedgehog->get_max();
                min = 0;
            }
            set_colormap(scalar_draw_hedgehog->read(idx),scalar_col, n_colors, max, min);
            fftw_real x = vec_scale * vectorial_draw->read_x(idx);
            fftw_real y = vec_scale * vectorial_draw->read_y(idx);
            float angle=0;
            float lenght = sqrt(x*x + y*y);
            if(lenght != 0)
            {
                x = x/lenght; // normalize
                y = y/lenght;

                angle = atan2 (-x,y) * (180 / M_PI);
            }
            if(hedgehog_type == HEDGEHOG_CONE)
            {
                drawCone(angle,lenght,wng+i*wng + jitter_i[i][j],hng+j*hng + jitter_j[i][j],10,wn,hn);
            }
            else if(hedgehog_type == HEDGEHOG_ARROW)
            {
                drawArrow(angle,lenght,wng+i*wng + jitter_i[i][j],hng+j*hng + jitter_j[i][j],10,wn,hn);
            }else if(hedgehog_type == HEDGEHOG_LINE)
            {
                drawLine(angle,lenght,wng+i*wng + jitter_i[i][j],hng+j*hng + jitter_j[i][j],10,wn,hn);
            }
          }
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
                //idx0 = scalar_draw->index1d(i, j);


                px1 = wn + (fftw_real)i * wn;
                py1 = hn + (fftw_real)(j + 1) * hn;
                idx1 = ((j + 1) * DIM) + i;
                //idx1 = scalar_draw->index1d(i, j+1);


                px2 = wn + (fftw_real)(i + 1) * wn;
                py2 = hn + (fftw_real)(j + 1) * hn;
                idx2 = ((j + 1) * DIM) + (i + 1);
                //idx2 = scalar_draw->index1d(i+1, j+1);


                px3 = wn + (fftw_real)(i + 1) * wn;
                py3 = hn + (fftw_real)j * hn;
                idx3 = (j * DIM) + (i + 1);
                //idx3 = scalar_draw->index1d(i+1, j);

                if(clamp)
                {
                    min = clamp_min;
                    max = clamp_max;
                }else
                {
                    max = scalar_draw->get_max();
                    min = 0;
                }

                set_colormap(scalar_draw->read(idx0),scalar_col, n_colors, max, min);    glVertex2f(px0, py0);
                set_colormap(scalar_draw->read(idx1),scalar_col, n_colors, max, min);    glVertex2f(px1, py1);
                set_colormap(scalar_draw->read(idx2),scalar_col, n_colors, max, min);    glVertex2f(px2, py2);
                set_colormap(scalar_draw->read(idx0),scalar_col, n_colors, max, min);    glVertex2f(px0, py0);
                set_colormap(scalar_draw->read(idx2),scalar_col, n_colors, max, min);    glVertex2f(px2, py2);
                set_colormap(scalar_draw->read(idx3),scalar_col, n_colors, max, min);    glVertex2f(px3, py3);

            }
        }
        glEnd();

        //printf("%f %f \n", scalar_draw->get_min(), scalar_draw->get_max());
        //printf("%f %f \n", simulation.get_f()->get_min(), simulation.get_f()->get_max());

        //StreamLine stream;
        //StreamLine send(0+10);
        int j=0;
        for (StreamLine i(0, vectorial_draw, DIM); i!=50; i++,j++)
            printf("%d streamei %d \n", *i, j);



           // NAO FACO A MENOR IDEIA DE COMO ATUALIZAR A INTERFACE (acho que vai ser necessario)
        //Window* parent = qobject_cast<Window*>(this->parent());
        //qobject_cast<QMainWindow>(this->parent()).ui->clamp_min->value();
        //parent->clamp_min->value();
        //clamp_min->value();
        //scalar_draw->reset_limits();
    }
    glFlush();
}

void MyGLWidget::drawLine(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn)
{
    glPushMatrix();
    glTranslatef(x_coord,y_coord, 0);
    glScaled(wn,hn,0);
    glRotated(angle,0,0,1);
    glScaled(log(lenght/scaling_factor+1),log(lenght/(scaling_factor/2)+1),0);
    glBegin(GL_LINES);

    glVertex2f(0, 0);
    glVertex2f(0, 1);

    glEnd();
    glPopMatrix(); // now it's at normal scale again
    glLoadIdentity(); // needed to stop the rotating, otherwise rotates the entire drawing
}

void MyGLWidget::drawArrow(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn)
{
    glPushMatrix();
    glTranslatef(x_coord,y_coord, 0);
    glScaled(wn,hn,0);
    glRotated(angle,0,0,1);
    glScaled(log(lenght/scaling_factor+1),log(lenght/(scaling_factor/2)+1),0);
    glBegin(GL_TRIANGLES);

    //arrow head
    float head_height = 1.0/2;
    float head_width = 1.0/4;
    glVertex2f(-head_width, head_height);
    glVertex2f(0, 2*head_height);
    glVertex2f(head_width, head_height);
    // arrow tail, 2 triangles forming one square
    float tail_width = 1.0/15;
    glScaled(log(lenght+1),log(lenght+1),0);
    glVertex2f(-tail_width, head_height);
    glVertex2f(tail_width, 0);
    glVertex2f(tail_width, head_height);
    glVertex2f(-tail_width, 0);
    glVertex2f(-tail_width, head_height);
    glVertex2f(tail_width, 0);

    glEnd();
    glPopMatrix(); // now it's at normal scale again
    glLoadIdentity(); // needed to stop the rotating, otherwise rotates the entire drawing
}

void MyGLWidget::drawCone(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn)
{
    glPushMatrix();
    glTranslatef(x_coord,y_coord, 0);
    glScaled(wn,hn,0);
    glRotated(angle,0,0,1);
    glScaled(log(lenght/scaling_factor+1),log(lenght/(scaling_factor/2)+1),0);

    // draw the upper part of the cone
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    // Smooth shading

    glShadeModel(GL_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glEnable (GL_LIGHTING);
    float radius = 1/3; // calculate radius
    // foreach degree angle draw circle + arrow point
    for (int angle = 1; angle <= 360; angle+=10) {
        glColor4f(0.5,0.5,0.5,0.5-(0.5/angle)); // colors(R, G, B, alpha)
        glVertex2f(1.0/2, 1); // draw cone point/tip
        glVertex2f(sin(angle) * radius, cos(angle) * radius); // draw cone base (circle)
    }


    glEnd();
    glPopMatrix(); // now it's at normal scale again
    glLoadIdentity(); // needed to stop the rotating, otherwise rotates the entire drawing
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
    simulation.drag(mx,my, DIM, winWidth/2, winHeight/2); // jean
    //simulation.drag(mx,my, DIM, winWidth, winHeight); // terrordoclash
}

void MyGLWidget::do_one_simulation_step(bool update)
{
    if (!simulation.get_frozen())
    {
        scalar_draw->reset_limits();
        scalar_draw_hedgehog->reset_limits();
        simulation.set_forces(DIM);
        simulation.solve(DIM);
        simulation.diffuse_matter(DIM);

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

void MyGLWidget::setClamp(bool new_clamp)
{
    clamp = new_clamp;
}

void MyGLWidget::setClampMax(double new_clamp_max)
{
    clamp_max = new_clamp_max;
}

void MyGLWidget::setClampMin(double new_clamp_min)
{
    clamp_min = new_clamp_min;
}

void MyGLWidget::setNColors(int n)
{
    n_colors = n;
}

void MyGLWidget::setROW(int n)
{
    ROW = n;
    update_jitter_matrix();
}

void MyGLWidget::setCOL(int n)
{
    COL = n;
    update_jitter_matrix();
}

void MyGLWidget::setRandomness(int r)
{
    randomness = (float)r/100.0;
    update_jitter_matrix();
}

void MyGLWidget::changeData(QString new_hedgehog_type){
    if (new_hedgehog_type == "Density") {
       data_type = DATA_DENSITY;
       scalar_draw = (Field*) simulation.get_rho();
    }
    else if (new_hedgehog_type == "Velocity") {
       data_type = DATA_VELOCITY;
       scalar_draw = (Field*) simulation.get_v();
    }
    else if (new_hedgehog_type == "Forcefield") {
       data_type = DATA_FORCEFIELD;
       scalar_draw = (Field*) simulation.get_f();
    }
    if(clamp)
    {
        min = clamp_min;
        max = clamp_max;
    }else
    {
        max = scalar_draw->get_max();
        min = 0;
    }
}

void MyGLWidget::timestep(int position)
{
    // dt start = 0.4
    //      case 't': simulation.set_dt(simulation.get_dt() - 0.001); break;
    //      case 'T': simulation.set_dt(simulation.get_dt() + 0.001); break;
    static int last_pos_timestep = 500;				//remembers last slider location, statics only get initialized once, after that they keep the new value
    double new_pos = position - last_pos_timestep;
    new_pos /= 2.f;
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
    else if (scalartype == "color array") {
       scalar_col = COLOR_ARRAY;
    }
    else if (scalartype == "rainbow2") {
        scalar_col = COLOR_RAINBOW2;
    }
}

void MyGLWidget::hedgehogType(QString new_hedgehog_type){
    if (new_hedgehog_type == "Lines") {
       hedgehog_type = HEDGEHOG_LINE;
    }
    else if (new_hedgehog_type == "Cones") {
       hedgehog_type = HEDGEHOG_CONE;
    }
    else if (new_hedgehog_type == "Arrows") {
       hedgehog_type = HEDGEHOG_ARROW;
    }
}

void MyGLWidget::hedgehogScalar(int new_h_scalar){
    hedgehog_scalar = new_h_scalar;
    if(hedgehog_scalar == DATA_DENSITY)
    {
        scalar_draw_hedgehog = simulation.get_rho();
    }else if(hedgehog_scalar == DATA_VELOCITY)
    {
        scalar_draw_hedgehog = simulation.get_v();
    }else if(hedgehog_scalar == DATA_FORCEFIELD)
    {
        scalar_draw_hedgehog = simulation.get_f();
    }
    if(clamp)
    {
        min = clamp_min;
        max = clamp_max;
    }else
    {
        max = scalar_draw_hedgehog->get_max();
        min = 0;
    }
}

void MyGLWidget::hedgehogVector(int new_h_vector){
    hedgehog_vector = new_h_vector;
    if(hedgehog_vector == DATA_VELOCITY)
    {
        vectorial_draw = simulation.get_v();
    }else if(hedgehog_vector == DATA_FORCEFIELD)
    {
        vectorial_draw = simulation.get_f();
    }
}

float MyGLWidget::get_max()
{
    return max;
}
float MyGLWidget::get_min()
{
    return min;
}

QColor MyGLWidget::color_legend(float value, fftw_real max=1.f, fftw_real min=0.f){
    return set_colormap(value, scalar_col, n_colors, max, min,true);
}

void MyGLWidget::update_jitter_matrix()
{
    fftw_real  wng = (fftw_real)winWidth / (fftw_real)(COL + 1);   // Grid cell width
    fftw_real  hng = (fftw_real)winHeight / (fftw_real)(ROW + 1);  // Grid cell heigh
    jitter_i.resize(COL);
    jitter_j.resize(COL);
    for (int i = 0; i < COL; ++i){
        jitter_i[i].resize(ROW);
        jitter_j[i].resize(ROW);
    }
    for(int i = 0; i < COL; i++)
        for(int j = 0; j < ROW; j++)
        {
            jitter_i[i][j] = randomness* ((wng*((rand()%100) - 50))/100.0);
            jitter_j[i][j] = randomness* ((hng*((rand()%100) - 50))/100.0);
        }
}
