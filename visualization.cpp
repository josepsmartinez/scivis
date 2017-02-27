#include "simulation.h"
#include "visualization.h"

//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
const int COLOR_RAINBOW=1;
const int COLOR_BANDS=2;
const int COLOR_RAINBOW2=3;

//------ VISUALIZATION CODE STARTS HERE -----------------------------------------------------------------
float max(float x, float y)
{ return x > y ? x : y; }



//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void rainbow(float value,float* R,float* G,float* B)
{
   const float dx=0.8f;
   if (value<0) value=0; if (value>1) value=1;
   value = (6-2*dx)*value+dx;

   *R = max(0.0f, (3-(float)fabs(value-4)-(float)fabs(value-5))/2);
   *G = max(0.0f, (4-(float)fabs(value-2)-(float)fabs(value-4))/2);
   *B = max(0.0f, (3-(float)fabs(value-1)-(float)fabs(value-2))/2);
}

//rainbow: Implements a color palette, mapping the scalar 'value' RGB parabolas
void rainbow2(float value,float* R,float* G,float* B)
{
    if (value<0) value=0; if (value>1) value=1;

   *R = max(0.0f, -4*value*(value-2) - 3);
   *G = max(0.0f, -4*value*(value-1)    );
   *B = max(0.0f, -4*value*value + 1    );
}


//set_colormap: Sets three different types of colormaps
void set_colormap(float value, int scalar_col, fftw_real min=0.f, fftw_real max=1.f)//, float color_clamp_min, float color_clamp_max,
                  //int color_bands, int hue_degree, float saturation, bool scale_colors, float vy_min, float vy_max, float alpha)
{
    float R,G,B;

    // normalizes value
    value = (value - min)/(max - min);

    if (scalar_col==COLOR_BLACKWHITE)
        R = G = B = value;
    else if (scalar_col==COLOR_RAINBOW)
        rainbow(value,&R,&G,&B);
    else if (scalar_col==COLOR_RAINBOW2)
        rainbow2(value,&R,&G,&B);
    else if (scalar_col==COLOR_BANDS){
       const int NLEVELS = 7;
       value *= NLEVELS; value = (int)(value); value/= NLEVELS;
       rainbow(value,&R,&G,&B);
    }

    glColor3f(R,G,B);

}


//direction_to_color: Set the current color by mapping a direction vector (x,y), using
//                    the color mapping method 'method'. If method==1, map the vector direction
//                    using a rainbow colormap. If method==0, simply use the white color
void direction_to_color(float x, float y, int method)//, int color_bands, int color_clamp_min, int color_clamp_max, int hue_degree, float saturation)
{
    float r,g,b,f;
    if (method)
    {
      f = atan2(y,x) / 3.1415927 + 1;
      r = f;
      if(r > 1) r = 2 - r;
      g = f + .66667;
      if(g > 2) g -= 2;
      if(g > 1) g = 2 - g;
      b = f + 2 * .66667;
      if(b > 2) b -= 2;
      if(b > 1) b = 2 - b;
    }
    else
    { r = g = b = 1; }
    glColor3f(r,g,b);

}





