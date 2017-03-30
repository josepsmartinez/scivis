#include "simulation.h"
#include "visualization.h"

//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
const int COLOR_RAINBOW=1;
const int COLOR_BANDS=2;
const int COLOR_RAINBOW2=3;
const int COLOR_ARRAY=4;

// COLOR CLASS
Color::Color(float r, float g, float b)
    : r(r), g(g), b(b)
{
    update_hsv();
    s*=2;
    load_hsv();
}

void Color::update_hsv()
{
    float M = max(r,max(g,b));
    float m = min(r,min(g,b));
    float d = M-m;

    v = M;
    s = (M>0.00001) ? d/M : 0;
    if (s==0) h=0;
    else{
        if      (r==M)      h =     (g-b)/d;
        else if (g==M)      h = 2 + (b-r)/d;
        else                h = 4 + (r-g)/d;

        h/=6;
        if(h<0) h+=1;
    }

}

void Color::load_hsv()
{
    int huec    = (int)(h*6);
    float f     = 6*h - huec;
    float lx    = v*(1-s);
    float ly    = v*(1-s*f);
    float lz    = v*(1-s*(1-f));

    switch(huec){
        case 0:
        case 6: r=v;  g=lz; b=lx; break;
        case 1: r=ly; g=v;  b=lx; break;
        case 2: r=lx; g=v;  b=lz; break;
        case 3: r=lx; g=ly; b=v;  break;
        case 4: r=lz; g=lx; b=v;  break;
        case 5: r=v;  g=lx; b=ly; break;
    }
}

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
QColor set_colormap(float value, int scalar_col, int n_colors, fftw_real max=1.f, fftw_real min=0.f, bool legend=false)
{
    float R =0,G=0,B=0;

    // normalizes value
    value = (value - min)/(max - min);
    if(value>1.f) value=1.f; if(value<0.f) value=0.f;

    // subsample according to n_colors
    value = (1/(float)(n_colors-1))*(int) ((n_colors-1)*value);

    // do gremio
    vector<Color> carr;
    carr.push_back(Color(1, 0, 0)); // blue
    carr.push_back(Color(0, 1, 0)); // white
    carr.push_back(Color(0, 0, 1)); // black

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

    if(R<0) R = 0;
    if(G<0) G = 0;
    if(B<0) B = 0;

    if(legend) return QColor(255*R,255*G,255*B);
    else{ glColor3f(R,G,B);
        return QColor(255*R,255*G,255*B);}

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





