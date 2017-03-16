#ifndef SIMULATION_H
#define SIMULATION_H

#include <rfftw.h>              //the numerical simulation FFTW library
#include "fields.h"

class Simulation {
    public:
        //Default Constructor
        Simulation();

        //Overload Constructor (to call Simulation with different default values)
        Simulation(double,float,int);

        //Destructor (destroy out of memory after being done)
        ~Simulation();

        //Accessor Functions (to get and set member variables)
        bool get_frozen() const;
        double get_dt() const;				//simulation time step
        float get_visc() const;				//fluid viscosity
        Field* get_rho() ;
        vectorialField* get_f() ;
        Field* get_rho0() ;
        vectorialField* get_v() ;
        fftw_real* get_vm() const;
        fftw_real* get_vx0() const;
        fftw_real* get_vy0() const;
        //Mutator functions
        void set_frozen(bool);
        void set_visc(float);
        void set_dt(double);

        //init_simulation: Initialize simulation data structures as a function of the grid size 'n'.
        //                 Although the simulation takes place on a 2D grid, we allocate all data structures as 1D arrays,
        //                 for compatibility with the FFTW numerical library.
        // Note to self: in header only give the type of the variable, not the name
        void init_simulation(int);

        //set_forces: copy user-controlled forces to the force vectors that are sent to the solver.
        //            Also dampen forces and matter density to get a stable simulation.
        void set_forces(int);

        // diffuse_matter: This function diffuses matter that has been placed in the velocity field. It's almost identical to the
        // velocity diffusion step in the function above. The input matter densities are in rho0 and the result is written into rho.
        void diffuse_matter(int);

        //solve: Solve (compute) one step of the fluid flow simulation
        void solve(int n);

        int clamp(float);
        void drag(int mx , int my, int DIM, int winWidth ,int winHeight);

    private:
        // member variables
        int n;
        double dt;				//simulation time step
        float visc;				//fluid viscosity    
        vectorialField v; //(vx,vy)   = velocity field at the current moment
        fftw_real *vm;             // velocity magnitude at the current moment
        fftw_real *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
        //fftw_real *fx, *fy, *fscalar;
        vectorialField f; //(f.x,f.y)   = user-controlled simulation forces, steered with the mouse
        //fftw_real *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
        scalarField rho, rho0;
        // min/max are lowest and highest values in the grid
        fftw_real vx_min, vx_max, vy_min, vy_max, v_min, v_max;
        fftw_real fx_min, fx_max, fy_min, fy_max, f_min, f_max;
        rfftwnd_plan plan_rc, plan_cr;  //simulation domain discretization
        int frozen;               //toggles on/off the animation

        //FFT: Execute the Fast Fourier Transform on the dataset 'vx'.
        //     'dirfection' indicates if we do the direct (1) or inverse (-1) Fourier Transform
        void FFT(int,void*);

};

#endif // SIMULATION_H
