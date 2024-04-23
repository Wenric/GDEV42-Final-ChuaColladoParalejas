#include <raylib.h>
#include <C:\raylib\raylib\src\raymath.h>
#include "Entity.hpp"
#include "Player.hpp"
#include <iostream>
float down_force = 0;

int magazine = 20; //amount of bullets
int boomer_pack = 1;
Bullet* ammo[20];
Boomerang* throws[1];


Bezier boomerang{2, 3, 5, 100, 10, BLUE}; //instantiate Bezier Curve

void InitBulletArray() {
    for (int i = 0; i < magazine; ++i) {
        ammo[i] = new Bullet(RED, 5.0f, {0, 0}, {0, 0}, 500, true, true);
    }
}

void InitBoomerangArray() {
    for (int i = 0; i < boomer_pack; ++i) {
        throws[i] = new Boomerang(GREEN, 20.0f, {0, 0}, {0, 0}, 700, true, true);
    }
}

// Vector2 bullet_pos = {20, -80};
// float bullet_rad = 32.0f;
// Color bullet_col = RED;

// // Bullet bullet(bullet_col, bullet_rad, bullet_pos);

void Jump(Player& player) {

    player.velocity.y = 0;
    player.velocity.y -= 15;
    player.isFalling = true;
    player.fall_timer = 0.01;
    
}


void Player::PhysicsUpdate(float TIMESTEP) {
    //gravity
    down_force = (velocity.y * fall_timer) + (0.5 * 9.8 * pow(fall_timer, 2) * 7);

    if (isFalling && down_force < 5) {
        fall_timer += TIMESTEP;
    }
    position.y += down_force;

    position.x += velocity.x * TIMESTEP;

}

void Player::Update(float delta_time, Camera2D camera) {
    current_state->Update(*this, delta_time, camera);
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

Vector2 direction; 
Vector2 dest;
void PlayerIdle::Update(Player& player, float delta_time, Camera2D camera) {
    
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        player.SetState(&player.moving);
    } 
    if (IsKeyPressed(KEY_SPACE)) {
        Jump(player);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        direction = Vector2Zero();
        direction = Vector2Normalize(Vector2Subtract(GetMousePosition(), GetWorldToScreen2D(player.position, camera)));
        
        for (int i = 0; i < magazine; ++i) {
            if (ammo[i]->exists) {
                // Set bullet properties
                ammo[i]->position = player.position;
                ammo[i]->exists = false;
                direction = Vector2Scale(direction, ammo[i]->speed);
                ammo[i]->direction = direction;
                // bullet = ammo[i];
                break; // Exit the loop after activating one bullet
            }
        }
    }

    for (int i = 0; i < magazine; ++i) {
            if (!ammo[i]->exists) {
                ammo[i]->position = Vector2Add(ammo[i]->position, Vector2Scale(ammo[i]->direction, delta_time));
            }
        }
    
    if(IsKeyPressed(KEY_LEFT_SHIFT)) {
        if (!boomerang.direction) {
            boomerang.Controller[0].Coords = Vector2Add(player.position, {40, 20});
            boomerang.Controller[1].Coords = Vector2Add(player.position, {1000, 0});
            boomerang.Controller[2].Coords = player.position;

        }
        else {
            boomerang.Controller[0].Coords = Vector2Add(player.position, {-40, 20});
            boomerang.Controller[1].Coords = Vector2Add(player.position, {-1000, 0});
            boomerang.Controller[2].Coords = player.position;
        }
        for (int i = 0; i < boomer_pack; ++i) {
            if (throws[i]->exists) {
                // Set bullet properties
                throws[i]->position = boomerang.Controller[0].Coords;
                throws[i]->exists = false;
                break; // Exit the loop after activating one bullet
            }
        }
    }

    for (int i = 0; i < boomer_pack; ++i) {
            if (!throws[i]->exists) {
                for (float e = 0; e < boomerang.step_input + 1; e++) {
                    if(CheckCollisionPointCircle(boomerang.Step_coords[e], throws[i]->position, throws[i]->collision_rad)) {
                        dest = boomerang.Step_coords[e + 1];
                        dest = Vector2Scale(Vector2Normalize(Vector2Subtract(dest, throws[i]->position)), throws[i]->speed * delta_time);    
                        }
                }
                std::cout << dest.x << std::endl;
                std::cout << dest.y << std::endl;
                throws[i]->position = Vector2Add(throws[i]->position, dest);

                if (CheckCollisionCircles(player.position, player.radius, throws[i]->position, throws[i]->collision_rad)) {
                    throws[i]->exists = true;
                }        
            }
        }
        

    if(boomerang.Controller.size() == boomerang.control_input) {
        boomerang.Update();
        // for (int i = 0; i < boomerang.Controller.size(); i++) {
        //     boomerang.Controller[i].Coords = Vector2Add(boomerang.Controller[i].Coords, player.position);
        // }
    }
}

void PlayerMoving::Update(Player& player, float delta_time, Camera2D camera) {
    player.velocity.x = 0;
    if (IsKeyDown(KEY_A)) {
        player.velocity.x = -player.speed;
        boomerang.direction = true;
    } else if (IsKeyDown(KEY_D)) {
        player.velocity.x = player.speed;
        boomerang.direction = false;
    }
    else if(player.velocity.x == 0 && player.velocity.y == 0) {
        player.SetState(&player.idle);
    }

    if (IsKeyPressed(KEY_SPACE)) {
        Jump(player);
    }

    if(IsKeyPressed(KEY_LEFT_SHIFT)) {
        if (!boomerang.direction) {
            boomerang.Controller[0].Coords = Vector2Add(player.position, {40, 20});
            boomerang.Controller[1].Coords = Vector2Add(player.position, {1000, 0});
            boomerang.Controller[2].Coords = player.position;

        }
        else {
            boomerang.Controller[0].Coords = Vector2Add(player.position, {-40, 20});
            boomerang.Controller[1].Coords = Vector2Add(player.position, {-1000, 0});
            boomerang.Controller[2].Coords = player.position;
        }

    }

    if (boomerang.Controller.size() == boomerang.control_input) {
        boomerang.Update();
        boomerang.Controller[boomerang.Controller.size()-1].Coords = player.position;
        
    }
}

