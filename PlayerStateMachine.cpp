#include <raylib.h>
#include <C:\raylib\raylib\src\raymath.h>
#include "Player.hpp"
#include <iostream>

void Player::PhysicsUpdate(float TIMESTEP) {
    //gravity
    velocity.y = 100 * TIMESTEP;
}
void Player::Update(float delta_time) {
    current_state->Update(*this, delta_time);
}


void Player::Draw() {
}

void Player::SetState(PlayerState* new_state) {
    current_state = new_state;
    current_state->Enter(*this);
}


void PlayerIdle::Enter(Player& player) {
    player.color = WHITE;
}

void PlayerMoving::Enter(Player& player) {
    player.color = BLUE;
}


Player::Player(Vector2 pos, float rad, float spd) {
    position = pos;
    radius = rad;
    speed = spd;
    color = WHITE;
}

void PlayerIdle::Update(Player& player, float delta_time) {
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        player.SetState(&player.moving);
    }
}

void PlayerMoving::Update(Player& player, float delta_time) {
    
}

void PlayerMoving::Update(Player& player, float delta_time) {
    player.velocity.x = 0;
    if (IsKeyDown(KEY_A)) {
        player.velocity.x = -player.speed;
    } else if (IsKeyDown(KEY_D)) {
        player.velocity.x = player.speed;
    }

    player.position.x += player.velocity.x * delta_time;


}

