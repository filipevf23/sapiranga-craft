#include "asset_manager.h"
#include <iostream>
#include <stdexcept>

using std::cout, std::endl, std::vector, std::string;

AssetManager::AssetManager() : texture_size {1} {
    // Creating a Cube Model by 'glueing' a texture to a mesh.
    //   *Must do this for every shape and different texture
    Mesh cube_mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model cube_model = LoadModelFromMesh(cube_mesh);
    add_texture(LoadTexture("assets/brick-wall.png"));
    cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textures[0];
    models.emplace_back(cube_model);

    // Trying new downloaded assets
    const char *file_name = "assets/kit1/Assets/obj/mug_full.obj";
    Model test_model = LoadModel(file_name);
    models.emplace_back(test_model);

    int dropper[] = {DROPPER1, DROPPER1, DROPPER2, DROPPER1, DROPPER1, DROPPER1};
    add_block_by_specific_texture(dropper);
    int podzol[] = {PODZOL2, PODZOL2, PODZOL1, DIRT, PODZOL2, PODZOL2};
    add_block_by_specific_texture(podzol);
}

AssetManager::~AssetManager() {
    for (Texture2D& tex : textures) {
        UnloadTexture(tex);
    }
    for (Model &m : models) {
        UnloadModel(m);
    }
}

bool AssetManager::add_texture(Texture2D t) {
    try {
        textures.push_back(t);
    } catch (const std::runtime_error& e) {
        cout << "Error 'add_texture' : " << e.what() << endl;
        return false;
    }
    return true;
}

bool AssetManager::add_model(Model m) {
    try {
        models.push_back(m);
    } catch (const std::runtime_error& e) {
        cout << "Error 'add_model' : " << e.what() << endl;
        return false;
    }
    return true;
}

bool AssetManager::load_minecraft_textures(int id) {
    return true;
}

bool AssetManager::add_block_by_repeating_texture(int id) {
    static float block_size = 16.0f;
    static int png_column_size = 32;
    Mesh cube_mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Rectangle cropped_png = {  (id % png_column_size)*block_size, (id / png_column_size) * block_size, block_size, block_size };
    Image img = LoadImage("assets/minecraft-textures.png");
    Texture2D texture = LoadTextureFromImage(ImageFromImage(img, cropped_png));
    textures.push_back(texture);
    UnloadImage(img);
    Model cube_model = LoadModelFromMesh(cube_mesh);
    cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    models.emplace_back(cube_model);
    return true;
}

bool AssetManager::add_block_by_specific_texture(int ids[]) {
    static float block_size = 16.0f;
    static float img_size = 512.0f;
    Mesh cube_mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Image img = LoadImage("assets/minecraft-textures.png");
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);
    textures.push_back(texture);

    // Loop through 6 faces of block
    float uv = block_size / img_size;
    int i = 0;
    for (int j=0; j<6; ++j) {
        Vector2 d = pixel_to_float(ids[j]);
        Vector2 bl = {d.x   , d.y+uv};  // bottom-left
        Vector2 br = {d.x+uv, d.y+uv};  // bottom-right
        Vector2 tr = {d.x+uv, d.y};     // top-right
        Vector2 tl = {d.x   , d.y};     // top-left
        

        if (j == 1 || j == 4) {
            // -90 degree rotation
            cube_mesh.texcoords[i++] = br.x; cube_mesh.texcoords[i++] = br.y;
            cube_mesh.texcoords[i++] = tr.x; cube_mesh.texcoords[i++] = tr.y;
            cube_mesh.texcoords[i++] = tl.x; cube_mesh.texcoords[i++] = tl.y;
            cube_mesh.texcoords[i++] = bl.x; cube_mesh.texcoords[i++] = bl.y;
        } else {
            cube_mesh.texcoords[i++] = bl.x ; cube_mesh.texcoords[i++] = bl.y;
            cube_mesh.texcoords[i++] = br.x ; cube_mesh.texcoords[i++] = br.y;
            cube_mesh.texcoords[i++] = tr.x ; cube_mesh.texcoords[i++] = tr.y;
            cube_mesh.texcoords[i++] = tl.x ; cube_mesh.texcoords[i++] = tl.y;
        }
        
    }
    UpdateMeshBuffer(cube_mesh, 1, cube_mesh.texcoords, cube_mesh.vertexCount * 2 * sizeof(float), 0);

    Model cube_model = LoadModelFromMesh(cube_mesh);
    cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    models.emplace_back(cube_model);
    return true;
}

Vector2 AssetManager::pixel_to_float(int id, int width, int height, int asset_width, int asset_height) {
    Vector2 address;
    int cols = width / asset_width;

    float pixel_x = (id % cols) * asset_width;
    float pixel_y = (id / cols) * asset_height;

    address.x = pixel_x / (float)width;
    address.y = pixel_y / (float)height;

    return address;
}