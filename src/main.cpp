#include <iostream>
#include "engine.h"

using namespace std;

int main() {
    int width=1280, height=720;
    const char *title = "Organic";	
    InitWindow(width, height, title);
    SetTargetFPS(60);
    Engine engine;
    engine.run();
    return 0;
}