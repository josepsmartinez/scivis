#include "simulation.h"
#include <cmath>

//Default Constructor
Simulation::Simulation() {
    // default values member variables
    dt = 0.4;				//simulation time step
    visc = 0.001;				//fluid viscosity
    frozen = false;               //toggles on/off the animation
}

//Destructor
Simulation::~Simulation(){}

//Accessor Functions
bool Simulation::get_frozen() const{return frozen;}
double Simulation::get_dt() const{return dt;}
float Simulation::get_visc() const{return visc;}

cVectorialField* Simulation::get_f()  {return &f;}
//fftw_real* Simulation::get_fx() const{return fx;}
//fftw_real* Simulation::get_fy() const{return fy;}

cField* Simulation::get_rho() {return &rho;}
cField *Simulation::get_rho0()  {return &rho0;}

cVectorialField* Simulation::get_v()  {return &v;}
fftw_real* Simulation::get_vm() const {return vm;}

fftw_real* Simulation::get_vx0() const {return vx0;}
fftw_real* Simulation::get_vy0() const {return vy0;}

//Mutator functions
void Simulation::set_frozen(bool new_frozen){frozen = new_frozen;}
void Simulation::set_dt(double new_dt){dt = new_dt;}
void Simulation::set_visc(float new_visc){visc = new_visc;}

//init_simulation: Initialize simulation data structures as a function of the grid size 'n'.
//                 Although the simulation takes place on a 2D grid, we allocate all data structures as 1D arrays,
//                 for compatibility with the FFTW numerical library.
void Simulation::init_simulation(int n)
{
    int i; size_t dim;
    dim     = n * 2*(n/2+1)*sizeof(fftw_real);        //Allocate data structures
    v.initialize(n * 2*(n/2+1), n);
    vm       = (fftw_real*) malloc(dim);
    vx0      = (fftw_real*) malloc(dim);
    vy0      = (fftw_real*) malloc(dim);
    dim     = n * n * sizeof(fftw_real);
    f.initialize(n*n, n);
    rho.initialize(n*n, n);
    rho0.initialize(n*n, n);
    plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
    plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

    for (i = 0; i < n * n; i++)                      //Initialize data structures to 0
    //{ vx[i] = vy[i] = vx0[i] = vy0[i] = fx[i] = fy[i] = rho[i] = rho0[i] = 0.0f; }
    { vx0[i] = vy0[i] = 0.0f; }
}

//FFT: Execute the Fast Fourier Transform on the dataset 'vx'.
//     'dirfection' indicates if we do the direct (1) or inverse (-1) Fourier Transform
void Simulation::FFT(int direction,void* vx)
{
    if(direction==1) rfftwnd_one_real_to_complex(plan_rc,(fftw_real*)vx,(fftw_complex*)vx);
    else             rfftwnd_one_complex_to_real(plan_cr,(fftw_complex*)vx,(fftw_real*)vx);
}

int Simulation::clamp(float x)
{ return ((x)>=0.0?((int)(x)):(-((int)(1-(x))))); }

