// Engine Class with all the functionalities of an Engine
//         *no game information

#pragma once

#include <cstdint>
#include "renderer.h"
#include "asset_manager.h"
#include "world.h"
#include "player.h"


using Entity = std::uint32_t;
using ComponentType = std::uint8_t;

const Entity MAX_ENTITIES = 5000;
const ComponentType MAX_COMPONENTS = 32;

class Engine {
private:
    Renderer renderer;
    AssetManager asset_manager;
    World world {true, 0.981};
    Player player;
    float mouse_sensitivity {0.05};
    bool break_block_command = false;

    // Player logic that will later be separated
    

public:
    void init();
    void update();
    void check_events(); 
    void run();

    bool is_face_visible(const Vector3 *face_center, const Vector3 *object_center, const Vector3 *camera_pos);
    float get_player_floor();
    void frustum_culling(Block *object, const Vector3 *camera_pos);
};