#include "player.h"
#include "linear_algebra.h"
#include "raymath.h"

void Player::rotate(double angle) {
    Algebra::rotate_yy(angle, &target, eye_position);
}

Vector3 Player::move_forward(float s) {
    Vector3 movement {
        .x = target.x - position.x,
        .y = 0.0f,
        .z = target.z - position.z
    };
    if (Algebra::norm(movement) > 0.001f) {
        movement = Algebra::normalize(movement);
        movement = Vector3Scale(movement, s);
        position = Vector3Add(position, movement);
        eye_position = Vector3Add(eye_position, movement);
        target = Vector3Add(target, movement);
    }
    return target;
}

Vector3 Player::move_backward(float s) {
    Vector3 movement {
        .x = target.x - position.x,
        .y = 0.0f,
        .z = target.z - position.z
    };
    if (Algebra::norm(movement) > 0.001f) {
        movement = Algebra::normalize(movement);
        movement = Vector3Scale(movement, s);
        position = Vector3Subtract(position, movement);
        eye_position = Vector3Subtract(eye_position, movement);
        target = Vector3Subtract(target, movement);
    }
    return target;
}

Vector3 Player::move_y(float s) {
    position.y += s;
    eye_position.y += s;
    target.y += s;
    return target;
}

Vector3 Player::move_down(float s) {
    position.y -= s;
    eye_position.y -= s;
    target.y -= s;
    return target;
}

Vector3 Player::move_left(float s) {
    Vector3 forward {.x = target.x - position.x,
                .y = 0.0f,
                .z = target.z - position.z
    };
    Vector3 up = {0, 1, 0};
    Vector3 left = Algebra::normalize(Vector3CrossProduct(up, forward));
    Vector3 movement = Vector3Scale(left, s);
    position = Vector3Add(position, movement);
    eye_position = Vector3Add(eye_position, movement);
    target = Vector3Add(target, movement);
    return target;
}

Vector3 Player::move_right(float s) {
    Vector3 forward {.x = target.x - position.x,
                .y = 0.0f,
                .z = target.z - position.z
    };
    Vector3 up = {0, 1, 0};
    Vector3 right = Algebra::normalize(Vector3CrossProduct(forward, up));
    Vector3 movement = Vector3Scale(right, s);
    position = Vector3Add(position, movement);
    eye_position = Vector3Add(eye_position, movement);
    target = Vector3Add(target, movement);
    return target;
}

void Player::set_y(float y) {
    position.y = y;
    eye_position.y = y+0.3f;
    target.y = y;
}

Vector3 Player::collision_reaction(Vector3 push) {
    position = Vector3Add(position, push);
    eye_position = Vector3Add(eye_position, push);
    target = Vector3Add(target, push);
    return target;
}