//solve: Solve (compute) one step of the fluid flow simulation
void Simulation::solve(int n)
{
    fftw_real x, y, x0, y0, f, r, U[2], V[2], s, t;
    int i, j, i0, j0, i1, j1;

    // resets min/max
    if(false){ // probably should not reset at every step, but maybe periodically
        v_min = 1.f;
        v_max = 0.f;
    }

    for (i=0;i<n*n;i++)
    { v.update_x(i, v.read_x(i)+dt*vx0[i]); vx0[i] = v.read_x(i); v.update_y(i, v.read_y(i)+dt*vy0[i]); vy0[i] = v.read_y(i); }

    for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
       for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
       {
          x0 = n*(x-dt*vx0[i+n*j])-0.5f;
          y0 = n*(y-dt*vy0[i+n*j])-0.5f;
          i0 = clamp(x0); s = x0-i0;
          i0 = (n+(i0%n))%n;
          i1 = (i0+1)%n;
          j0 = clamp(y0); t = y0-j0;
          j0 = (n+(j0%n))%n;
          j1 = (j0+1)%n;
          v.update_x(i+n*j, (1-s)*((1-t)*vx0[i0+n*j0]+t*vx0[i0+n*j1])+s*((1-t)*vx0[i1+n*j0]+t*vx0[i1+n*j1]));
          v.update_y(i+n*j, (1-s)*((1-t)*vy0[i0+n*j0]+t*vy0[i0+n*j1])+s*((1-t)*vy0[i1+n*j0]+t*vy0[i1+n*j1]));
       }

    for(i=0; i<n; i++)
      for(j=0; j<n; j++)
      {  vx0[i+(n+2)*j] = v.read_x(i+n*j); vy0[i+(n+2)*j] = v.read_y(i+n*j); }

    FFT(1,vx0);
    FFT(1,vy0);

    for (i=0;i<=n;i+=2)
    {
        x = 0.5f*i;
        for (j=0;j<n;j++)
        {
            y = j<=n/2 ? (fftw_real)j : (fftw_real)j-n;
            r = x*x+y*y;
            if ( r==0.0f ) continue;
            f = (fftw_real)exp(-r*dt*visc);
            U[0] = vx0[i  +(n+2)*j]; V[0] = vy0[i  +(n+2)*j];
            U[1] = vx0[i+1+(n+2)*j]; V[1] = vy0[i+1+(n+2)*j];

            vx0[i  +(n+2)*j] = f*((1-x*x/r)*U[0]     -x*y/r *V[0]);
            vx0[i+1+(n+2)*j] = f*((1-x*x/r)*U[1]     -x*y/r *V[1]);
            vy0[i+  (n+2)*j] = f*(  -y*x/r *U[0] + (1-y*y/r)*V[0]);
            vy0[i+1+(n+2)*j] = f*(  -y*x/r *U[1] + (1-y*y/r)*V[1]);
        }
    }

    FFT(-1,vx0);
    FFT(-1,vy0);

    f = 1.0/(n*n);
    for (i=0;i<n;i++)
       for (j=0;j<n;j++)
       {
           v.update_x(i+n*j, f*vx0[i+(n+2)*j]);
           v.update_y(i+n*j, f*vy0[i+(n+2)*j]);

       }


}


// diffuse_matter: This function diffuses matter that has been placed in the velocity field. It's almost identical to the
// velocity diffusion step in the function above. The input matter densities are in rho0 and the result is written into rho.
void Simulation::diffuse_matter(int n)
{
    fftw_real x, y, x0, y0, s, t;
    int i, j, i0, j0, i1, j1;

    for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
        for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
        {
            x0 = n*(x-dt*v.read_x(i+n*j))-0.5f;
            y0 = n*(y-dt*v.read_y(i+n*j))-0.5f;
            i0 = clamp(x0);
            s = x0-i0;
            i0 = (n+(i0%n))%n;
            i1 = (i0+1)%n;
            j0 = clamp(y0);
            t = y0-j0;
            j0 = (n+(j0%n))%n;
            j1 = (j0+1)%n;
            rho.update(i+n*j,(1-s)*((1-t)*rho0.read(i0+n*j0)
                 +t*rho0.read(i0+n*j1)) +s*((1-t)*rho0.read(i1+n*j0)+t*rho0.read(i1+n*j1)));
        }
    }

//set_forces: copy user-controlled forces to the force vectors that are sent to the solver.
//            Also dampen forces and matter density to get a stable simulation.
void Simulation::set_forces(int DIM)
{
    int i;
    for (i = 0; i < DIM * DIM; i++)
    {
        rho0.update(i, 0.995 * rho.read(i));
        f.update_x(i, f.read_x(i)*0.85);
        f.update_y(i, f.read_y(i)*0.85);
        vx0[i]    = f.read_x(i);
        vy0[i]    = f.read_y(i);
    }
}

void Simulation::drag(int mx, int my, int DIM, int winWidth, int winHeight)
{
    int xi,yi,X,Y; double  dx, dy, len;
    static int lmx=0,lmy=0;				//remembers last mouse location

    // Compute the array index that corresponds to the cursor location
    xi = (int)clamp((double)(DIM + 1) * ((double)mx / (double)winWidth));
    yi = (int)clamp((double)(DIM + 1) * ((double)(winHeight - my) / (double)winHeight));

    X = xi; Y = yi;

    if (X > (DIM - 1))  X = DIM - 1; if (Y > (DIM - 1))  Y = DIM - 1;
    if (X < 0) X = 0; if (Y < 0) Y = 0;

    // Add force at the cursor location
    my = winHeight - my;
    dx = mx - lmx;
    dy = my - lmy;
    len = sqrt(dx * dx + dy * dy);
    if (len != 0.0)
    {
        dx *= 0.1 / len;
        dy *= 0.1 / len;
    }
    f.update_x(Y * DIM + X, f.read_x(Y * DIM + X)+dx);
    f.update_y(Y * DIM + X, f.read_y(Y * DIM + X)+dy);
    rho.update(Y * DIM + X, 10.0f);
    lmx = mx; lmy = my;
}
