#include <raylib.h>
#include <C:\raylib\raylib\src\raymath.h>
#include "Player.hpp"
#include <iostream>

int magazine = 20; //amount of bullets
int boomer_pack = 1;
Bullet* ammo[20];
Boomerang* throws[1];
Vector2 direction; 
Vector2 dest;

Bezier boomerang{2, 3, 5, 100, 10, BLUE}; //instantiate Bezier Curve

void InitBulletArray() {
    for (int i = 0; i < magazine; ++i) {
        ammo[i] = new Bullet(1, RED, 5.0f, {0, 0}, {0, 0}, 500, true, false);
    }
}

void Player::PassSoundInfo(Sound& shoot, Sound& boom, Sound& hurt) {
    shoot_sound = &shoot;
    boomerang_sound = &boom;
    damage_sound = &hurt;
}

void Player::Jump() {
    if (IsKeyPressed(KEY_SPACE) && !isFalling) {
        velocity.y -= 25;
        fall_timer = 0;
    }
}

void InitBoomerangArray() {
    for (int i = 0; i < boomer_pack; ++i) {
        throws[i] = new Boomerang(3, GREEN, 20.0f, {0, 0}, {0, 0}, 700, true, false);
    }
}

void Player::PhysicsUpdate(float TIMESTEP) {
    //gravity
    float down_force = (velocity.y * fall_timer) + (0.5 * 9.8 * pow(fall_timer, 2) * 7);

    if (down_force < 5) {
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
    DrawTexturePro(*texture, frameRec,
            (Rectangle){position.x - 32, position.y - 32, 64, 64},
            (Vector2){0, 0}, 0, WHITE);
}

void Player::SetState(PlayerState* new_state) {
    current_state = new_state;
    current_state->Enter(*this);
}

void Player::PassEntityInfo(Entity& enemy) {
    entities.push_back(&enemy);
}

void Player::PassCameraInfo(Camera2D& cam) {
    camera = &cam;
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


Player::Player(Vector2 pos, float rad, float spd, Texture2D* tex, float rate) {
    position = pos;
    radius = rad;
    speed = spd;
    fall_timer = 0;
    health = 5;
    isFalling = true;
    fall_timer = 0.0f;
    color = WHITE;
    dash_cooldown = 0;
    iframe_time = 0.5f;
    isAlive = true;
    isHit = false;
    velocity = Vector2Zero();
    texture = tex;
    fireRate = rate;
    SetState(&idle);

    charaNumFrames = 2;
    charaFrameWidth = 128 / charaNumFrames;
    walkFrameRec = { 0.0f, 0.0f, (float)64, (float)64};

    frameDelay = 8;
    frameDelayCounter = 0;
    frameIndex = 0;

    frameRec;
}

void Player::BoomerShoot(float delta_time) {
    for (int i = 0; i < boomer_pack; ++i) {
        if (throws[i]->exists) {
            for (float e = 0; e < boomerang.step_input + 1; e++) {
                if(CheckCollisionPointCircle(boomerang.Step_coords[e], throws[i]->position, throws[i]->collision_rad)) {
                    dest = boomerang.Step_coords[e + 1];
                    dest = Vector2Scale(Vector2Normalize(Vector2Subtract(dest, throws[i]->position)), throws[i]->speed * delta_time);    
                    }
            }
            throws[i]->position = Vector2Add(throws[i]->position, dest);

            if (CheckCollisionCircles(position, radius, throws[i]->position, throws[i]->collision_rad)) {
                throws[i]->exists = false;
            }        
        }
    }
}


void PlayerIdle::Update(Player& player, float delta_time) {
    
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        player.SetState(&player.moving);
    } 
    if (IsKeyPressed(KEY_SPACE) && !player.isFalling) {
        player.Jump(); 
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        PlaySound(*player.shoot_sound);
        player.fireRate = 2;
        direction = Vector2Zero();
        direction = Vector2Normalize(Vector2Subtract(GetMousePosition(), GetWorldToScreen2D(player.position, *player.camera)));
        
        for (int i = 0; i < magazine; ++i) {
            if (!ammo[i]->exists) {
                // Set bullet properties
                ammo[i]->position = player.position;
                ammo[i]->exists = true;
                direction = Vector2Scale(direction, ammo[i]->speed);
                ammo[i]->direction = direction;
                // bullet = ammo[i];
                break; // Exit the loop after activating one bullet
            }
        }
    }

    // update cooldown timer
    player.fireRate -= delta_time;
    if (player.fireRate < 0.0f)
        player.fireRate = 0.0f;

    for (int i = 0; i < magazine; ++i) {
            if (ammo[i]->exists) {
                ammo[i]->position = Vector2Add(ammo[i]->position, Vector2Scale(ammo[i]->direction, delta_time));
            }

            if (!CheckCollisionCircleRec(ammo[i]->position, ammo[i]->radius, {0, -854, 2700, 1050})) {
                ammo[i]->exists = false;
            }
        }
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        if (!player.direction) {
            PlaySound(*player.boomerang_sound);
            boomerang.Controller[0].Coords = Vector2Add(player.position, {40, 20});
            boomerang.Controller[1].Coords = Vector2Add(player.position, {1000, 0});
            boomerang.Controller[2].Coords = player.position;

        }
        else {
            PlaySound(*player.boomerang_sound);
            boomerang.Controller[0].Coords = Vector2Add(player.position, {-40, 20});
            boomerang.Controller[1].Coords = Vector2Add(player.position, {-1000, 0});
            boomerang.Controller[2].Coords = player.position;
        }
        for (int i = 0; i < boomer_pack; ++i) {
            if (!throws[i]->exists) {
                // Set bullet properties
                throws[i]->position = boomerang.Controller[0].Coords;
                throws[i]->exists = true;
                break; // Exit the loop after activating one bullet
            }
        }
    }

    player.BoomerShoot(delta_time);
        

    if(boomerang.Controller.size() == boomerang.control_input) {
        boomerang.Update();
        // for (int i = 0; i < boomerang.Controller.size(); i++) {
        //     boomerang.Controller[i].Coords = Vector2Add(boomerang.Controller[i].Coords, player.position);
        // }
    }
}

