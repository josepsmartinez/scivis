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
#include "timebuffer.h"


MyGLWidget::MyGLWidget(QWidget *parent)
{
    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    color_dir = 1;
    vec_scale = 1000;			//scaling of hedgehogs
    draw_smoke = true;           //draw the smoke or not
    draw_vecs = true;            //draw the vector field or not
    draw_frozenstreamline = true;
    draw_slices = false;
    draw_streamline = false;
    separate_streamlines = true;
    streamline_lenght = 250;
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
    numStreamline = 20;
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

    streamlines.resize(numStreamline);
    update_streamline_matrix();

    alpha = 1;
    data_alpha = false;
    rotation_angle = 0;
    num_slices = 10;
    timesteps_each_slice = 10;
    timestep_count = timesteps_each_slice;

    buffer.resize(3);

    buffer[DATA_DENSITY].initialize(num_slices,simulation.get_rho(),DIM);
    buffer[DATA_FORCEFIELD].initialize(num_slices,simulation.get_f(),DIM);
    buffer[DATA_VELOCITY].initialize(num_slices,simulation.get_v(),DIM);

    timestep(6000);
}

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::initializeGL()
{
    qglClearColor(Qt::black);
}

void MyGLWidget::slice_to_position(int plane)
{
    float halfw = winWidth/2;
    float halfh =winHeight/2;
    glTranslatef(halfw,halfh,0);
    glRotatef(-45 + rotation_angle,0,1,0);
    glRotatef(5,1,0,0);
    glTranslatef(0,0,(plane-(float)num_slices/2)*halfh/10); //
    glScalef(0.7f,0.7f,0.7f);
    glTranslatef(-halfw,-halfh,0);
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

    for(int plane = 0;plane<num_slices;plane++)
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        if(draw_slices){
            slice_to_position(plane);
        }

        int        i, j, idx; double px,py;
        fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
        fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh

        if(draw_streamline || (simulation.get_frozen() && draw_frozenstreamline)) { // streamline
            for(int i = 0; i < numStreamline; i++) //get and draw lines
            {
                vector<Point> stream_points = streamlines[i]->line(-1,streamline_lenght); // negative is until border is reached
                glBegin(GL_LINE_STRIP);
                //glColor3f(1.f,1.f,1.f);
                for(int i=0; i < stream_points.size(); i++){
                    if(clamp)
                    {
                        min = clamp_min;
                        max = clamp_max;
                    }else
                    {
                        max = simulation.get_v()->get_max();
                        min = 0;
                    }

                    idx = ((int)(stream_points[i].p[0] + 0.5f)) + ((int)(stream_points[i].p[1] + 0.5f))*DIM;
                    set_colormap(simulation.get_v()->read(idx),scalar_col, n_colors, max, min, hues, sats, alpha, data_alpha);
                    glVertex2f(wn+stream_points[i].p[0]*wn, hn+stream_points[i].p[1]*hn);
                }
                glEnd();
            }

        }
        if (draw_vecs)
        {
            //vectorial_draw->rebuild();

            fftw_real  wng = ((fftw_real)winWidth -wn) / (fftw_real)(COL);   // Grid cell width
            fftw_real  hng = ((fftw_real)winHeight -hn) / (fftw_real)(ROW);  // Grid cell heigh

            for (i = 0; i < COL; i++)
              for (j = 0; j < ROW; j++)
              {
                int interpolated_j = ((float)j+ jitter_j[i][j])*((float)DIM/(float)ROW) +0.5; // closest neighbor
                int interpolated_i = ((float)i+ jitter_i[i][j])*((float)DIM/(float)COL) +0.5;
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
                Field* cur_scalar_draw_hedgehog = buffer[hedgehog_scalar].read(plane);
                if(cur_scalar_draw_hedgehog == NULL||!draw_slices)
                        cur_scalar_draw_hedgehog = scalar_draw_hedgehog;
                QColor colr = set_colormap(cur_scalar_draw_hedgehog->read(idx),scalar_col, n_colors, max, min, hues, sats, alpha, data_alpha);
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
                    drawCone(angle,lenght,wn+(i + jitter_i[i][j])*wng,hn+(j + jitter_j[i][j])*hng,10,wn,hn, colr,plane);
                }
                else if(hedgehog_type == HEDGEHOG_ARROW)
                {
                    drawArrow(angle,lenght,wn+(i + jitter_i[i][j])*wng,hn+(j + jitter_j[i][j])*hng,10,wn,hn,plane);
                }else if(hedgehog_type == HEDGEHOG_LINE)
                {
                    drawLine(angle,lenght,wn+(i + jitter_i[i][j])*wng,hn+(j + jitter_j[i][j])*hng,10,wn,hn,plane);
                }
              }
            if(draw_slices){
                slice_to_position(plane);
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
                    Field* cur_scalar_draw = buffer[data_type].read(plane);
                    if(cur_scalar_draw == NULL||!draw_slices)
                            cur_scalar_draw = scalar_draw;
                    set_colormap(cur_scalar_draw->read(idx0),scalar_col, n_colors, max, min, hues, sats, alpha, data_alpha);    glVertex2f(px0, py0);
                    set_colormap(cur_scalar_draw->read(idx1),scalar_col, n_colors, max, min, hues, sats, alpha, data_alpha);    glVertex2f(px1, py1);
                    set_colormap(cur_scalar_draw->read(idx2),scalar_col, n_colors, max, min, hues, sats, alpha, data_alpha);    glVertex2f(px2, py2);
                    set_colormap(cur_scalar_draw->read(idx0),scalar_col, n_colors, max, min, hues, sats, alpha, data_alpha);    glVertex2f(px0, py0);
                    set_colormap(cur_scalar_draw->read(idx2),scalar_col, n_colors, max, min, hues, sats, alpha, data_alpha);    glVertex2f(px2, py2);
                    set_colormap(cur_scalar_draw->read(idx3),scalar_col, n_colors, max, min, hues, sats, alpha, data_alpha);    glVertex2f(px3, py3);

                }
            }
            glEnd();

            //printf("%f %f \n", scalar_draw->get_min(), scalar_draw->get_max());
            //printf("%f %f \n", simulation.get_f()->get_min(), simulation.get_f()->get_max());




        }
        glPopMatrix();
        glLoadIdentity();
        if(!draw_slices) break;
    }
    glFlush();
}

