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

void World::break_block(int i, int j, int k, int c) {
    chunks[c].blocks[i][j][k] = AIR;
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

int World::get_block_index(int global_x, int global_y, int global_z) {
    int chunk_x = (int)floor((float)global_x / CHUNK_SIZE);
    int chunk_z = (int)floor((float)global_z / CHUNK_SIZE);
    
    // revert offset
    int cx = chunk_x - offset_xz;
    int cy = global_y;
    int cz = chunk_z - offset_xz;

    if (cx < 0 || cx >= chunks_x || cy < WORLD_HEIGHT_MIN || cy > WORLD_HEIGHT_MAX || cz < 0 || cz >= chunks_z) {
        return -1;
    }

    int local_x = global_x - chunk_x * CHUNK_SIZE;
    int local_z = global_z - chunk_z * CHUNK_SIZE;

    int chunk_idx = cx * (chunks_z) + cz;
    int block_idx = local_x * (CHUNK_SIZE * (WORLD_HEIGHT_MAX+1)) + local_z * (WORLD_HEIGHT_MAX+1) + global_y;

    return chunk_idx * (CHUNK_SIZE * CHUNK_SIZE * (WORLD_HEIGHT_MAX+1)) + block_idx;
}


Vector3 World::get_block_center(int c_x, int c_z, int i, int j, int k) {
    return Vector3 {c_x + i + 0.5f, k+0.5f, c_z + j + 0.5f};
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

void World::get_neighbors() {

    for (size_t i=0; i<objects.size(); ++i) {
        Block *b = &objects[i];
        
        int gx = (int)floor(b->center.x);
        int gy = (int)floor(b->center.y);
        int gz = (int)floor(b->center.z);

        b->front = get_block_index(gx + 1, gy, gz);
        b->back  = get_block_index(gx - 1, gy, gz);
        b->up    = get_block_index(gx, gy + 1, gz);
        b->down  = get_block_index(gx, gy - 1, gz);
        b->right = get_block_index(gx, gy, gz + 1);
        b->left  = get_block_index(gx, gy, gz - 1);

        if (b->front != -1 && b->back != -1 && b->up != -1 && b->down != -1 && b->left != -1 && b->right != -1) {
            if (objects[b->front].active && objects[b->back].active && objects[b->up].active && objects[b->down].active && objects[b->left].active && objects[b->right].active)
                b->show = false;
        }    
    }
}

void World::generate_overworld() {

    for (int i=0; i<chunks_x; i++) {
        for (int j=0; j<chunks_z; j++) {
            generate_chunk(i + offset_xz, j + offset_xz);
        }
    }

    std::cout << "created " << chunks.size() << " chunks." << std::endl;
    //get_neighbors();
}