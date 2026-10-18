#include "world.h"
#include "map.h"
#include "math.h"
#include <iostream>

World::World(bool _gravity, double _gravity_force) 
: gravity {_gravity} , gravity_force {_gravity_force}
{   
    seed = 1;
    generate_overworld();
}

void World::test_world() {
    // floor
    for (int i=0; i<MAP_Y; i++) {
        for (int j=0; j<MAP_X; j++) {
            for (int k=0; k<8; k++) {
                objects.push_back({{i+0.5f, (-k-0.5f), j+0.5f}, 1.0f});
            }
        }
    }

    floor_objects_count = MAP_X * MAP_Y * 8;

    // walls
    for (int i=0; i<MAP_Y; i++) {
        for (int j=0; j<MAP_X; j++) {
            if (Map[i][j] == 0) continue;
            objects.push_back({{i+0.5f, 0.5f, j+0.5f}, 1.0f});
        }
    }
}

void World::break_block(int i) {
    objects.erase(objects.begin() + i);
    floor_objects_count--;
}

int World::get_height_from_block_and_seed(int i, int j) {
    return round( sin((i+5)*PI / 20.0f) * cos((j+0.5)*PI / 10.0f) );
}

int World::get_height_from_block_and_seed(Vector3 pos) {
    return get_height_from_block_and_seed((int)pos.x, (int)pos.z);
}

int World::get_height_from_pos(float x, float z) {
    int ix = (int)floor(x);
    int iz = (int)floor(z);
    int height = -1000;
    for (size_t i=0; i<objects.size(); ++i) {
        if ((int)floor(objects[i].center.x) != ix || (int)floor(objects[i].center.z) != iz)
            continue;
        if ((int)ceil(objects[i].center.y) > height) {
            height = (int)ceil(objects[i].center.y);
        }
    }
    return height;
}

void World::generate_chunk(int x, int z) {
    int hs[256];
    int chunk_size = 16;

    for (int i=0; i<chunk_size; i++) {
        for (int j=0; j<chunk_size; j++) {
            int global_x = x * chunk_size + i;
            int global_z = z * chunk_size + j;
            hs[i*chunk_size + j] = get_height_from_block_and_seed(global_x, global_z);
        }
    }

    for (int i=0; i<chunk_size; i++) {
        for (int j=0; j<chunk_size; j++) {
            for (int k=0; k<chunk_size; k++) {
                float real_x = x * chunk_size + i + 0.5f;
                float real_z = z * chunk_size + j + 0.5f;
                float real_y = hs[i*chunk_size + j] - k - 0.5f;
                objects.push_back({{real_x, real_y, real_z}, 1.0f});
            }
        }
    }
}

void World::generate_overworld() {
    int chunks = 16;
    for (int i=-2 ; i<sqrt(chunks)-2; i++) {
        for (int j=-2 ; j<sqrt(chunks)-2; j++)
        generate_chunk(i, j);
    }
    floor_objects_count = 16*16*16 * chunks;
    std::cout << "created " << objects.size() << " objects." << std::endl;
}