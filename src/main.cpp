#include <iostream>
#include "engine.h"

using namespace std;

int main() {
    int width=1920, height=1080;
    SetConfigFlags(FLAG_FULLSCREEN_MODE); 
    const char *title = "Minecraft";	
    InitWindow(width, height, title);
    SetTargetFPS(200);
    Engine engine;
    engine.run();
    return 0;
}