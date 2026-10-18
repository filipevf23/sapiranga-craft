#pragma once

#include "shapes.h"

namespace CollisionManager {
    
    bool check_collision(Block*, Block*);
    bool check_collision(Sphere*, Sphere*);
    bool check_collision(Block*, Sphere*);
    bool check_collision(Sphere*, Block*);
    Vector3 get_reaction_vector(const Sphere*, Block*);
    Vector3 get_reaction_vector(const Block*, Block*);
    BoundingBox get_bounding_box(const Quad*);
    BoundingBox get_bounding_box(const Vector3*, const Vector3*);
    bool point_inside_AABB(Vector3, BoundingBox);
    RayCollision get_ray_collision(const Vector3 *p_eyes, const Vector3 *p_target, const BoundingBox *object);
};