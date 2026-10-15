#pragma once

#include "math_types.h"
#include <vector>

namespace Algebra {
    void rotate_square_xx(double alpha, std::vector<Line> *v, Vector3 center);
    void rotate_square_yy(double alpha, std::vector<Line> *v, Vector3 center);
    void rotate_square_zz(double alpha, std::vector<Line> *v, Vector3 center);
    void rotate_yy(double, Vector3*, Vector3);
    double norm(Vector3 v);
    Vector3 cross_product(Vector3, Vector3);
    Vector3 vector_divide(Vector3, float);
    Vector3 normalize(Vector3);
}
