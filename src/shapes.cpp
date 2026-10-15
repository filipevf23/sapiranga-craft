#include "shapes.h"
#include <iostream>

Quad create_quad(Vector3 center, float size) {
    return create_quad(center, (Vector3){.x=size, .y=size, .z=size});
}

Quad create_quad(Vector3 center, Vector3 size) {
    Line *v = new Line[12];
    Vector3 p0 = {center.x-size.x/2.0f,      // 000
                  center.y-size.y/2.0f,
                  center.z-size.z/2.0f};
    Vector3 p1 = {p0.x+size.x, p0.y, p0.z}; // 100
    Vector3 p2 = {p0.x, p0.y+size.y, p0.z}; // 010
    Vector3 p3 = {p0.x, p0.y, p0.z+size.z}; // 001
    Vector3 p4 = {p1.x, p2.y, p0.z};      // 110
    Vector3 p5 = {p1.x, p0.y, p3.z};      // 101
    Vector3 p6 = {p1.x, p2.y, p3.z};      // 111
    Vector3 p7 = {p0.x, p2.y, p3.z};      // 011

    Vector3 *vertices = new Vector3[8];
    vertices[0] = p0;
    vertices[1] = p1;
    vertices[2] = p2;
    vertices[3] = p3;
    vertices[4] = p4;
    vertices[5] = p5;
    vertices[6] = p6;
    vertices[7] = p7;

    v[0] = {p0,p1};
    v[1] = {p0,p2};
    v[2] = {p1,p4};
    v[3] = {p2,p4};
    v[4] = {p3,p7};
    v[5] = {p3,p5};
    v[6] = {p6,p7};
    v[7] = {p6,p5};
    v[8] = {p2,p7};
    v[9] = {p1,p5};
    v[10] = {p6,p4};
    v[11] = {p0,p3};

    Quad q;
    q.lines = v; 
    q.vertices = vertices;
    q.size = size;
    q.center = center;
    q.show = true;

    return q;
}

bool destroy_quad(Quad *quad) {
    delete [] quad->lines;
    delete [] quad->vertices;
    quad->lines = nullptr;
    quad->vertices = nullptr;
    return true;
}

void draw_lines(std::vector<Line> *v) {
    for (auto &l : (*v)) {
        DrawLine3D(l.p0, l.p1, {255, 0, 0, 255});
    }
}
