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
    player.target.y = fh + player.half_size_y;
    player.is_falling = false;
    player.fall_speed = 0.0f;
    player.eye_position = {player.position.x, player.position.y + player.eye_height, player.position.z + 0.1f};
    renderer.set_camera_position(player.eye_position);
    renderer.update_camera(player.eye_position, mouse_sensitivity);
    player.target = renderer.get_target();
    generate_map_mesh();
}


//////////////  Util  ///////////////

bool Engine::is_face_visible(const Vector3 *face_center, const Vector3 *object_center, const Vector3 *camera_pos) {
    Vector3 normal = Vector3Subtract(*face_center, *object_center);
    Vector3 sight = Vector3Subtract(*camera_pos, *face_center);
    return Vector3DotProduct(normal, sight) > 0;
}

float Engine::get_player_floor() {
    // check 4 vertices of player:
    float px = player.position.x;
    float pz = player.position.z;
    float py = player.position.y;
    float hsx = player.size.x / 2.0f;
    float hsy = player.size.y / 2.0f;
    float hsz = player.size.z / 2.0f;

    float h1 = world.get_height_from_pos(px-hsx, py-hsy, pz-hsz);
    float h2 = world.get_height_from_pos(px-hsx, py-hsy, pz+hsz);
    float h3 = world.get_height_from_pos(px+hsx, py-hsy, pz+hsz);
    float h4 = world.get_height_from_pos(px+hsx, py-hsy, pz-hsz);

    float floor = std::max({h1, h2, h3, h4});
    if (floor<=-256) return -256;
    return floor + 1.0f;
}

void Engine::generate_map_mesh() {
    std::vector<Vector3> mesh_vertices; // vertices
    std::vector<int> face_texture_ids;  // texture_ids

    for (size_t i=0; i<world.objects.size(); ++i) {
        Block *object = &world.objects[i];

        if (!object->active) continue;

        bool draw_front = object->front == -1 || !world.objects[object->front].active;
        bool draw_back = object->back == -1 || !world.objects[object->back].active;
        bool draw_up = object->up == -1 || !world.objects[object->up].active;
        bool draw_down = object->down == -1 || !world.objects[object->down].active;
        bool draw_right = object->right == -1 || !world.objects[object->right].active;
        bool draw_left = object->left == -1 || !world.objects[object->left].active;

        if (!draw_front && !draw_back && !draw_up && !draw_down && !draw_right && !draw_left) {
            continue;
        }

        
        Vector3 normal_front {0.5f, 0.0f, 0.0f};
        Vector3 normal_back {-0.5f, 0.0f, 0.0f};
        Vector3 normal_up {0.0f, 0.5f, 0.0f};
        Vector3 normal_down {0.0f, -0.5f, 0.0f};
        Vector3 normal_right {0.0f, 0.0f, 0.5f};
        Vector3 normal_left {0.0f, 0.0f, -0.5f};
    
        Vector3 faces[6] {
            Vector3Add(object->center, normal_front),
            Vector3Add(object->center, normal_back),
            Vector3Add(object->center, normal_up),
            Vector3Add(object->center, normal_down),
            Vector3Add(object->center, normal_right),
            Vector3Add(object->center, normal_left)
        };

        int ids[6];
        asset_manager.get_block_textures(object->texture_id, ids);

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

    map_model = asset_manager.create_complex_model(mesh_vertices, face_texture_ids);
}

//////////////  Engine Pipeline   ///////////////

void Engine::check_events() {
    if (IsKeyDown(KEY_A)) {
        renderer.set_target_position(player.move_left(player.speed * GetFrameTime()));
    } else if (IsKeyDown(KEY_D)) {
        renderer.set_target_position(player.move_right(player.speed * GetFrameTime()));
    }
    if (IsKeyDown(KEY_W)) {
        renderer.set_target_position(player.move_forward(player.speed * GetFrameTime()));
    }
    else if (IsKeyDown(KEY_S)) {
        renderer.set_target_position(player.move_backward(player.speed * GetFrameTime()));
    }
    if (IsKeyDown(KEY_SPACE) && !player.is_falling) {
        player.fall_speed = sqrt(2 * world.gravity_force * 1.25);
        player.is_falling = true;
    }
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        // renderer.set_target_position(player.move_down(player.speed * GetFrameTime()));
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
            if (player.position.y - fh < player.half_size_y) {
                player.position.y = fh + player.half_size_y;
                player.target.y = fh + player.half_size_y;
                player.is_falling = false;
                player.fall_speed = 0.0f;
            }
        }
    }

    BoundingBox player_object = CollisionManager::get_bounding_box(&player.position, &player.size);
    Vector3 reaction_vector {0,0,0};
    float aim_dist = 1000.0f;
    player.looking_at = -1;

    for (size_t i=0; i<world.objects.size(); ++i) {
        Block *object = &world.objects[i];
        if (!object->active)
            continue;

        // is player looking at this object?
        RayCollision rc = CollisionManager::get_ray_collision(&player.eye_position, &player.target, &object->bb);
        if (rc.hit) {
            if (rc.distance < aim_dist) {
                aim_dist = rc.distance;
                player.looking_at = i;
            }
        }

        // Collision
        float block_top = world.objects[i].center.y + world.objects[i].size.y/2.0f;
        float player_bottom = player.position.y - player.half_size_y;
        if (block_top <= player_bottom + 0.05f) {
            continue;
        }

        if (CollisionManager::check_collision(&player_object, &world.objects[i])) {
            player.collision = true;
            reaction_vector = Vector3Add(reaction_vector,(CollisionManager::get_reaction_vector(&player_object, &world.objects[i])));
        }
    }
    
    // mark object player is looking at
    renderer.specific_render(player.looking_at);
    if (player.looking_at != -1 && break_block_command) {
        world.break_block(player.looking_at);
        player.looking_at = -1;
        renderer.specific_render(-1);
        break_block_command = false;

        asset_manager.unload_mesh();
        generate_map_mesh();
    }

    if (player.collision) {
        renderer.set_target_position(player.collision_reaction(reaction_vector));
        player.collision = false;
    }
    
    renderer.set_camera_position(player.eye_position);
    renderer.update_camera(player.eye_position, mouse_sensitivity);
    player.target = renderer.get_target();
}

void Engine::run() {
    init();
    while(!WindowShouldClose()) {

        check_events();
        update();
        renderer.render(map_model);
    }
    UnloadModel(map_model);
    CloseWindow();
}