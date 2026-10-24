#include "world.h"
#include "map.h"
#include "math.h"
#include <iostream>

#define FIRST_OCT_SIZE 512
#define SECOND_OCT_SIZE 512
#define THIRD_OCT_SIZE 256
#define FOURTH_OCT_SIZE 256

World::World(bool _gravity, float _gravity_force) 
: gravity {_gravity} , gravity_force {_gravity_force}
{   
    seed = 1;

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

void World::break_block(int i) {
    objects[i].active = false;
    if (objects[i].front != -1)
        objects[objects[i].front].back = -1;
    if (objects[i].back != -1)
        objects[objects[i].back].front = -1;
    if (objects[i].up != -1)
        objects[objects[i].up].down = -1;
    if (objects[i].down != -1)
        objects[objects[i].down].up = -1;
    if (objects[i].right != -1)
        objects[objects[i].right].left = -1;
    if (objects[i].left != -1)
        objects[objects[i].left].right = -1;
}

int World::get_height_from_pos(float x, float y, float z) {
    int index;
    int ix = (int)std::floor(x);
    int iz = (int)std::floor(z);
    int iy = (int)std::floor(y);

    for (int i=iy; i>=-256; --i) {
        if ((index = get_block_index(ix, i, iz)) != -1 && objects[index].active) {
            return i;
        }
    }
    return -256;
}

int World::get_block_index(int global_x, int global_y, int global_z) {
    int chunk_x = (int)floor((float)global_x / CHUNK_SIZE);
    int chunk_y = (int)floor((float)global_y / CHUNK_SIZE);
    int chunk_z = (int)floor((float)global_z / CHUNK_SIZE);
    
    // revert offset
    int cx = chunk_x - offset_xz;
    int cy = chunk_y - height_offset;
    int cz = chunk_z - offset_xz;

    if (cx < 0 || cx >= chunks_x || cy < 0 || cy >= chunks_y || cz < 0 || cz >= chunks_z) {
        return -1;
    }

    int local_x = global_x - chunk_x * CHUNK_SIZE;
    int local_y = global_y - chunk_y * CHUNK_SIZE;
    int local_z = global_z - chunk_z * CHUNK_SIZE;

    int chunk_idx = cx * (chunks_z * chunks_y) + cz * chunks_y + cy;
    int block_idx = local_x * (CHUNK_SIZE * CHUNK_SIZE) + local_z * CHUNK_SIZE + local_y;

    return chunk_idx * (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) + block_idx;
}


void World::generate_chunk(int x, int z, int y) {
    int hs[CHUNK_SIZE * CHUNK_SIZE];

    // Creating Blocks
    for (int i=0; i<CHUNK_SIZE; i++) {
        for (int j=0; j<CHUNK_SIZE; j++) {
            for (int k=0; k<CHUNK_SIZE; k++) {
                int global_x = x * CHUNK_SIZE + i;
                int global_y = y * CHUNK_SIZE + k;
                int global_z = z * CHUNK_SIZE + j;
                hs[i*CHUNK_SIZE + j] = (int)std::floor(noise(global_x, global_z));
                bool active = global_y <= hs[i*CHUNK_SIZE + j];
                Block b {{(float)global_x + 0.5f, (float)global_y + 0.5f, (float)global_z + 0.5f}, 1.0f, B_PODZOL, active};
                objects.push_back(b);
            }
        }
    }
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
    chunks_y = 2;
    chunks_x = 4;
    chunks_z = 4;
    offset_xz = -(chunks_x / 2);

    for (int i=0; i<chunks_x; i++) {
        for (int j=0; j<chunks_z; j++) {
            for (int k=0; k<chunks_y; k++){
                generate_chunk(i + offset_xz, j + offset_xz, k + height_offset);
            }
        }
    }

    std::cout << "created " << objects.size() << " objects." << std::endl;
    get_neighbors();
}