// Creation, Manipulation and Drawing of all shapes.

#pragma once

#include "raylib.h"
#include <vector>

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

Quad create_quad(Vector3 center, float size);
Quad create_quad(Vector3 center, Vector3 size);
bool destroy_quad(Quad*);
void draw_lines(std::vector<Line> *v);