#pragma once

#include "raylib.h"
#include <vector>

#define NORMAL_SPEED 4.317f
#define SNEAK_SPEED 1.295f
#define SPRINT_SPEED 5.612f

class Player {
public:
    bool is_falling {false};
    bool collision {false};
    bool is_walking {false};
    bool is_sprinting {false};
    bool is_sneaking {false};
    bool is_breaking {false};
    float fall_speed {0.0f};
    float eye_height {0.72}; // from middle of the body
    Vector3 size = { 0.6f, 1.8f, 0.6f };
    float half_size_y = 0.9f;
    Vector3 position = { 2.5f, 90.9f, 4.5f };
    Vector3 eye_position = { 2.5f, 91.62f, 4.6f };
    Vector3 target;
    float speed = NORMAL_SPEED;
    float drag = -SNEAK_SPEED;
    int looking_at; // index of object that player is looking at

    void rotate(double);
    Vector3 move_forward(float = 0.1);
    Vector3 move_backward(float = 0.1);
    Vector3 move_y(float = 0.1);
    Vector3 move_down(float = 0.1); 
    Vector3 move_left(float = 0.1);
    Vector3 move_right(float = 0.1);
    void set_y(float);
    Vector3 collision_reaction(Vector3);

};