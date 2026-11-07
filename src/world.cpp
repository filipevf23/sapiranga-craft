#include "world.h"
#include "map.h"
#include "math.h"
#include <iostream>
#include "utils.h"

#define FIRST_OCT_SIZE 512
#define SECOND_OCT_SIZE 512
#define THIRD_OCT_SIZE 256
#define FOURTH_OCT_SIZE 256

World::World(bool _gravity, float _gravity_force) 
: gravity {_gravity} , gravity_force {_gravity_force}
{   
    seed = 1;
    chunks_x = render_distance;
    chunks_z = render_distance;
    offset_xz = -(chunks_x / 2);

    pn_first_oct = GenImagePerlinNoise(FIRST_OCT_SIZE, FIRST_OCT_SIZE, 0, 0, 120.0f);
    for (int i=0; i<FIRST_OCT_SIZE; ++i) {
        for (int j=0; j<FIRST_OCT_SIZE; ++j) {
            pn_first_data.push_back(GetImageColor(pn_first_oct, i, j).r);
        }
    }
    pn_second_oct = GenImagePerlinNoise(SECOND_OCT_SIZE, SECOND_OCT_SIZE, 0, 0, 40.0f);
    for (int i=0; i<SECOND_OCT_SIZE; ++i) {
        for (int j=0; j<SECOND_OCT_SIZE; ++j) {
            pn_second_data.push_back(GetImageColor(pn_second_oct, i, j).r);
        }
    }
    pn_third_oct = GenImagePerlinNoise(THIRD_OCT_SIZE, THIRD_OCT_SIZE, 0, 0, 20.0f);
    for (int i=0; i<THIRD_OCT_SIZE; ++i) {
        for (int j=0; j<THIRD_OCT_SIZE; ++j) {
            pn_third_data.push_back(GetImageColor(pn_third_oct, i, j).r);
        }
    }
    pn_fourth_oct = GenImagePerlinNoise(FOURTH_OCT_SIZE, FOURTH_OCT_SIZE, 0, 0, 10.0f);
    for (int i=0; i<FOURTH_OCT_SIZE; ++i) {
        for (int j=0; j<FOURTH_OCT_SIZE; ++j) {
            pn_fourth_data.push_back(GetImageColor(pn_fourth_oct, i, j).r);
        }
    }

    generate_overworld();
}

World::~World() {
    UnloadImage(pn_first_oct);
    UnloadImage(pn_second_oct);
    UnloadImage(pn_third_oct);
    UnloadImage(pn_fourth_oct);
}

float World::noise(int x, int z) {

    int cx = x / CHUNK_SIZE;
    int cz = z / CHUNK_SIZE;

    int x1 = ((cx / 4) % FIRST_OCT_SIZE)/2 + FIRST_OCT_SIZE/2 ;
    int z1 = ((cz / 4) % FIRST_OCT_SIZE)/2 + FIRST_OCT_SIZE/2;
    int c1 = pn_first_data[x1*FIRST_OCT_SIZE + z1];

    int x2 = (cz % SECOND_OCT_SIZE)/2 + SECOND_OCT_SIZE/2;
    int z2 = (cz % SECOND_OCT_SIZE)/2 + SECOND_OCT_SIZE/2;
    int c2 = pn_second_data[x2*SECOND_OCT_SIZE + z2];

    int x3 = ((x / 4) % THIRD_OCT_SIZE)/2 + THIRD_OCT_SIZE/2;
    int z3 = ((z / 4) % THIRD_OCT_SIZE)/2 + THIRD_OCT_SIZE/2;
    int c3 = pn_third_data[x3*THIRD_OCT_SIZE + z3];

    int x4 = (x % FOURTH_OCT_SIZE)/2 + FOURTH_OCT_SIZE/2;
    int z4 = (z % FOURTH_OCT_SIZE)/2 + FOURTH_OCT_SIZE/2;
    int c4 = pn_fourth_data[x4*FOURTH_OCT_SIZE + z4];

    float pixel = (c1 + c2 + c3 + c4) / (4.0f*255.0f);
    float offset = 70;
    float size = 70;
    return (pixel * size) + offset;
}

