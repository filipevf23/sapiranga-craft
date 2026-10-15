#pragma once

#include "raylib.h"

struct Line {
    Vector3 p0, p1;
};

struct Quad {
    Line *lines;
    Vector3 *vertices;
    Vector3 size;
    Vector3 center;
    bool show;
};

struct Sphere {
    Line *lines;
    double radius;
    Vector3 center;
};