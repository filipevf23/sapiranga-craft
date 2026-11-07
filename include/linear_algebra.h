#pragma once

#include "shapes.h"
#include <vector>

typedef struct {
    int x;
    int z;
} Vector2Int;

typedef struct {
    int x;
    int y;
    int z;
} Vector3Int;

typedef struct {
    int a;
    int b;
    int c;
    int d;
} Vector4Int;

namespace Algebra {
    /*
    *   Think about using only raymath and changing these!
    */

    void rotate_square_xx(double alpha, std::vector<Line> *v, Vector3 center);
    void rotate_square_yy(double alpha, std::vector<Line> *v, Vector3 center);
    void rotate_square_zz(double alpha, std::vector<Line> *v, Vector3 center);
    void rotate_yy(double, Vector3*, Vector3);
    double norm(Vector3 v);
    Vector3 cross_product(Vector3, Vector3);
    Vector3 vector_divide(Vector3, float);
    Vector3 normalize(Vector3);
    float distance3(Vector3Int, Vector3Int);
    float distance2(Vector2Int, Vector2Int);
}
