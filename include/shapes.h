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
    bool active;
    BoundingBox bb;
    int front, back, up, down, left, right;
    Block(Vector3 c, Vector3 s, bool sh=false) {
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
        show = sh;
        active = true;
        front= back= up= down= left= right= -1;
    }
    Block(Vector3 c, float s, bool sh=false) : Block(c, {s, s, s}, sh) {
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