#pragma once

#include "raylib.h"
#include <vector>
#include "shapes.h"


class World {
public:
    std::vector<Quad> objects;
    bool gravity;
    double gravity_force;
    double floor_height = 0.0;
    size_t floor_objects_count;
    int seed;

    World(bool=false, double=9.81);
    
    void test_world();
    void break_block(int i);
    int get_height_from_block_and_seed(int, int);
    int get_height_from_block_and_seed(Vector3);
    int get_height_from_pos(float x, float z);
    void generate_chunk(int, int);
    void generate_overworld();
};