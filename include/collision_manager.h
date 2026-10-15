#pragma once

#include "math_types.h"

namespace CollisionManager {
    
    bool check_collision(Quad*, Quad*);
    bool check_collision(Sphere*, Sphere*);
    bool check_collision(Quad*, Sphere*);
    bool check_collision(Sphere*, Quad*);
    Vector3 get_reaction_vector(const Sphere*, Quad*);
    Vector3 get_reaction_vector(const Quad*, Quad*);
    BoundingBox get_bounding_box(const Quad*);
    BoundingBox get_bounding_box(const Vector3*, const Vector3*);
    bool point_inside_AABB(Vector3, BoundingBox);
    RayCollision get_ray_collision(const Vector3 *p_eyes, const Vector3 *p_target, const Vector3 *p_size, const Quad *object);
};