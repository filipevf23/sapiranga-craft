#include "engine.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include "raymath.h"
#include "collision_manager.h"
#include "linear_algebra.h"
#include "utils.h"


using std::cout, std::endl;


void Engine::init() {
    float fh = get_player_floor();
    player.position.y = fh + player.half_size_y;
    player.target.y = player.position.y + player.eye_height;
    player.is_falling = false;
    player.fall_speed = 0.0f;
    player.eye_position = {player.position.x, player.position.y + player.eye_height, player.position.z + 0.1f};
    renderer.set_camera_position(player.eye_position);
    renderer.update_camera(player.eye_position, mouse_sensitivity);
    player.target = renderer.get_target();
    for (auto &c : world.chunks) {
        generate_chunk_mesh(&c);
    }
    cout << "Engine Initialized" << endl;
}



//////////////  Util  ////////////////

bool Engine::is_face_visible(const Vector3 *face_center, const Vector3 *object_center, const Vector3 *camera_pos) {
    Vector3 normal = Vector3Subtract(*face_center, *object_center);
    Vector3 sight = Vector3Subtract(*camera_pos, *face_center);
    return Vector3DotProduct(normal, sight) > 0;
}

float Engine::get_player_floor() {
    // check 4 vertices of player:
    float px = player.position.x;
    float pz = player.position.z;
    float hsx = player.size.x / 2.0f;
    float hsz = player.size.z / 2.0f;

    float h1 = world.get_height_from_pos(px-hsx, 256, pz-hsz);
    float h2 = world.get_height_from_pos(px-hsx, 256, pz+hsz);
    float h3 = world.get_height_from_pos(px+hsx, 256, pz+hsz);
    float h4 = world.get_height_from_pos(px+hsx, 256, pz-hsz);

    float floor = std::max({h1, h2, h3, h4});
    if (floor<=-256) return -256;
    return floor + 1.0f;
}

