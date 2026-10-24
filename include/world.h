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
    int height_offset = 100 / CHUNK_SIZE;

    Image pn_first_oct;
    Image pn_second_oct;
    Image pn_third_oct;
    Image pn_fourth_oct;

    std::vector<int> pn_first_data;
    std::vector<int> pn_second_data;
    std::vector<int> pn_third_data;
    std::vector<int> pn_fourth_data;

    World(bool=false, float=9.81f);
    ~World();
    
    void break_block(int i);
    int get_height_from_pos(float x, float y, float z);
    float noise(int, int);
    void generate_chunk(int, int, int);
    int get_block_index(int global_x, int global_y, int global_z);
    void get_neighbors();
    void generate_overworld();
};