#include "linear_algebra.h"
#include <math.h>

using namespace std;

void Algebra::rotate_square_xx(double alpha, vector<Line> *v, Vector3 center) {
    double cos_alpha = cos(alpha);
    double sin_alpha = sin(alpha);
    for (auto &l : *v) {
        l.p0 = {l.p0.x - center.x, l.p0.y - center.y, l.p0.z - center.z};
        l.p0.y = cos_alpha*l.p0.y - sin_alpha*l.p0.z;
        l.p0.z = sin_alpha*l.p0.y + cos_alpha*l.p0.z ;
        l.p0 = {l.p0.x + center.x, l.p0.y + center.y, l.p0.z + center.z};
        l.p1 = {l.p1.x - center.x, l.p1.y - center.y, l.p1.z - center.z};
        l.p1.y = cos_alpha*l.p1.y - sin_alpha*l.p1.z;
        l.p1.z = sin_alpha*l.p1.y + cos_alpha*l.p1.z ;
        l.p1 = {l.p1.x + center.x, l.p1.y + center.y, l.p1.z + center.z};
    }
}

void Algebra::rotate_square_yy(double alpha, vector<Line> *v, Vector3 center) {
    double cos_alpha = cos(alpha);
    double sin_alpha = sin(alpha);
    for (auto &l : *v) {
        l.p0 = {l.p0.x - center.x, l.p0.y - center.y, l.p0.z - center.z};
        l.p0.x = cos_alpha*l.p0.x - sin_alpha*l.p0.z;
        l.p0.z = sin_alpha*l.p0.x + cos_alpha*l.p0.z ;
        l.p0 = {l.p0.x + center.x, l.p0.y + center.y, l.p0.z + center.z};
        l.p1 = {l.p1.x - center.x, l.p1.y - center.y, l.p1.z - center.z};
        l.p1.x = cos_alpha*l.p1.x - sin_alpha*l.p1.z;
        l.p1.z = sin_alpha*l.p1.x + cos_alpha*l.p1.z ;
        l.p1 = {l.p1.x + center.x, l.p1.y + center.y, l.p1.z + center.z};
    }
}

void Algebra::rotate_square_zz(double alpha, vector<Line> *v, Vector3 center) {
    double cos_alpha = cos(alpha);
    double sin_alpha = sin(alpha);
    for (auto &l : *v) {
        l.p0 = {l.p0.x - center.x, l.p0.y - center.y, l.p0.z - center.z};
        l.p0.x = cos_alpha*l.p0.x - sin_alpha*l.p0.y;
        l.p0.y = sin_alpha*l.p0.x + cos_alpha*l.p0.y ;
        l.p0 = {l.p0.x + center.x, l.p0.y + center.y, l.p0.z + center.z};
        l.p1 = {l.p1.x - center.x, l.p1.y - center.y, l.p1.z - center.z};
        l.p1.x = cos_alpha*l.p1.x - sin_alpha*l.p1.y;
        l.p1.y = sin_alpha*l.p1.x + cos_alpha*l.p1.y ;
        l.p1 = {l.p1.x + center.x, l.p1.y + center.y, l.p1.z + center.z};
    }
}

void Algebra::rotate_yy(double alpha, Vector3 *p, Vector3 center) {
    double cos_alpha = cos(alpha);
    double sin_alpha = sin(alpha);
    *p = {p->x - center.x, p->y - center.y, p->z - center.z};
    p->x = cos_alpha*p->x - sin_alpha*p->z;
    p->z = sin_alpha*p->x + cos_alpha*p->z;
    *p = {p->x + center.x, p->y + center.y, p->z + center.z};
}

double Algebra::norm(Vector3 v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vector3 Algebra::cross_product(Vector3 a, Vector3 b) {
    return Vector3 {
        .x = a.y*b.z - a.z*b.y,
        .y = a.z*b.x - a.x*b.z,
        .z = a.x*b.y - a.y*b.x
    };
}

Vector3 Algebra::vector_divide(Vector3 v, float p) {
    return Vector3 {.x=v.x/p, .y=v.y/p, .z=v.z/p};
}

Vector3 Algebra::normalize(Vector3 v) {
    double norm = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    Vector3 nv {.x=v.x, .y=v.y, .z=v.z};
    nv.x /= norm;
    nv.y /= norm;
    nv.z /= norm;
    return nv;
}