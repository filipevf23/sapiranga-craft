#pragma once

#include "raylib.h"
#include <vector>
#include "shapes.h"

#define CHUNK_SIZE 16

class World {
public:
    std::vector<Block> objects;
    bool gravity;
    float gravity_force;
    double floor_height = 0.0;
    int seed;
    int chunks_y;
    int chunks_x;
    int chunks_z;
    int offset_xz;
    int height_offset = 70 / CHUNK_SIZE;
    Image perlin_noise;

    World(bool=false, float=9.81f);
    
    void break_block(int i);
    int get_height_from_pos(float x, float y, float z);
    float noise(int, int);
    void generate_chunk(int, int, int);
    int get_block_index(int global_x, int global_y, int global_z);
    void get_neighbors();
    void generate_overworld();
};