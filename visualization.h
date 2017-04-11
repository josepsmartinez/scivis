#ifndef VISUALIZATION_H
#define VISUALIZATION_H



/*typedef struct {
    float r,g,b;
} Color ;*/

class Color{
private:
    float h,s,v;
protected:
public:
    Color(float r,float g,float b);
    ~Color(){}

    float r,g,b;
    void update_hsv();  //rgb->hsv
    void load_hsv();    //hsv->rgb

    void shift(float, float);
};

using namespace std;

#endif // VISUALIZATION_H
