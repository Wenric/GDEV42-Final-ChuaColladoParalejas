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
    virtual void PhysicsUpdate(Player& player, float TIMESTEP) = 0;
};

class PlayerIdle : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void PhysicsUpdate(Player& player, float TIMESTEP);
};

class PlayerMoving : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void PhysicsUpdate(Player& player, float TIMESTEP);
};


class Player : public Entity {
public:
    Color color;

    PlayerIdle idle;
    PlayerMoving moving;
    std::vector<Entity> entities;   

    Player(Vector2 pos, float rad, float spd);
    void Update(float delta_time);
    void PhysicsUpdate(float TIMESTEP);
    void Draw();
    void SetState(PlayerState* new_state);

private:
    PlayerState* current_state;
};

#endif