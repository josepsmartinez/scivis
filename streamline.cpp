#include "streamline.h"
#include <QtWidgets>


// (x,y) <-> ix
void StreamLine::load_ix(int ix)
{
    points.push_back(Point(vector<fftw_real>{(float) (ix % dim), (float) (ix / dim)}));
    //point.p[0] = (float) (ix % dim);
    //point.p[1] = (float) (ix / dim);
}

int StreamLine::get_ix()
{
    Point point = points.back();
    return ((int)(point.p[0] + 0.5f)) + ((int)(point.p[1]+ 0.5f))*dim; // closest neighbor (jean pattern)
}


// CONSTRUCTORS
StreamLine::StreamLine(int ix, vectorialField* srcv, int dim)
    : points(), v(srcv), dim(dim), steps(0)
{
    /*
     * here we can decide whether the stream follows
     *  a flowing velocity field (how it is implemented, every iteration acessing the updated v field)
     *  a static, predictive velocity field (implies allocating and copying the field)

     v = (vectorialField*) malloc(sizeof(vectorialField));
     *v = vectorialField(*srcv); // not entirely sure about vecfield copy constructor
    */
    load_ix(ix);
}

StreamLine::StreamLine(const StreamLine &mit)
    : v(mit.v), dim(mit.dim), steps(mit.steps), dt(mit.dt), points(mit.points)
{
    //for(int i=0; i<mit.points.size();i++) points[i] = mit.points[i];
}

// OPERATORS
StreamLine &StreamLine::operator++()
{

    Point point = points.back();
    int ix = get_ix();
    /*QVector2D mov(v->read_x(ix),v->read_y(ix));
    mov.normalize();
    point.pointwise_sum({
        mov.x()*dt,
        mov.y()*dt
    });
    */
    Point shift({v->read_x(ix), v->read_y(ix)});
    //shift.scalar_mul(1/pow(v->read_x(ix)*v->read_x(ix) + v->read_y(ix)*v->read_y(ix), 0.5f));
    shift.scalar_mul(1/shift.norm());
    //shift.normalize();
    shift.scalar_mul(dt);

    point.pointwise_sum({shift.p[0], shift.p[1]});
    points.push_back(point);


    steps++;
}


bool StreamLine::operator==(const StreamLine &other)
{
    return (steps==other.steps);
}

bool StreamLine::operator!=(const StreamLine &other)
{
    return (steps!=other.steps);
}

bool StreamLine::operator==(int s)
{
    return (steps==s);
}

bool StreamLine::operator!=(int s)
{
    return (steps!=s);
}

vector<Point> StreamLine::operator*()
{
    return points; // copy by reference (hopefully)
}



// POINT CLASS (maybe should not be here)
Point::Point(int dim)
    : p(dim)
{

}

Point::Point(vector<fftw_real> src)
    : p(src.size())
{
    initialize(src);
}

Point::Point(const Point &src)
    : p(src.p.size())
{
    initialize(src.p);
}

Point::initialize(vector<fftw_real> src)
{
    int dim = p.size();
    if (src.size() == dim){
        for(int i=0; i<dim; i++) p[i] = src[i];
    }
}

fftw_real Point::norm()
{
    fftw_real sum=0;
    int n = p.size();
    for(int i=0; i<n;i++) sum+=p[i]*p[i];
    return pow(sum, 1/(float)n);
}

Point::normalize()
{
    this->scalar_mul(1/this->norm());
}

Point::scalar_mul(fftw_real s)
{
    for(int i=0; i<p.size(); i++) p[i] *= s;
}

Point::pointwise_mul(vector<fftw_real> src)
{
    int dim = p.size();
    if (src.size() == dim){
        for(int i=0; i<dim; i++) p[i] *= src[i];
    }
}

Point::pointwise_sum(vector<fftw_real> src)
{
    int dim = p.size();
    if (src.size() == dim){
        for(int i=0; i<dim; i++) p[i] += src[i];
    }
}
