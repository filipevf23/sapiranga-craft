#include "collision_manager.h"
#include "linear_algebra.h"
#include "shapes.h"
#include "raymath.h"
#include <algorithm>

bool CollisionManager::check_collision(Quad *a, Quad *b) {
    return CheckCollisionBoxes(get_bounding_box(a), get_bounding_box(b));
}

bool CollisionManager::check_collision(Sphere *a, Sphere *b) {
    Vector3 aux {
        .x = a->center.x - b->center.x,
        .y = a->center.y - b->center.y,
        .z = a->center.z - b->center.z
    };
    return Algebra::norm(aux) <= (a->radius + b->radius);
}

bool CollisionManager::check_collision(Quad *c, Sphere *s) {
    return CheckCollisionBoxSphere(get_bounding_box(c), s->center, s->radius);
}

bool CollisionManager::check_collision(Sphere *s, Quad *c) {
    return check_collision(c, s);
}

Vector3 CollisionManager::get_reaction_vector(const Sphere *s, Quad *c) {
    Ray ray = Ray{s->center, Algebra::normalize(Vector3Subtract(c->center, s->center))};
    RayCollision rc = GetRayCollisionBox(ray, get_bounding_box(c));
    return rc.normal;
}

Vector3 CollisionManager::get_reaction_vector(const Quad*s, Quad *c) {
    BoundingBox pBox = get_bounding_box(const_cast<Quad*>(s)); 
    BoundingBox bBox = get_bounding_box(c);

    float overlapX = std::min(pBox.max.x - bBox.min.x, bBox.max.x - pBox.min.x);
    float overlapY = std::min(pBox.max.y - bBox.min.y, bBox.max.y - pBox.min.y);
    float overlapZ = std::min(pBox.max.z - bBox.min.z, bBox.max.z - pBox.min.z);

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

RayCollision CollisionManager::get_ray_collision(const Vector3 *p_eyes, const Vector3 *p_target, const Vector3 *p_size, const Quad *object) {
    Vector3 face = Vector3Add(*p_eyes, {.x=p_size->x/2.0f, .y=0.0f, .z = p_size->z/2.0f});
    Ray ray = Ray{face, Algebra::normalize(Vector3Subtract(*p_target, face))};
    RayCollision rc = GetRayCollisionBox(ray, get_bounding_box(object));
    return rc;
}