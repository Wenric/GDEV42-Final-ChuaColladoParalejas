#ifndef ENTITY
#define ENTITY

#include <raylib.h>
#include <raymath.h>

class Tile {
public:
    Rectangle loc_sprite_sheet;
    bool has_collision;
};

class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    float health;
    float radius;
    float speed;
    bool isAlive;
    bool isHit;
    float iframe_time;

    void CheckTileCollision(const Rectangle tile) {
        Vector2 closest_point;
        closest_point.x = Clamp(position.x, tile.x, tile.x + tile.width);
        closest_point.y = Clamp(position.y, tile.y, tile.y + tile.height);
        float distance = sqrt((position.x - closest_point.x) * (position.x - closest_point.x) + (position.y - closest_point.y) * (position.y - closest_point.y));

        if (distance < radius) {
            Vector2 col_norm;
            col_norm = Vector2Normalize(Vector2Subtract(position, closest_point));
            float overlap = radius - distance;
            Vector2 movement = Vector2Scale(col_norm, overlap);
            position = Vector2Add(position, movement);
        }
    }

    void TakeDamage(float dmg) {
        if (!isHit) {
            health -= dmg;
        }
        isHit = true;
        if (health <= 0) {
            isAlive = false;
        }
    }

};

#endif 