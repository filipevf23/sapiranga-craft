#include "engine.h"
#include <iostream>
#include "collision_manager.h"
#include "linear_algebra.h"
#include "utils.h"
#include <math.h>
#include <algorithm>
#include "raymath.h"


using std::cout, std::endl;


void Engine::init() {

    // check 4 vertices of player:
    float px = player.position.x;
    float pz = player.position.z;
    float hsx = player.size.x / 2.0f;
    float hsz = player.size.z / 2.0f;

    float h1 = world.get_height_from_block_and_seed(floor(px-hsx), floor(pz-hsz));
    float h2 = world.get_height_from_block_and_seed(floor(px-hsx), floor(pz+hsz));
    float h3 = world.get_height_from_block_and_seed(floor(px+hsx), floor(pz+hsz));
    float h4 = world.get_height_from_block_and_seed(floor(px+hsx), floor(pz-hsz));

    float fh = std::max({h1, h2, h3, h4});
    player.position.y = fh + player.half_size_y;
    player.target.y = fh + player.half_size_y;
    player.is_falling = false;
    player.fall_speed = 0.0f;
    renderer.set_camera_position(player.eye_position);
    renderer.update_camera(player.eye_position, mouse_sensitivity);
    player.target = renderer.get_target();

    
}

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

        // check 4 vertices of player:
        float px = player.position.x;
        float pz = player.position.z;
        float hsx = player.size.x / 2.0f;
        float hsz = player.size.z / 2.0f;

        float h1 = world.get_height_from_pos(px-hsx, pz-hsz);
        float h2 = world.get_height_from_pos(px-hsx, pz+hsz);
        float h3 = world.get_height_from_pos(px+hsx, pz+hsz);
        float h4 = world.get_height_from_pos(px+hsx, pz-hsz);

        float fh = std::max({h1, h2, h3, h4});

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

    Quad player_object = create_quad(player.position, player.size);
    Vector3 reaction_vector {0,0,0};
    float aim_dist = 1000.0f;
    player.looking_at = -1;

    for (size_t i=0; i<world.objects.size(); ++i) {

        // is player looking at this object?
        RayCollision rc = CollisionManager::get_ray_collision(&player.eye_position, &player.target, &player.size, &world.objects[i]);
        if (rc.hit) {
            if (rc.distance < aim_dist) {
                aim_dist = rc.distance;
                player.looking_at = i;
            }
        }

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

    destroy_quad(&player_object);
    
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