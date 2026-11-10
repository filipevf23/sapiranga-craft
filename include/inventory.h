#pragma once

#include <cstdint>
#include "items.h"

#define STACK 64

typedef struct {
    ItemID item;
    uint8_t amount; 
} Item;

class Inventory {
public:
    Item inventory [4][9];

    Item helmet;
    Item chestplate;
    Item leggings;
    Item boots;
    int hotbar_index = 27;
    int capacity = 36;

    Inventory();
    Item* get_hotbar(); // get hotbar items
    bool add_to_inventory(ItemID);
    
};