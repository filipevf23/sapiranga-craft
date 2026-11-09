#pragma once

#include "raylib.h"
#include <vector>
#include "shapes.h"
#include "items.h"


class AssetManager {
public:
    // Later change vector for better Data Structure!
    std::vector<Texture2D> textures;
    std::vector<Model> models;
    Texture2D minecraft_texture;
    size_t texture_size;
    Mesh mesh;
    AssetManager();
    ~AssetManager();
    bool add_texture(Texture2D);
    bool add_model(Model);
    void unload_mesh();
    void unload_models();
    bool load_minecraft_textures(int);
    void load_texture_ids(int front, int back, int up, int down, int right, int left, int *dest);
    void get_block_textures(ItemID id, int *destination);
    Color get_face_color(int id);
    bool add_block_by_repeating_texture(int);
    bool add_block_by_specific_texture(int []); // six faces of cube;
    Model create_complex_model(std::vector<Vector3> &vertices, std::vector<int> &t_ids);
    Vector2 pixel_to_float(int id, int width=512, int height=512, int asset_width=16, int asset_height=16);
};