void World::break_block(Vector4Int &block) {
    chunks[block.d].blocks[block.a][block.b][block.c] = AIR;
}

int World::get_height_from_pos(float x, float y, float z) {

    int ix = (int)std::floor(x);
    int iz = (int)std::floor(z);
    int iy = (int)std::floor(y);

    if (iy > WORLD_HEIGHT_MAX) {
        iy = WORLD_HEIGHT_MAX;
    }

    for (auto &c : chunks) {
        if (c.x <= ix && (c.x+CHUNK_SIZE) > ix && c.z <= iz && (c.z+CHUNK_SIZE) > iz) {
            for (int height=iy; height>WORLD_HEIGHT_MIN; --height) {
                if (c.blocks[ix - c.x][iz - c.z][height] != AIR)
                    return height;
            }
        }
    }

    return WORLD_HEIGHT_MIN;
}

Vector3 World::get_block_center(int c_x, int c_z, int i, int j, int k) {
    return Vector3 {c_x + i + 0.5f, k+0.5f, c_z + j + 0.5f};
}

Vector4 World::get_block_from_pos(float x, float y, float z) {
    Vector4 indices; // i, j, k, chunk_index
    int ix = (int)floor(x);
    int iy = (int)floor(y);
    int iz = (int)floor(z);
    for (size_t chunk = 0; chunk < chunks.size(); ++chunk) {
        Chunk &c = chunks[chunk];
        if (c.x <= ix && (c.x + CHUNK_SIZE) > ix && c.z <= iz && (c.z + CHUNK_SIZE) > iz) {
            ix -= c.x;
            iz -= c.z;
            indices.x = ix;
            indices.y = iy;
            indices.z = iz;
            indices.w = chunk;
            return indices;
        }
    }
    // if not found
    return {-1,-1,-1,-1};
}

bool World::does_chunk_exist(int i, int j) {
    for (auto &c: chunks) {
        if (c.x == i*CHUNK_SIZE && c.z == j*CHUNK_SIZE)
            return true;
    }
    return false;
}

std::vector<size_t> World::chunks_to_generate(float pos_x, float pos_z) {
    // Center = (pos_x, pos_z);
    // x^2 + z^2 = r^2;
    std::vector<size_t> indices;
    int ipos_x = (int)floor(pos_x) / CHUNK_SIZE;
    int ipos_z = (int)floor(pos_z) / CHUNK_SIZE;
    for (int x = -render_distance; x <= render_distance; ++x) {
        for (int z = -render_distance; z <= render_distance; ++z) {
            if (x*x + z*z <= pow(render_distance,2)) {
                size_t i = ipos_x + x;
                size_t j = ipos_z + z;
                if (!does_chunk_exist(i, j)) {
                    indices.push_back(chunks.size()); // add index to array so engine can call for its model to me created!
                    generate_chunk(i, j);
                }
            }
        }
    }
    return indices;
}

void World::generate_chunk(int x, int z) {
    int hs[CHUNK_SIZE * CHUNK_SIZE];
    Chunk chunk;
    // Creating Blocks
    for (int i=0; i<CHUNK_SIZE; i++) {
        for (int j=0; j<CHUNK_SIZE; j++) {
            int global_x = x * CHUNK_SIZE + i;
            int global_z = z * CHUNK_SIZE + j;
            hs[i*CHUNK_SIZE + j] = (int)std::floor(noise(global_x, global_z));
            for (int height=WORLD_HEIGHT_MIN; height<=WORLD_HEIGHT_MAX; height++) {
                BlockID id;
                if (height > hs[i*CHUNK_SIZE + j]) {
                    id = AIR;
                } else {
                    id = height == hs[i*CHUNK_SIZE + j] ? B_GRASS : B_DIRT;
                }
                chunk.blocks[i][j][height] = id;
            }
        }
    }
    chunk.x = x * CHUNK_SIZE;
    chunk.z = z * CHUNK_SIZE;
    chunks.push_back(chunk);
}

void World::generate_overworld() {
    chunks_to_generate(0,0);
    std::cout << "created " << chunks.size() << " chunks." << std::endl;
}