#include "collision_manager.h"
#include "linear_algebra.h"
#include "shapes.h"
#include "raymath.h"
#include <algorithm>

bool CollisionManager::check_collision(Block *a, Block *b) {
    return CheckCollisionBoxes(a->bb, b->bb);
}

bool CollisionManager::check_collision(Sphere *a, Sphere *b) {
    Vector3 aux {
        .x = a->center.x - b->center.x,
        .y = a->center.y - b->center.y,
        .z = a->center.z - b->center.z
    };
    return Algebra::norm(aux) <= (a->radius + b->radius);
}

bool CollisionManager::check_collision(Block *c, Sphere *s) {
    return CheckCollisionBoxSphere(c->bb, s->center, s->radius);
}

bool CollisionManager::check_collision(Sphere *s, Block *c) {
    return check_collision(c, s);
}

Vector3 CollisionManager::get_reaction_vector(const Sphere *s, Block *c) {
    Ray ray = Ray{s->center, Algebra::normalize(Vector3Subtract(c->center, s->center))};
    RayCollision rc = GetRayCollisionBox(ray, c->bb);
    return rc.normal;
}

Vector3 CollisionManager::get_reaction_vector(const Block*s, Block *c) {

    float overlapX = std::min(s->bb.max.x - c->bb.min.x, c->bb.max.x - s->bb.min.x);
    float overlapY = std::min(s->bb.max.y - c->bb.min.y, c->bb.max.y - s->bb.min.y);
    float overlapZ = std::min(s->bb.max.z - c->bb.min.z, c->bb.max.z - s->bb.min.z);

    Vector3 push {0,0,0};

    if (overlapX < overlapY && overlapX < overlapZ) {
        push.x = (s->center.x < c->center.x) ? -overlapX : overlapX;
    } else if (overlapY < overlapX && overlapY < overlapZ) {
        push.y = (s->center.y < c->center.y) ? -overlapY : overlapY;
    } else {
        push.z = (s->center.z < c->center.z) ? -overlapZ : overlapZ;
    }
    return push;
}

BoundingBox CollisionManager::get_bounding_box(const Quad *c) {
    return get_bounding_box(&c->center, &c->size);
}

BoundingBox CollisionManager::get_bounding_box(const Vector3 *center, const Vector3 *size) {
    Vector3 min {
        .x = center->x - size->x/2,
        .y = center->y - size->y/2,
        .z = center->z - size->z/2,
    }, 
    max {
        .x = center->x + size->x/2,
        .y = center->y + size->y/2,
        .z = center->z + size->z/2,
    };
    
    return BoundingBox{min, max};
}

bool CollisionManager::point_inside_AABB(Vector3 point, BoundingBox box) {
    return (
        point.x >= box.min.x &&
        point.x <= box.max.x &&
        point.y >= box.min.y &&
        point.y <= box.max.y &&
        point.z >= box.min.z &&
        point.z <= box.max.z 
    );
}

RayCollision CollisionManager::get_ray_collision(const Vector3 *p_eyes, const Vector3 *p_target, const BoundingBox *object) {
    Ray ray = Ray{*p_eyes, Algebra::normalize(Vector3Subtract(*p_target, *p_eyes))};
    RayCollision rc = GetRayCollisionBox(ray, *object);
    return rc;
}