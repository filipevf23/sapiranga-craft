#include "renderer.h"
#include "raylib.h"
#include "linear_algebra.h"
#include "raymath.h"
#include <iostream>

Color bg_color = {.r=121, .g=166, .b=255, .a=255};
Color wall_color = {.r=255, .g=255, .b=255, .a=255};
Color line_color = {.r=255, .g=0, .b=0, .a=255};

Renderer::Renderer() {
    Vector3 center = {0,1.7,10};
    camera.position = (Vector3){ 2.5, 1.7, 4.6 };
    camera.target = center;
    camera.up = (Vector3){ 0.0, 1.0, 0.0 };
    camera.fovy = 70.0;
    camera.projection = CAMERA_PERSPECTIVE;
    DisableCursor();
}

void Renderer::update_camera(Vector3 player_position, float mouse_sensitivity) {
    Vector3 rotation = {
        GetMouseDelta().x * mouse_sensitivity, 
        GetMouseDelta().y * mouse_sensitivity, 
        0.0f                             
    };
    Vector3 movement = {0,0,0};
    UpdateCameraPro(&camera, movement, rotation, 0.0f);
}

void Renderer::rotate_camera(double angle) {
    Algebra::rotate_yy(angle, &camera.target, camera.position);
}

Vector3 Renderer::get_target() {
    return camera.target;
}
Vector3 Renderer::get_camera_position() {
    return camera.position;
}
double Renderer::get_camera_position_x() {
    return camera.position.x;
}
double Renderer::get_camera_position_y() {
    return camera.position.y;
}
double Renderer::get_camera_position_z() {
    return camera.position.z;
}
void Renderer::set_camera_position_y(double pos) {
    camera.position.y = pos;
}
void Renderer::set_camera_position(Vector3 new_pos) {
    camera.position = new_pos;
}
void Renderer::set_target_position(Vector3 new_tgt) {
    camera.target = new_tgt;
}

void Renderer::specific_render(int i) {
    specific = i;
}

void Renderer::render(std::vector<Quad> *objects, Model model) {

    /*
    *   Create entity-model relationship
    *   for every &world entity.
    *   This function must do it all, no logic in engine.cpp!
    *   
    *   TODO: Substitute Quads for simple Bounding Boxes!
    *         Check All Calculations With Bounding Boxes!
    *         Create 1 Mesh / Chunk!
    *         
    */
    BeginDrawing();
        BeginMode3D(camera);
            ClearBackground(bg_color);
            for (int i=0; i<(int)objects->size(); ++i) {
                Quad *q = &(*objects)[i];
                if (!q->show) {
                    continue;
                }
                if (specific != -1 && i == specific) {
                    DrawModel(model, q->center, 1.0, RED);
                    continue;
                }
                DrawModel(model, q->center, 1.0, WHITE);
            }
		EndMode3D();
    EndDrawing();
}