#include "asset_manager.h"
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include "shapes.h"
#include "raymath.h"

using std::cout, std::endl, std::vector, std::string;

AssetManager::AssetManager() : texture_size {1} {
    // Creating a Cube Model by 'glueing' a texture to a mesh.
    //   *Must do this for every shape and different texture
    Mesh cube_mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model cube_model = LoadModelFromMesh(cube_mesh);
    add_texture(LoadTexture("assets/brick-wall.png"));
    minecraft_texture = LoadTextureFromImage(LoadImage("assets/minecraft-textures.png"));
    SetTextureFilter(minecraft_texture, TEXTURE_FILTER_POINT);
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

void AssetManager::unload_mesh() {
    UnloadMesh(mesh);
}

bool AssetManager::load_minecraft_textures(int id) {
    return true;
}

void AssetManager::load_texture_ids(int a, int b, int c, int d, int e, int f, int *dest) {
    dest[0] = a;
    dest[1] = b;
    dest[2] = c;
    dest[3] = d;
    dest[4] = e;
    dest[5] = f;
}

void AssetManager::get_block_textures(int id, int *dest) {
    switch (id) {
        case B_PODZOL:
            load_texture_ids(PODZOL2, PODZOL2, PODZOL1, DIRT, PODZOL2, PODZOL2, dest);
            break;

        case B_DROPPER:
            load_texture_ids(DROPPER1, DROPPER1, DROPPER2, DROPPER1, DROPPER1, DROPPER1, dest);
            break;

        default:
            load_texture_ids(0,0,0,0,0,0,dest);
            break;
    }
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
    cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = minecraft_texture;
    models.emplace_back(cube_model);
    return true;
}

Model AssetManager::create_complex_model(std::vector<Vector3> &vertices, std::vector<int> &t_ids) {
    static float block_size = 16.0f;
    static float img_size = 512.0f;
    float uv = block_size / img_size;
    mesh = {0};
    mesh.vertexCount = vertices.size();
    mesh.triangleCount = vertices.size() / 2;
    mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.indices = (unsigned short*)MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

    // Vertex Coords
    size_t i=0;
    for (size_t v=0; v < vertices.size(); ++v) {
        mesh.vertices[i++] = vertices[v].x;
        mesh.vertices[i++] = vertices[v].y;
        mesh.vertices[i++] = vertices[v].z;
    }

    // Indices
    i=0;
    for (size_t v=0; v < vertices.size()/4; ++v) {
        mesh.indices[i++] = v*4;
        mesh.indices[i++] = v*4 + 1;
        mesh.indices[i++] = v*4 + 2;
        mesh.indices[i++] = v*4;
        mesh.indices[i++] = v*4 + 2;
        mesh.indices[i++] = v*4 + 3;
    }

    // normals
    i = 0;
    for (size_t v = 0; v < vertices.size(); v += 4) {
        Vector3 v1 = Vector3Subtract(vertices[v+1], vertices[v]);
        Vector3 v2 = Vector3Subtract(vertices[v+2], vertices[v]);
        Vector3 normal = Vector3Normalize(Vector3CrossProduct(v2, v1)); 

        for (int j = 0; j < 4; ++j) {
            mesh.normals[i++] = normal.x;
            mesh.normals[i++] = normal.y;
            mesh.normals[i++] = normal.z;
        }
    }

    // Texture Coords
    i = 0;
    for (size_t face=0; face < t_ids.size(); ++face) {
        Vector2 d = pixel_to_float(t_ids[face]);
        Vector2 tl = {d.x   , d.y};     // top-left
        Vector2 bl = {d.x   , d.y+uv};  // bottom-left
        Vector2 br = {d.x+uv, d.y+uv};  // bottom-right
        Vector2 tr = {d.x+uv, d.y};     // top-right
            
        mesh.texcoords[i++] = tl.x ; mesh.texcoords[i++] = tl.y;
        mesh.texcoords[i++] = bl.x ; mesh.texcoords[i++] = bl.y;
        mesh.texcoords[i++] = br.x ; mesh.texcoords[i++] = br.y;
        mesh.texcoords[i++] = tr.x ; mesh.texcoords[i++] = tr.y;
        
    }
    UploadMesh(&mesh, false);
    Model model = LoadModelFromMesh(mesh);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = minecraft_texture;
    return model;
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