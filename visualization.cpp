#include "simulation.h"
#include "visualization.h"

//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
const int COLOR_RAINBOW=1;
const int COLOR_BANDS=2;
const int COLOR_RAINBOW2=3;
const int COLOR_ARRAY=4;

//------ VISUALIZATION CODE STARTS HERE -----------------------------------------------------------------
float max(float x, float y)
{ return x > y ? x : y; }



//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void rainbow(float value,float* R,float* G,float* B)
{
   const float dx=0.8f;
   //if (value<0) value=0; if (value>1) value=1;
   value = (6-2*dx)*value+dx;

   *R = max(0.0f, (3-(float)fabs(value-4)-(float)fabs(value-5))/2);
   *G = max(0.0f, (4-(float)fabs(value-2)-(float)fabs(value-4))/2);
   *B = max(0.0f, (3-(float)fabs(value-1)-(float)fabs(value-2))/2);
}

//rainbow: Implements a color palette, mapping the scalar 'value' RGB parabolas
void rainbow2(float value,float* R,float* G,float* B)
{
    //if (value<0) value=0; if (value>1) value=1;

   *R = max(0.0f, -4*value*(value-2) - 3);
   *G = max(0.0f, -4*value*(value-1)    );
   *B = max(0.0f, -4*value*value + 1    );
}

//bluered
void bluered(float value,float* R,float* G,float* B)
{
    //if (value<0) value=0; if (value>1) value=1;

   *R = max(0.0f, -4*value*(value-2) - 3);
   *G = max(0.0f, -4*value*(value-1)    );
   *B = max(0.0f, -4*value*value + 1    );
}

//color_array
void color_array(float value,float* R,float* G,float* B,vector<Color> array) {
    int n = array.size(); // number of colors
    float dx = 1 / ((float)n-1); // interval of each interpolation

    //if (value<0) value=0; if (value>1) value=1.f;
    int c_index = 0;

    while(value > dx) { // finds interpolation interval and its parameter
        c_index++;
        value -= dx;
    }
    value /= dx; // normalizes interpolation parameter

    Color c = array[c_index];
    Color c_next = array[c_index+1];

    *R = (1 - value)*c.r + value*c_next.r;
    *G = (1 - value)*c.g + value*c_next.g;
    *B = (1 - value)*c.b + value*c_next.b;

}


//set_colormap: Sets three different types of colormaps
void set_colormap(float value, int scalar_col, fftw_real max=1.f, fftw_real min=0.f)//, float color_clamp_min, float color_clamp_max,
                  //int color_bands, int hue_degree, float saturation, bool scale_colors, float vy_min, float vy_max, float alpha)
{
    float R,G,B;

    // normalizes value
    value = (value - min)/(max - min);
    if(value>1.f) value=1.f; if(value<0.f) value=0.f;

    // do gremio
    vector<Color> carr;
    carr.push_back({0, 0, 1}); // blue
    carr.push_back({1, 1, 1}); // white
    carr.push_back({0, 0, 0}); // black

    if (scalar_col==COLOR_BLACKWHITE)
        R = G = B = value;
    else if (scalar_col==COLOR_RAINBOW)
        rainbow(value,&R,&G,&B);
    else if (scalar_col==COLOR_RAINBOW2)
        rainbow2(value,&R,&G,&B);
    else if (scalar_col == COLOR_ARRAY)
        color_array(value,&R,&G,&B,carr);
        //rainbow2(value,&R,&G,&B);
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





