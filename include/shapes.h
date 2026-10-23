// Creation, Manipulation and Drawing of all shapes.

#pragma once

#include "raylib.h"
#include <vector>

enum BlockID {
    B_DIRT,
    B_GRASS,
    B_STONE,
    B_COBBLESTONE,
    B_PODZOL,
    B_WOOD_OAK,
    B_PLANK_OAK,
    B_DROPPER,
};

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
    int texture_id;
    Block(Vector3 c, Vector3 s, int tid, bool act) {
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
        show = false;
        active = act;
        front= back= up= down= left= right= -1;
        texture_id = tid;
    }
    Block(Vector3 c, float s, int tid, bool act) : Block(c, {s, s, s}, tid, act) {
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