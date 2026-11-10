#include "items.h"

bool ItemInformation::is_item_stackable(ItemID id) {
    if (id == ItemID::I_COBBLESTONE || id == ItemID::I_STONE)
        return true;
    return false;
}

TextureID ItemInformation::get_item_texture(ItemID id) {
    switch(id) {
    case ItemID::I_DIAMONDHOE: return DIAMONDHOE;
    default: return NOTHING;
    }
}