void PlayerMoving::Update(Player& player, float delta_time) {
    player.velocity.x = 0;
    if (IsKeyDown(KEY_A)) {
        player.velocity.x = player.speed;
        player.direction = -1;
        if (player.walkFrameRec.width < 0)
        {
            player.walkFrameRec.width = -player.walkFrameRec.width;
        }
        
    } else if (IsKeyDown(KEY_D)) {
        player.velocity.x = player.speed;
        player.direction = 1;
        if (player.walkFrameRec.width > 0)
        {
            player.walkFrameRec.width = -player.walkFrameRec.width;
        }
        
    }
    else if(player.velocity.x == 0 && player.velocity.y == 0) {
        player.SetState(&player.idle);
    }

    if (IsKeyPressed(KEY_SPACE) && !player.isFalling) {
        player.Jump();
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        player.fireRate = 2;
        PlaySound(*player.shoot_sound);
        direction = Vector2Zero();
        direction = Vector2Normalize(Vector2Subtract(GetMousePosition(), GetWorldToScreen2D(player.position, *player.camera)));
        
        for (int i = 0; i < magazine; ++i) {
            if (!ammo[i]->exists) {
                // Set bullet properties
                ammo[i]->position = player.position;
                ammo[i]->exists = true;
                direction = Vector2Scale(direction, ammo[i]->speed);
                ammo[i]->direction = direction;
                // bullet = ammo[i];
                break; // Exit the loop after activating one bullet
            }
        }
    }

    // update cooldown timer
    player.fireRate -= delta_time;
    if (player.fireRate < 0.0f)
        player.fireRate = 0.0f;

    for (int i = 0; i < magazine; ++i) {
            if (ammo[i]->exists) {
                ammo[i]->position = Vector2Add(ammo[i]->position, Vector2Scale(ammo[i]->direction, delta_time));
            }

            if (!CheckCollisionCircleRec(ammo[i]->position, ammo[i]->radius, {0, -854, 2700, 1050})) {
                ammo[i]->exists = false;
            }
        }

    if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        if (player.direction == 1) {
            PlaySound(*player.boomerang_sound);
            boomerang.Controller[0].Coords = Vector2Add(player.position, {40, 20});
            boomerang.Controller[1].Coords = Vector2Add(player.position, {1000, 0});
            boomerang.Controller[2].Coords = player.position;

        }
        else {
            PlaySound(*player.boomerang_sound);
            boomerang.Controller[0].Coords = Vector2Add(player.position, {-40, 20});
            boomerang.Controller[1].Coords = Vector2Add(player.position, {-1000, 0});
            boomerang.Controller[2].Coords = player.position;
        }
        for (int i = 0; i < boomer_pack; ++i) {
            if (!throws[i]->exists) {
                // Set bullet properties
                throws[i]->position = boomerang.Controller[0].Coords;
                throws[i]->exists = true;
                break; // Exit the loop after activating one bullet
            }
        }

    }
    player.BoomerShoot(delta_time);

    if (boomerang.Controller.size() == boomerang.control_input) {
        boomerang.Update();
        boomerang.Controller[boomerang.Controller.size()-1].Coords = player.position;

    

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

