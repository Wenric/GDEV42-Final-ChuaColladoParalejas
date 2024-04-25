#ifndef ENEMY
#define ENEMY

#include <raylib.h>
#include "Entity.hpp"

class Enemy;

class EnemyState {
public:
    virtual ~EnemyState() {}
    virtual void Enter(Enemy& enemy) = 0;
    virtual void Update(Enemy& enemy, float delta_time) = 0;
};

class EnemyIdle : public EnemyState {
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
};

class EnemyWandering : public EnemyState {
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
};

class EnemyChasing : public EnemyState {
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
};

class EnemyReadying : public EnemyState {
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
};

class EnemyAttacking : public EnemyState {
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
};

class Enemy : public Entity {
public:
    Color color;
    int direction;
    float detect_rad;
    float aggro_rad;
    float attack_rad;
    float attack_dur;
    float gravity;
    float windup_time;
    float attack_hurtbox_rad;
    Sound* attack_sound;

    bool isFalling;
    float fall_timer;

    float wander_time;
    Entity* player;

    EnemyIdle idle;
    EnemyWandering wandering;
    EnemyChasing chasing;
    EnemyReadying readying;
    EnemyAttacking attacking;

    Enemy(Vector2 pos, float rad, float spd, Texture2D* tex, bool exists);
    void PhysicsUpdate(float TIMESTEP);
    void Update(float delta_time);
    void Draw();
    void SetState(EnemyState* new_state);
    void PassPlayerInfo(Entity& player);
    void PassSoundInfo(Sound& hurt, Sound& attack);

private:
    EnemyState* current_state;
};

#endif