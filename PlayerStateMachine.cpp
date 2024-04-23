#include <raylib.h>
#include <C:\raylib\raylib\src\raymath.h>
#include "Player.hpp"
#include <iostream>
float down_force = 0;

void Jump(Player& player) {
    if (IsKeyPressed(KEY_SPACE)) {
        player.velocity.y = 0;
        player.velocity.y -= 15;
        player.isFalling = true;
        player.fall_timer = 0;
    }
}


void Player::PhysicsUpdate(float TIMESTEP) {
    //gravity
    down_force = (velocity.y * fall_timer) + (0.5 * 9.8 * pow(fall_timer, 2) * 7);

    if (isFalling && down_force < 5) {
        fall_timer += TIMESTEP;
    }
    position.y += down_force;
    // std::cout << fall_timer << std::endl;
    position.x += velocity.x * TIMESTEP;
    std::cout << isFalling << std::endl;
}

void Player::Update(float delta_time) {
    current_state->Update(*this, delta_time);
}


void Player::Draw() {
    DrawCircle(position.x, position.y, radius, color);
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
    fall_timer = 0;
    isFalling = true;
    color = WHITE;
    velocity = Vector2Zero();
    SetState(&idle);
}

void PlayerIdle::Update(Player& player, float delta_time) {
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        player.SetState(&player.moving);
    }
    Jump(player);
}

void PlayerMoving::Update(Player& player, float delta_time) {
    player.velocity.x = 0;
    if (IsKeyDown(KEY_A)) {
        player.velocity.x = -player.speed;
    } else if (IsKeyDown(KEY_D)) {
        player.velocity.x = player.speed;
    }

    Jump(player);

}

