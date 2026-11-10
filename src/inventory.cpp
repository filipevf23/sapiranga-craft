#include "inventory.h"

Inventory::Inventory() {
    for (int i=0; i<4; ++i) {
        for (int j=0; j<9; j++)
        inventory[i][j] = {ItemID::EMPTY, 1};
    }
}

Item* Inventory::get_hotbar() {
    return inventory[3];
}

bool Inventory::add_to_inventory(ItemID new_item) {
    // Check hotbar for similar item
    for (int i=0; i<9; ++i) {
        if (inventory[3][i].item == new_item) {
            if (inventory[3][i].amount == STACK || !ItemInformation::is_item_stackable(inventory[3][i].item)) {
                continue;
            }
            inventory[3][i].amount++;
            return true;
        }
    }
    // Check rest of inventory for similar item
    for (int j=0; j<3; j++) {
        for (int i=0; i<9; ++i) {
            if (inventory[j][i].item == new_item) {
                if (inventory[j][i].amount == STACK || !ItemInformation::is_item_stackable(inventory[j][i].item)) {
                    continue;
                }
                inventory[j][i].amount++;
                return true;
            }
        }
    }
    
    // Check hotbar for empty space
    for (int i=hotbar_index; i<capacity; ++i) {
        if (inventory[3][i].item == ItemID::EMPTY) {
            inventory[3][i].item = new_item;
            inventory[3][i].amount = 1;
            return true;
        }
    }
    // Check rest of inventory space
    for (int j=0; j<3; j++) {
        for (int i=0; i<9; ++i) {
            if (inventory[j][i].item == ItemID::EMPTY) {
                inventory[j][i].item = new_item;
                inventory[j][i].amount = 1;
                return true;
            }
        }
    }
    return false;
}