void Engine::generate_chunk_mesh(Chunk *c) {
    std::vector<Vector3> mesh_vertices; // vertices
    std::vector<int> face_texture_ids;  // texture_ids
    Vector3 normal_front {0.5f, 0.0f, 0.0f};
    Vector3 normal_back {-0.5f, 0.0f, 0.0f};
    Vector3 normal_up {0.0f, 0.5f, 0.0f};
    Vector3 normal_down {0.0f, -0.5f, 0.0f};
    Vector3 normal_right {0.0f, 0.0f, 0.5f};
    Vector3 normal_left {0.0f, 0.0f, -0.5f};

    // Loop Through Blocks
    for (int i=0; i<CHUNK_SIZE; ++i) {
        for (int j=0; j<CHUNK_SIZE; ++j) {
            for (int k=0; k<=WORLD_HEIGHT_MAX; ++k) {

                if (c->blocks[i][j][k] == ItemID::AIR) continue;

                int front = i+1;
                int back = i-1;
                int left = j-1;
                int right = j+1;
                int up = k+1;
                int down = k-1;

                Vector3 center = world.get_block_center(c->x, c->z, i, j, k);
                Vector3 faces[6] {
                    Vector3Add(center, normal_front),
                    Vector3Add(center, normal_back),
                    Vector3Add(center, normal_up),
                    Vector3Add(center, normal_down),
                    Vector3Add(center, normal_right),
                    Vector3Add(center, normal_left)
                };

                bool draw_front = front >= CHUNK_SIZE ? true : c->blocks[front][j][k] == ItemID::AIR;
                bool draw_back = back < 0 ? true : c->blocks[back][j][k] == ItemID::AIR;
                bool draw_up = up >= WORLD_HEIGHT_MAX ? true : c->blocks[i][j][up] == ItemID::AIR;
                bool draw_down = down < 0 ? true: c->blocks[i][j][down] == ItemID::AIR;
                bool draw_right = right >= CHUNK_SIZE ? true : c->blocks[i][right][k] == ItemID::AIR;
                bool draw_left = left < 0 ? true : c->blocks[i][left][k] == ItemID::AIR;

                if (!draw_front && !draw_back && !draw_up && !draw_down && !draw_right && !draw_left) {
                    continue;
                }

                int ids[6];
                asset_manager.get_block_textures(c->blocks[i][j][k], ids);

                if (draw_front) { 
                    mesh_vertices.push_back(Vector3Add(faces[0], {0.0f, 0.5f, 0.5f}));   // TL
                    mesh_vertices.push_back(Vector3Add(faces[0], {0.0f, -0.5f, 0.5f}));  // BL
                    mesh_vertices.push_back(Vector3Add(faces[0], {0.0f, -0.5f, -0.5f})); // BR
                    mesh_vertices.push_back(Vector3Add(faces[0], {0.0f, 0.5f, -0.5f}));  // TR
                    face_texture_ids.push_back(ids[0]);
                }
                if (draw_back) { 
                    mesh_vertices.push_back(Vector3Add(faces[1], {0.0f, 0.5f, -0.5f}));  // TL
                    mesh_vertices.push_back(Vector3Add(faces[1], {0.0f, -0.5f, -0.5f})); // BL
                    mesh_vertices.push_back(Vector3Add(faces[1], {0.0f, -0.5f, 0.5f}));  // BR
                    mesh_vertices.push_back(Vector3Add(faces[1], {0.0f, 0.5f, 0.5f}));   // TR
                    face_texture_ids.push_back(ids[1]);
                }
                if (draw_up) { 
                    mesh_vertices.push_back(Vector3Add(faces[2], {-0.5f, 0.0f, -0.5f})); // TL
                    mesh_vertices.push_back(Vector3Add(faces[2], {-0.5f, 0.0f, 0.5f}));  // BL
                    mesh_vertices.push_back(Vector3Add(faces[2], {0.5f, 0.0f, 0.5f}));   // BR
                    mesh_vertices.push_back(Vector3Add(faces[2], {0.5f, 0.0f, -0.5f}));  // TR
                    face_texture_ids.push_back(ids[2]);
                }
                if (draw_down) { 
                    mesh_vertices.push_back(Vector3Add(faces[3], {-0.5f, 0.0f, 0.5f}));  // TL
                    mesh_vertices.push_back(Vector3Add(faces[3], {-0.5f, 0.0f, -0.5f})); // BL
                    mesh_vertices.push_back(Vector3Add(faces[3], {0.5f, 0.0f, -0.5f}));  // BR
                    mesh_vertices.push_back(Vector3Add(faces[3], {0.5f, 0.0f, 0.5f}));   // TR
                    face_texture_ids.push_back(ids[3]);
                }
                if (draw_right) { 
                    mesh_vertices.push_back(Vector3Add(faces[4], {-0.5f, 0.5f, 0.0f}));  // TL
                    mesh_vertices.push_back(Vector3Add(faces[4], {-0.5f, -0.5f, 0.0f})); // BL
                    mesh_vertices.push_back(Vector3Add(faces[4], {0.5f, -0.5f, 0.0f}));  // BR
                    mesh_vertices.push_back(Vector3Add(faces[4], {0.5f, 0.5f, 0.0f}));   // TR
                    face_texture_ids.push_back(ids[4]);
                }
                if (draw_left) { 
                    mesh_vertices.push_back(Vector3Add(faces[5], {0.5f, 0.5f, 0.0f}));   // TL
                    mesh_vertices.push_back(Vector3Add(faces[5], {0.5f, -0.5f, 0.0f}));  // BL
                    mesh_vertices.push_back(Vector3Add(faces[5], {-0.5f, -0.5f, 0.0f})); // BR
                    mesh_vertices.push_back(Vector3Add(faces[5], {-0.5f, 0.5f, 0.0f}));  // TR
                    face_texture_ids.push_back(ids[5]);
                }
            }
        }
    }
    cout << mesh_vertices.size() << " | " << face_texture_ids.size() << endl;
    c->model = asset_manager.create_complex_model(mesh_vertices, face_texture_ids);
}


/////////  Engine Pipeline  //////////

void Engine::check_events() {

    if (IsKeyDown(KEY_E)) {
        player.is_inventory_open = !player.is_inventory_open;
        // TODO: Change Camera to stop following player and become 2D with mouse showing!
    }

    

    if (player.is_inventory_open) {

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            // TODO: Hold item with mouse
        }

        return;
    }
        

    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        player.is_sneaking = true;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        player.is_sprinting = true;
        player.is_sneaking = false;
    }

    if (IsKeyDown(KEY_A)) {
        player.is_walking = true;
        player.speed = NORMAL_SPEED;
        if (player.is_sneaking)
            player.speed = SNEAK_SPEED;
        float ds = player.speed * GetFrameTime();
        renderer.set_target_position(player.move_left(ds));
    }
    
    else if (IsKeyDown(KEY_D)) {
        player.is_walking = true;
        player.speed = NORMAL_SPEED;
        if (player.is_sneaking)
            player.speed = SNEAK_SPEED;
        float ds = player.speed * GetFrameTime();
        renderer.set_target_position(player.move_right(ds));
    }

    if (IsKeyDown(KEY_W)) {
        player.is_walking = true;
        player.speed = NORMAL_SPEED;
        if (player.is_sneaking)
            player.speed = SNEAK_SPEED;
        if (player.is_sprinting)
            player.speed = SPRINT_SPEED;
        float ds = player.speed * GetFrameTime();
        renderer.set_target_position(player.move_forward(ds));
    }

    else if (IsKeyDown(KEY_S)) {
        player.is_walking = true;
        player.speed = NORMAL_SPEED;
        if (player.is_sneaking)
            player.speed = SNEAK_SPEED;
        float ds = player.speed * GetFrameTime();
        renderer.set_target_position(player.move_backward(ds));
    }
    if (IsKeyDown(KEY_SPACE) && !player.is_falling) {
        player.fall_speed = sqrt(2 * world.gravity_force * 1.25);
        player.is_falling = true;
    }
    
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        break_block_command = true;
    }

}

