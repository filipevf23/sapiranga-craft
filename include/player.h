#pragma once

#include "raylib.h"
#include <vector>

class Player {
public:
    bool is_falling {false};
    bool collision {false};
    float fall_speed {0.0f};
    float eye_height {0.72}; // from middle of the body
    Vector3 size = { 0.2f, 1.8f, 0.2f };
    float half_size_y = 0.9f;
    Vector3 position = { 2.5f, 0.9f, 4.5f };
    Vector3 eye_position = { 2.5f, 1.62f, 4.6f };
    Vector3 target;
    float speed = 5.0f;
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