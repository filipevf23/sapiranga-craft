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
    renderer.set_camera_position(player.eye_position);
    renderer.update_camera(player.eye_position, mouse_sensitivity);
    player.target = renderer.get_target();

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
    float hsx = player.size.x / 2.0f;
    float hsz = player.size.z / 2.0f;

    float h1 = world.get_height_from_block_and_seed(floor(px-hsx), floor(pz-hsz));
    float h2 = world.get_height_from_block_and_seed(floor(px-hsx), floor(pz+hsz));
    float h3 = world.get_height_from_block_and_seed(floor(px+hsx), floor(pz+hsz));
    float h4 = world.get_height_from_block_and_seed(floor(px+hsx), floor(pz-hsz));

    return std::max({h1, h2, h3, h4});
}

void Engine::frustum_culling(Block *object, const Vector3 *camera_pos) {
    Vector3 v1 {Vector3Subtract(object->center, *camera_pos)};
    Vector3 v2 {Vector3Subtract(renderer.get_target(), *camera_pos)};
    object->show = Vector3DotProduct(v1, v2) > 0.0f && Vector3Distance(object->center, *camera_pos) <= renderer.render_distance;
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
    if (IsKeyDown(KEY_SPACE)) {
        renderer.set_target_position(player.move_up(player.speed * GetFrameTime()));
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
            player.fall_speed += (world.gravity_force * GetFrameTime()) / 10.0f;
            renderer.set_target_position(player.move_down(player.fall_speed));
            if (player.position.y - fh < player.half_size_y) {
                player.position.y = fh + player.half_size_y;
                player.target.y = fh + player.half_size_y;
                player.is_falling = false;
                player.fall_speed = 0.0f;
            }
        }
        
    }

    Block player_object {player.position, player.size};
    Vector3 reaction_vector {0,0,0};
    float aim_dist = 1000.0f;
    player.looking_at = -1;

    Vector3 camera_pos = renderer.get_camera_position();
    for (size_t i=0; i<world.objects.size(); ++i) {
        Block *object = &world.objects[i];

        // Culling Pipeline
        frustum_culling(object, &camera_pos);
        

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
        float player_bottom = player_object.center.y - player.half_size_y;
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
    while(!WindowShouldClose()) {

        check_events();
        update();
        size_t model = 3;
        // trying dropper block
        renderer.render(&world.objects, asset_manager.models.at(model));
    }
    CloseWindow();
}