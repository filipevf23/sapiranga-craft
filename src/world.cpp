#include "world.h"
#include "map.h"
#include "math.h"
#include <iostream>

World::World(bool _gravity, float _gravity_force) 
: gravity {_gravity} , gravity_force {_gravity_force}
{   
    seed = 1;
    generate_overworld();
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

int World::get_height_from_block_and_seed(int i, int j) {
    return round( sin((i+5)*PI / 20.0f) * cos((j+0.5)*PI / 10.0f) );
}

int World::get_height_from_block_and_seed(Vector3 pos) {
    return get_height_from_block_and_seed((int)pos.x, (int)pos.z);
}

int World::get_height_from_pos(float x, float y, float z) {
    int index;
    int ix = (int)std::floor(x);
    int iz = (int)std::floor(z);
    int iy = (int)std::floor(y);

    for (int i = iy; i >= -256; --i) {
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
    int cx = chunk_x - (-(chunks_x / 2));
    int cy = chunk_y - (-(chunks_y / 2));
    int cz = chunk_z - (-(chunks_z / 2));

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
                hs[i*CHUNK_SIZE + j] = get_height_from_block_and_seed(global_x, global_z);
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
    int chunks = 16;
    chunks_y = 2;
    chunks_x = sqrt(chunks);
    chunks_z = sqrt(chunks);
    int offset_xz = -(chunks_x / 2);
    int offset_y = -(chunks_y / 2);
    for (int i=0 ; i<chunks_x; i++) {
        for (int j=0; j<chunks_z; j++) {
            for (int k=0; k<chunks_y; k++){
                generate_chunk(i+offset_xz, j+offset_xz, k+offset_y);
            }
        }
    }
    std::cout << "created " << objects.size() << " objects." << std::endl;
    get_neighbors();
}