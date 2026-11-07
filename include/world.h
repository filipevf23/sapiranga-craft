#pragma once

#include "raylib.h"
#include <vector>
#include "linear_algebra.h"
#include <cstdint>

#define CHUNK_SIZE 16
#define WORLD_HEIGHT_MIN 0
#define WORLD_HEIGHT_MAX 255

typedef struct Chunk {
    uint8_t blocks[CHUNK_SIZE][CHUNK_SIZE][WORLD_HEIGHT_MAX+1];
    int x;
    int z;
    Model model;
} Chunk;

class World {
public:
    std::vector<Chunk> chunks;
    bool gravity;
    float gravity_force;
    double floor_height = 0.0;
    int seed;
    int render_distance = 12;
    
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
    
    void break_block(Vector4Int &);
    Vector3 get_block_center(int, int, int, int, int);
    int get_height_from_pos(float x, float y, float z);
    Vector4 get_block_from_pos(float x, float y, float z);
    bool does_chunk_exist(int i, int j);
    float noise(int, int);
    std::vector<size_t> chunks_to_generate(float, float);
    void generate_chunk(int, int);
    void generate_overworld();
};