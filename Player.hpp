#ifndef PLAYER
#define PLAYER

#include <raylib.h>
#include <raymath.h>
#include "Entity.hpp"
#include <vector>

class Player;

class PlayerState {
public:
    virtual ~PlayerState() {}
    virtual void Enter(Player& player) = 0;
    virtual void Update(Player& player, float delta_time) = 0;
    // virtual void PhysicsUpdate(Player& player, float TIMESTEP) = 0;
};

class PlayerIdle : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    // void PhysicsUpdate(Player& player, float TIMESTEP);
};

class PlayerMoving : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    // void PhysicsUpdate(Player& player, float TIMESTEP);
};

class PlayerDashing : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
};

class Player : public Entity {
public:
    Color color;
    bool isFalling;
    float fall_timer;
    float dash_timer;
    float dash_cooldown;
    float direction;
    PlayerIdle idle;
    PlayerMoving moving;
    PlayerDashing dashing;
    Camera2D* camera;
    std::vector<Entity*> entities; 

    bool active = false;
    int frames_counter = 0;

    void CheckTileCollision(const Rectangle tile) {
        Vector2 closest_point;
        closest_point.x = Clamp(position.x, tile.x, tile.x + tile.width);
        closest_point.y = Clamp(position.y, tile.y, tile.y + tile.height);
        float distance = sqrt((position.x - closest_point.x) * (position.x - closest_point.x) + (position.y - closest_point.y) * (position.y - closest_point.y));

        if (distance < radius) {
            velocity.y = 0;
            fall_timer = 0;

            closest_point.x = Clamp(position.x, tile.x, tile.x + tile.width);
            closest_point.y = Clamp(position.y, tile.y, tile.y + tile.height);
            float distance = sqrt((position.x - closest_point.x) * (position.x - closest_point.x) + (position.y - closest_point.y) * (position.y - closest_point.y));
            Vector2 col_norm;
            col_norm = Vector2Normalize(Vector2Subtract(position, closest_point));
            float overlap = radius - distance;
            Vector2 movement = Vector2Scale(col_norm, overlap);
            position = Vector2Add(position, movement);            

        }
          
    }

    void CheckProjectileCollision(Projectile& projectile) {
        for (auto& enemy : entities ) {
            if(CheckCollisionCircles(enemy->position, enemy->radius, projectile.position, projectile.radius)) {
                enemy->health -= projectile.damage;
                projectile.exists = false;
            }
        }
    }

    Player(Vector2 pos, float rad, float spd, Texture2D* tex);
    void Update(float delta_time);
    void PhysicsUpdate(float TIMESTEP);
    void PassCameraInfo(Camera2D& cam);
    void PassEntityInfo(Entity& enemy);
    void Draw();
    void Jump();
    void SetState(PlayerState* new_state);
    void BoomerShoot(float delta_time);

private:
    PlayerState* current_state;
};

#endif