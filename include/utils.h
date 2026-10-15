#pragma once

#include "raylib.h"
#include <string>
#include <format>


inline std::string format(const Vector3 &v) {
    return std::format("[{}, {}, {}]", v.x, v.y, v.z);
}