void Engine::update() {

    if (world.gravity) {

        float fh = get_player_floor();

        if (player.position.y - player.half_size_y - fh > 0.1) {
            player.is_falling = true;
        }
        if (player.is_falling) {
            float time = GetFrameTime();
            player.fall_speed -= (world.gravity_force * time);
            float dy = player.fall_speed * time;
            renderer.set_target_position(player.move_y(dy));
            if (player.position.y - fh < player.half_size_y && player.fall_speed < 0.0f) {
                fh += 0.005;
                player.set_y(fh + player.half_size_y);
                player.is_falling = false;
                player.fall_speed = 0.0f;
            }
        }
    }

    if (player.is_walking) {
        std::vector<size_t> new_chunks = world.chunks_to_generate(player.position.x, player.position.z);
        for (auto i: new_chunks) {
            generate_chunk_mesh(&world.chunks[i]);
        }
    }

    // Is no longer actively walking
    if (!player.is_walking && player.speed > 0.0f) {
        player.is_sprinting = false;
        player.speed -= player.drag;
        if (player.speed < 0.0f) {
            player.speed = 0.0f;
        }
    }
    player.is_sneaking = false;
    player.is_walking = false;

    BoundingBox player_object = CollisionManager::get_bounding_box(&player.position, &player.size);
    Vector3 reaction_vector {0,0,0};
    float aim_dist = 32.0f;
    player.looking_at = {-1,-1,-1,-1};

    for (size_t c=0; c<world.chunks.size(); ++c) {
        Chunk &chunk = world.chunks[c];

        // If chunk is too far from player
        if ( Algebra::distance2({(int)player.position.x, (int)player.position.z}, {chunk.x, chunk.z}) > CHUNK_SIZE ) {
            continue;
        }

        for (int i=0; i<CHUNK_SIZE; ++i) {
            for (int j=0; j<CHUNK_SIZE; ++j) {
                for (int k=0; k<=WORLD_HEIGHT_MAX; ++k) {
                    if (chunk.blocks[i][j][k] == ItemID::AIR || Vector3Distance(player.position, world.get_block_center(chunk.x,chunk.z,i,j,k)) > 5.0f) {
                        continue;
                    }

                    // is player looking at this object?
                    Vector3 size = {1,1,1};
                    Vector3 center = world.get_block_center(chunk.x, chunk.z, i, j, k);
                    BoundingBox bbb = CollisionManager::get_bounding_box(&center, &size);
                    RayCollision rc = CollisionManager::get_ray_collision(&player.eye_position, &player.target, &bbb);
                    if (rc.hit) {
                        if (rc.distance < aim_dist) {
                            aim_dist = rc.distance;
                            player.looking_at = {i, j, k, (int)c};
                        }
                    }

                    
                    // Collision
                    float block_top = center.y + size.y/2.0f;
                    float player_bottom = player.position.y - player.half_size_y;
                    if (block_top <= player_bottom + 0.005f) {
                        continue;
                    }

                    if (CollisionManager::check_collision(&player_object, &bbb)) {
                        player.collision = true;
                        reaction_vector = Vector3Add(reaction_vector,(CollisionManager::get_reaction_vector(&player_object, &bbb)));
                    }

                }
            }
        } 
    }

    
    // mark object player is looking at
    if (player.looking_at.d != -1 && break_block_command) {
        world.break_block(player.looking_at);
        generate_chunk_mesh(&world.chunks[player.looking_at.d]);
        player.looking_at = {-1,-1,-1,-1};
        break_block_command = false;
    }

    if (player.collision) {
        renderer.set_target_position(player.collision_reaction(reaction_vector));
        player.collision = false;
    }
    
    renderer.set_camera_position(player.eye_position);
    renderer.update_camera(player.eye_position, mouse_sensitivity);
    player.target = renderer.get_target();
    
}

void Engine::draw() {
    BeginDrawing();
        ClearBackground(bg_color);
        BeginMode3D(renderer.camera);
            renderer.render_chunks(world.chunks);
        EndMode3D();
        DrawFPS(10,10);
        renderer.show_crosshair();
        if (player.is_inventory_open) {
            renderer.show_inventory(player.inventory);
        }
    EndDrawing();
}

void Engine::run() {
    init();
    while(!WindowShouldClose()) {
        check_events();
        update();
        draw();
    }
    CloseWindow();
}