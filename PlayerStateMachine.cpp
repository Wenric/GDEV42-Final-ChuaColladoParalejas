#include <raylib.h>
#include <C:\raylib\raylib\src\raymath.h>
#include "Player.hpp"
#include <iostream>


void Player::Jump() {
    if (IsKeyPressed(KEY_SPACE) && !isFalling) {
        velocity.y -= 23;
        fall_timer = 0;
    }
}


void Player::PhysicsUpdate(float TIMESTEP) {
    //gravity
    float down_force = (velocity.y * fall_timer) + (0.5 * 9.8 * pow(fall_timer, 2) * 7);

    if (isFalling && down_force < 5) {
        fall_timer += TIMESTEP;
    }
    position.y += down_force;
    position.x += velocity.x * TIMESTEP * direction;
    
    //grounding checker
    isFalling = abs(velocity.y) != 0;
}

void Player::Update(float delta_time) {
    //update cooldowns
    dash_cooldown -= delta_time;

    std::cout << isHit << std::endl;
   
    //iframe 
    if (isHit) {
        iframe_time -= delta_time;
        if(iframe_time <= 0) {
            isHit = false;
            iframe_time = 0.5f;
        }
    }
    current_state->Update(*this, delta_time);
}


void Player::Draw() {
    DrawCircle(position.x, position.y, radius, color);
}

void Player::SetState(PlayerState* new_state) {
    current_state = new_state;
    current_state->Enter(*this);
}

void Player::PassEntityInfo(Entity& enemy) {
    entities.push_back(&enemy);
}


void PlayerIdle::Enter(Player& player) {
    player.color = WHITE;
}

void PlayerMoving::Enter(Player& player) {
    player.color = BLUE;
}

void PlayerDashing::Enter(Player& player) {
    player.color = GREEN;
    player.dash_timer = 0.25f;
    player.dash_cooldown = 2.0f;
}


Player::Player(Vector2 pos, float rad, float spd) {
    position = pos;
    radius = rad;
    speed = spd;
    fall_timer = 0;
    health = 20;
    isFalling = true;
    color = WHITE;
    dash_cooldown = 0;
    iframe_time = 0.5f;
    isAlive = true;
    isHit = false;
    velocity = Vector2Zero();
    SetState(&idle);
}

void PlayerIdle::Update(Player& player, float delta_time) {
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        player.SetState(&player.moving);
    }
    if (IsKeyPressed(KEY_SPACE) && !player.isFalling) {
        player.Jump(); 
    }
}

void PlayerMoving::Update(Player& player, float delta_time) {
    player.velocity.x = 0;
    if (IsKeyDown(KEY_A)) {
        player.velocity.x = player.speed;
        player.direction = -1;
    } else if (IsKeyDown(KEY_D)) {
        player.velocity.x = player.speed;
        player.direction = 1;
    } else {
        player.SetState(&player.idle);
    }

    if (IsKeyPressed(KEY_SPACE) && !player.isFalling) {
        player.Jump();
    }

    if (IsKeyPressed(KEY_LEFT_SHIFT) && player.dash_cooldown <= 0) {
        player.SetState(&player.dashing);
    }

}

void PlayerDashing::Update(Player& player, float delta_time) {
    player.velocity.x = 1250;
    player.dash_timer -= delta_time;

    if (player.dash_timer <= 0) {
        player.SetState(&player.idle);
    }

}

