#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "Enemy.hpp"



void Enemy::PhysicsUpdate(float TIMESTEP) {
    //gravity
    float down_force = (velocity.y * fall_timer) + (0.5 * 9.8 * pow(fall_timer, 2) * 7);
    

    if (isFalling && down_force < 5) {
        fall_timer += TIMESTEP;
    }

    position.y += down_force;
    position.x += velocity.x * TIMESTEP * direction;
    
    isFalling = abs(velocity.y) != 0;

}

void Enemy::Update(float delta_time) {
    current_state->Update(*this, delta_time);
}

void Enemy::Draw() {
    if (current_state==&attacking) {
        DrawCircle(position.x, position.y,Clamp(attack_hurtbox_rad, 0, attack_rad), WHITE);
    }
    DrawCircle(position.x, position.y, radius, color);
    DrawCircleLines(position.x,position.y,detect_rad, WHITE);
    DrawCircleLines(position.x, position.y, aggro_rad, WHITE);
    DrawCircleLines(position.x, position.y, attack_rad, WHITE);
    
}

void Enemy::SetState(EnemyState* new_state) {
    current_state = new_state;
    current_state->Enter(*this);
}

void Enemy::PassPlayerInfo(Entity& playerEntity) {
    player = &playerEntity;
}

void EnemyIdle::Enter(Enemy& enemy) {
    enemy.color = BROWN;
    enemy.velocity.y -= 2;
    enemy.fall_timer = 0;
    enemy.SetState(&enemy.wandering);
}

void EnemyWandering::Enter(Enemy& enemy) {
    enemy.color = PURPLE;
    enemy.velocity.x = enemy.direction * enemy.speed;
    enemy.wander_time = 2.50f;
}

void EnemyChasing::Enter(Enemy& enemy) {
    enemy.color = ORANGE;
}

void EnemyReadying::Enter(Enemy& enemy) {
    enemy.color = YELLOW;
    enemy.windup_time = 0.75f;
}

void EnemyAttacking::Enter(Enemy& enemy) {
    enemy.color = RED;
    enemy.attack_dur = 0.4f;
    enemy.attack_hurtbox_rad = 0;
}

Enemy::Enemy(Vector2 pos, float rad, float spd) {
    position = pos;
    radius = rad;
    speed = spd;
    fall_timer = 0;
    color = WHITE;
    isFalling = true;
    velocity = Vector2Zero();
    direction = rand() % 2 == 0 ? 1.0f : -1.0f;

    detect_rad = 600;
    aggro_rad = 404;
    attack_rad = 96;
    SetState(&idle);
}

void EnemyIdle::Update(Enemy& enemy, float delta_time) {}

void EnemyWandering::Update(Enemy& enemy, float delta_time) {
    
    enemy.wander_time -= delta_time;
    enemy.velocity.x = enemy.speed/3;
    if (enemy.wander_time <= 0.0f) {
        
        enemy.direction = rand() % 2 == 0 ? 1.0f : -1.0f;
        enemy.wander_time = GetRandomValue(2,3);
    }

    if (CheckCollisionCircles(enemy.position, enemy.aggro_rad, enemy.player->position, enemy.player->radius)) {
        enemy.SetState(&enemy.chasing);
    }
}

void EnemyChasing::Update(Enemy& enemy, float delta_time) {
    if (enemy.position.x < enemy.player->position.x) {
        enemy.direction = 1.0f; 
    } else if (enemy.position.x > enemy.player->position.x) {
        enemy.direction = -1.0f; 
    }
    
    enemy.velocity.x = enemy.speed;

    if (!CheckCollisionCircles(enemy.position, enemy.detect_rad, enemy.player->position, enemy.player->radius)) {
        enemy.SetState(&enemy.wandering);
    }

    if (CheckCollisionCircles(enemy.position, enemy.attack_rad, enemy.player->position, enemy.player->radius)) {
        enemy.SetState(&enemy.readying);
    }
}

void EnemyReadying::Update(Enemy& enemy, float delta_time) {
    enemy.velocity.x = 0;
    enemy.windup_time -= delta_time;
    if (enemy.windup_time <= 0) {
        enemy.SetState(&enemy.attacking);
    }
}

void EnemyAttacking::Update(Enemy& enemy, float delta_time) {
    enemy.velocity.x = 0;

    enemy.attack_dur -= delta_time;
    if (enemy.attack_dur > 0) {
        enemy.attack_hurtbox_rad += 6;
    } else {
        enemy.SetState(&enemy.wandering);
    }
    if(CheckCollisionCircles(enemy.position,enemy.attack_hurtbox_rad, enemy.player->position, enemy.player->radius)) {
       enemy.player->TakeDamage(5);
    }

}
 