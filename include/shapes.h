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

struct Block {
    Vector3 center;
    Vector3 size;
    bool show;
    BoundingBox bb;
    Block(Vector3 c, Vector3 s) {
        center = c;
        size = s;
        Vector3 min {
            .x = center.x - size.x/2,
            .y = center.y - size.y/2,
            .z = center.z - size.z/2,
        }, 
        max {
            .x = center.x + size.x/2,
            .y = center.y + size.y/2,
            .z = center.z + size.z/2,
        };
        bb = {min, max};
        show = true;
    }
    Block(Vector3 c, float s) : Block(c, {s, s, s}) {
    }
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