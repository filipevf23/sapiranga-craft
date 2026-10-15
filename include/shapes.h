// Creation, Manipulation and Drawing of all shapes.

#pragma once

#include "math_types.h"
#include <vector>

Quad create_quad(Vector3 center, float size);
Quad create_quad(Vector3 center, Vector3 size);
bool destroy_quad(Quad*);
void draw_lines(std::vector<Line> *v);