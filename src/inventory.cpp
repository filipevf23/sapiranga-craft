#include "inventory.h"

Item* Inventory::get_hotbar() {
    return inventory + hotbar_index;
}

bool Inventory::add_to_inventory(ItemID new_item) {
    // Check hotbar for similar item
    for (int i=hotbar_index; i<capacity; ++i) {
        if (inventory[i].item == new_item) {
            if (inventory[i].amount == STACK || !ItemInformation::is_item_stackable(inventory[i].item)) {
                continue;
            }
            inventory[i].amount++;
            return true;
        }
    }
    // Check rest of inventory for similar item
    for (int i=0; i<hotbar_index; ++i) {
        if (inventory[i].item == new_item) {
            if (inventory[i].amount == STACK || !ItemInformation::is_item_stackable(inventory[i].item)) {
                continue;
            }
            inventory[i].amount++;
            return true;
        }
    }
    // Check hotbar for empty space
    for (int i=hotbar_index; i<capacity; ++i) {
        if (inventory[i].item == ItemID::EMPTY) {
            inventory[i].item = new_item;
            inventory[i].amount = 1;
            return true;
        }
    }
    for (int i=0; i<hotbar_index; ++i) {
        if (inventory[i].item == ItemID::EMPTY) {
            inventory[i].item = new_item;
            inventory[i].amount = 1;
            return true;
        }
    }
    return false;
}