void MyGLWidget::drawLine(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn, int plane)
{
    glPushMatrix();

    if(draw_slices){
        slice_to_position(plane);
    }

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

void MyGLWidget::drawArrow(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn,int plane)
{
    glPushMatrix();

    if(draw_slices){
        slice_to_position(plane);
    }

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

void MyGLWidget::drawCone(float angle, float lenght, int x_coord, int y_coord, int scaling_factor, fftw_real wn, fftw_real hn, QColor color, int plane)
{
    glPushMatrix();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightpos[] = {100., 100., 0., 0.};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    if(draw_slices){
        slice_to_position(plane);
    }

    glTranslatef(x_coord,y_coord, 0);
    glScaled(wn,hn,0);
    glRotated(angle,0,0,1);
    glScaled(log(lenght/scaling_factor+1),log(lenght/(scaling_factor/2)+1),0);
    glRotated(-90,1,0,0);
    glRotated(45.,0,0,1);

    GLfloat col[] = {color.redF(), color.greenF(), color.blueF(), color.alphaF()};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
    GLUquadricObj *quadObj = gluNewQuadric();
    gluCylinder(quadObj, 0.3, 0, 1, 5, 5);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glPopMatrix(); // now it's at normal scale again
    glLoadIdentity(); // needed to stop the rotating, otherwise rotates the entire drawing
}

void MyGLWidget::resizeGL(int width, int height)
{
    glViewport(0.0f, 0.0f, (GLfloat)width, (GLfloat)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)width, 0.0, (GLdouble)height,-2000,2000);
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
        simulation.get_rho()->reset_limits();
        simulation.get_f()->reset_limits();
        simulation.get_v()->reset_limits();
        simulation.set_forces(DIM);
        simulation.solve(DIM);
        simulation.diffuse_matter(DIM);
        timestep_count++;
        if(timestep_count >= timesteps_each_slice)
        {
            buffer[DATA_DENSITY].append();
            buffer[DATA_VELOCITY].append();
            buffer[DATA_FORCEFIELD].append();
            timestep_count = 0;
        }
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

void MyGLWidget::drawSlices(bool new_draw_slices)
{
    draw_slices = new_draw_slices;
}

void MyGLWidget::setClamp(bool new_clamp)
{
    clamp = new_clamp;
}

void MyGLWidget::setSeparateStreamlines(bool new_sep_s)
{
    separate_streamlines = new_sep_s;
}

void MyGLWidget::setDataAlpha(bool new_data_alpha)
{
    data_alpha = new_data_alpha;
}

void MyGLWidget::setClampMax(double new_clamp_max)
{
    clamp_max = new_clamp_max;
}

void MyGLWidget::setClampMin(double new_clamp_min)
{
    clamp_min = new_clamp_min;
}

void MyGLWidget::setHueShift(float new_hues)
{
    hues = new_hues;
}

void MyGLWidget::setSaturationShift(float new_sats)
{
    sats = new_sats;
}

void MyGLWidget::setAlpha(float new_alpha)
{
    alpha = new_alpha;
}

void MyGLWidget::setStreamlineLenght(int n)
{
    streamline_lenght = n;
}

void MyGLWidget::setRotationAngle(int n)
{
    rotation_angle = n;
}

void MyGLWidget::setnumSlices(int n)
{
    num_slices = n;
}

void MyGLWidget::setSliceTimestep(int n)
{
    timesteps_each_slice = n;
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

void MyGLWidget::setnumStreamline(int n)
{
    numStreamline = n;
    update_streamline_matrix();
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
    static int last_pos_timestep = 4000;				//remembers last slider location, statics only get initialized once, after that they keep the new value
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
    }else if(hedgehog_vector == DATA_GRADIENT_DENSITY)
    {
        vectorial_draw = simulation.get_rho()->get_gradient();
    }else if(hedgehog_vector == DATA_GRADIENT_VELOCITY)
    {
        vectorial_draw = simulation.get_v()->get_gradient();
    }
}

void MyGLWidget::drawStreamlines(QString draw){
    if (draw == "Always") {
       draw_streamline = true;
    }
    else if (draw == "Frozen") {
       draw_frozenstreamline = true;
       draw_streamline = false;
    }
    else if (draw == "Never") {
        draw_frozenstreamline = false;
        draw_streamline = false;
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
    return set_colormap(value, scalar_col, n_colors, max, min, hues, sats,1.f,false, true);
}

void MyGLWidget::update_streamline_matrix()
{
    for(int i = 0; i < streamlines.size();i++)
            delete streamlines[i];
    streamlines.resize(numStreamline);
    float stream_areas = sqrt(numStreamline);
    float mul_fac = (float)DIM/stream_areas;
    for(int i = 0; i < numStreamline; i++)
    {
        float x =25,y=25;
        if(separate_streamlines)
        {
            float a = (float)i/stream_areas;
            x = mul_fac*(i - (int)a*stream_areas + ((rand()%100)/150.0));
            y = mul_fac*(a + ((rand()%100)/150.0));
        }else{
            x = (rand()%500)/10.0;
            y = (rand()%500)/10.0;
        }
        streamlines[i] = new StreamLine(x,y, simulation.get_v(), DIM);

    }
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
            jitter_i[i][j] = randomness* (((rand()%100) - 50)/100.0);
            jitter_j[i][j] = randomness* (((rand()%100) - 50)/100.0);
        }
}
