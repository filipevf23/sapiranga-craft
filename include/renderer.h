#pragma once

#include <cstdint>
#include "shapes.h"
#include "world.h"
#include <vector>
#include "inventory.h"

extern Color bg_color;
extern Color wall_color;
extern Color line_color;

class Renderer {
public:
    Camera3D camera = { 0 };
    int specific = -1;

    int render_distance {64};
    Renderer();
    void update_camera(Vector3, float);
    void rotate_camera(double);
    Vector3 get_target();
    Vector3 get_camera_position();
    double get_camera_position_x();
    double get_camera_position_y();
    double get_camera_position_z();
    void set_camera_position_y(double);
    void set_camera_position(Vector3);
    void set_target_position(Vector3);
    void specific_render(int);

    void show_crosshair();
    void show_inventory(Inventory &inventory);
    void render_chunks(std::vector<Chunk> &chunks);
};