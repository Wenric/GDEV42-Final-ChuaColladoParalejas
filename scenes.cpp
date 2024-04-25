#include <raylib.h>
#include "scene_manager.hpp"
#include "ui_library.cpp"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "PlayerStateMachine.cpp"
#include "EnemyStateMachine.cpp"
#include <algorithm>

UiLibrary ui_library;

class TitleScene : public Scene {

public:

float centerX = GetScreenWidth() / 2;
float centerY = GetScreenHeight() / 2;

float buttonWidth = 350;
float buttonHeight = 60; 
float buttonX = centerX - (buttonWidth / 2);
float buttonY = centerY - (buttonHeight / 2);

Rectangle playButton = { buttonX, buttonY, buttonWidth, buttonHeight };
Rectangle scoreButton = { buttonX, buttonY + 90, buttonWidth, buttonHeight };
Rectangle quitButton = { buttonX, buttonY + 180, buttonWidth, buttonHeight };

// static Texture2D titleTexture;

    void Begin() override {
        // Load the title image
        // titleTexture = LoadTexture("resources/title.png");
    }

    void End() override {
        // UnloadTexture(titleTexture);
    }

    void Update() override {

    }

    void Draw() override {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), LIGHTGRAY);

        //DrawTexturePro(titleTexture, (Rectangle){ 0, 0, (float)titleTexture.width, (float)titleTexture.height },
        //(Rectangle){ (float)(GetScreenWidth() / 2 - titleTexture.width / 4), 100,
        //(float)(titleTexture.width / 2), (float)(titleTexture.height / 2) }, (Vector2){ 0, 0 }, 0, WHITE);

        // Play game button
        if (ui_library.Button(0, "Play", playButton))
        {
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(1);
            }
        }

        // Quit game button
        if (ui_library.Button(1, "Scores", scoreButton))
        {
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(2);
            }
        }

        if (ui_library.Button(2, "Quit", quitButton))
        {
            CloseWindow();
        }
    }

};

class GameScene : public Scene {

    //variables for tile functionality

    float accumulator = 0.0f, TIMESTEP = 1.0f / FPS;
    std::string line;
    std::string image_name;
    std::vector<Rectangle> sprite_rects;
    int x_dim, y_dim, tile_count;
    int tile_counter = 0;
    Tile* tile_list;
    Texture2D tilemap;
    std::vector<std::vector<int>> grid;
    float tile_size;
    std::vector<bool> tile_collision;
    Texture2D background;
    Sound shoot_sound;
    Sound boomerang_sound;
    Sound hurt_sound;
    Sound hit_sound;
    Sound enemy_attack_sound;

    //variables for camera
    Camera2D camera_view = { 0 };

    //variables for entities 

    //player instantiation
    Vector2 player_pos = {975, 192};
    float player_rad = 26.0f;
    float player_speed = 120.0f;
    const char *file_name = "resources/chara.png";
    Texture2D character = LoadTexture(file_name); 
    float rate = 0.5;
    Player player{player_pos, player_rad, player_speed, &character, rate};
    

    //enemy instantiation
    
    Texture2D enemy_one = LoadTexture("resources/enemy1.png");
    float enemy_rad = 32;
    float enemy_speed = 100.0f;
    Vector2 enemy_pos = {0,0};
    int enemy_counter = 0;
    int spawn_counter = 0;
    int enemy_count = 20;
    float spawn_timer = 2.50f;
    Vector2 enemy_spawnpoints[3] = {
        {38, 192}, {2516, 192}, {983, 32}
    };

    Enemy enemies[20] = {
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false},
        {enemy_pos, enemy_rad, enemy_speed, &enemy_one, false}
    };
    
    //camera instanations
    float EDGE_X[2], EDGE_Y[2];

    // health
    Texture2D healthTexture;
    int maxHealth = 5;
    int crossScale = 3.0f;

    //projectile stuff
    Texture2D bullet_texture;
    Texture2D boomerang_texure;


public:
    void Begin() override {

        player.position = player_pos;
        for (int i = 0; i < enemy_count; i++) {
            enemies[i].isAlive = false;
        }
        enemy_counter = 0;

        shoot_sound = LoadSound("resources/sfx/shoot.wav");
        boomerang_sound = LoadSound("resources/sfx/boomerang.wav");
        hurt_sound = LoadSound("resources/sfx/hurt.wav");
        enemy_attack_sound = LoadSound("resources/sfx/enemyattack.wav");
        hit_sound = LoadSound("resources/sfx/hit.wav");
        player.PassSoundInfo(shoot_sound, boomerang_sound, hurt_sound);

        //camera stuff
        score = 0;
        player.health = maxHealth;
        float enemy_timer = 3.0f;
        player.velocity.y = -5;
        player.fall_timer = 0;
        player.isAlive = true;
        camera_view.zoom = 1.0f;
        camera_view.offset = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
        camera_view.target = { 0, 0};
        //file reading
        std::ifstream settings("settings.txt");
        if (!settings.is_open()) {
            std::cout << "No File Found." << std::endl;
        } else {
            std::cout << "File Opened." << std::endl;
        }

        while (std::getline(settings, line)) {
            std::istringstream current(line);
            std::string checker;
            current >> checker;
            if (checker == "IMAGE_NAME") {
                current >> image_name;
            } else if (checker == "TILE_COUNT") {
                current >> tile_count;
            } else if (checker == "GRID") {
                current >> x_dim >> y_dim;
                grid.resize(x_dim, std::vector<int>(y_dim,0));
                grid.clear();
            } else if (checker == "TILE") {
                int x, y, width, height;
                bool has_collision;
                current >> x >> y >> width >> height >> has_collision;
                tile_collision.push_back(has_collision);
                sprite_rects.push_back({  (float)x, (float)y, (float)width, (float)height });
            } else if (checker == "ROW") {
                std::vector<int> row;
                int num;
                while (current >> num) {
                    row.push_back(num);
                }
                grid.push_back(row);
            }
        }
        settings.close();

        tile_list = new Tile[tile_count];
        for (int i = 0; i < tile_count; i++) {
            tile_list[i].loc_sprite_sheet = sprite_rects[i];
            tile_list[i].has_collision = tile_collision[i];
        }

        tilemap = LoadTexture(image_name.c_str());
        background = LoadTexture("resources/fullbg.png");
        tile_size = static_cast<int>(tile_list[0].loc_sprite_sheet.width);

        bullet_texture = LoadTexture("resources/bullet.png");
        boomerang_texure = LoadTexture("resources/boomerang.png");
        InitBulletArray();
        InitBoomerangArray();
        for (int i = 0; i < boomerang.Pasc_list.size(); i++) {   
        }

        player.PassCameraInfo(camera_view);
        
        for (int i = 0; i < 20; i++) {
            enemies[i].PassPlayerInfo(player);
            player.PassEntityInfo(enemies[i]);
            enemies[i].PassSoundInfo(hit_sound,enemy_attack_sound);
        }

       
        EDGE_X[0] = 0;
        EDGE_X[1] = 2700;
        EDGE_Y[0] = 200;
        EDGE_Y[1] = -854;

        healthTexture = LoadTexture("resources/health.png");

        


    }   

    void End() override {}

    void Update() override {

        std::cout << player.radius << std::endl;

        float deltaTime = GetFrameTime();
       
        player.Update(deltaTime);

        for (int i = 0; i < enemy_count; i++) {
            if (enemies[i].isAlive) {
                enemies[i].Update(deltaTime);
            }
            else {
                enemies[i].position = {-1000, 1000};
            }
        }

        accumulator += deltaTime;
        while (accumulator >= TIMESTEP) {
            player.PhysicsUpdate(TIMESTEP);
            for (int i = 0; i < enemy_count; i++) {
                if (enemies[i].isAlive) {
                    enemies[i].PhysicsUpdate(TIMESTEP);
                }
            }
            accumulator -= TIMESTEP;
        }
        
        for (int i = 0; i < y_dim; i++) {
            for (int j = 0; j < x_dim; j++) {
                if (tile_list[grid[i][j]].has_collision == true) {
                    player.CheckTileCollision({ j * tile_size, i * tile_size, tile_size, tile_size });
                    for (int k = 0; k < enemy_count; k++) {
                        enemies[k].CheckTileCollision({j * tile_size, i * tile_size, tile_size, tile_size});
                    }
                }
            }
        }
        if (abs(player.velocity.x) != 0.0f)
        {
            // Walking animation
            ++player.frameDelayCounter;
            if (player.frameDelayCounter > player.frameDelay)
            {
                player.frameDelayCounter = 0;
                ++player.frameIndex;
                player.frameIndex %= player.charaNumFrames;
                player.walkFrameRec.x = (float)64 * player.frameIndex;
                player.frameRec = player.walkFrameRec;
                
            }
        }
        else
        {
            // display first frame of walking animation
            player.frameIndex = 0; // reset frame index to 0
            player.walkFrameRec.x = 0.0f; // set it to first frame
            player.frameRec = player.walkFrameRec;
        }
        for (int i = 0; i < boomer_pack; ++i) {
            if(throws[i]->exists) {
                player.CheckProjectileCollision(*throws[i]);
            }
        }
            for (int i = 0; i < magazine; ++i) {
                if(ammo[i]->exists) {
                    player.CheckProjectileCollision(*ammo[i]);
                }
            }
    
        camera_view.target = player.position;

        camera_view.target.x = Clamp(camera_view.target.x, EDGE_X[0] + WINDOW_WIDTH/2, EDGE_X[1] -WINDOW_WIDTH/2 - 150);
        camera_view.target.y = Clamp(camera_view.target.y, EDGE_Y[1] + WINDOW_HEIGHT/2, EDGE_Y[0] - WINDOW_HEIGHT/4) - 60;

        player.position.x = Clamp(player.position.x, 32, 2516);
        player.position.y = Clamp(player.position.y, -854, 200);

        //enemy clampt to stage
        for (int i = 0; i < enemy_count; i++) {
            if(enemies[i].isAlive) {
                enemies[i].position.x = Clamp(enemies[i].position.x, 32, 2516);
                enemies[i].position.y = Clamp(enemies[i].position.y, -854, 200);
            }
        }

        // Check for space bar press to reset cooldown timer
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && rate <= 0.0f)
        {
            rate = 2.0f;
        }

        // update cooldown timer
        rate -= deltaTime;
        if (rate < 0.0f) {
            rate = 0.0f;
        }

        if (player.dash_cooldown < 0.0f) {
            player.dash_cooldown = 0.0f;
        }

        //enemy spawner
        if (enemy_counter >= enemy_count) {
            enemy_counter = 0;
        }
        if (spawn_counter >= 3) {
            spawn_counter = 0;
        }
        spawn_timer -= deltaTime;
        if (spawn_timer < 0) {
            enemies[enemy_counter].isAlive = true;
            enemies[enemy_counter].position = enemy_spawnpoints[spawn_counter];
            enemy_counter++;
            spawn_counter++;

            spawn_timer = 2.50f; 
        }

        if (!player.isAlive) {
            GetSceneManager()->SwitchScene(3);
        }
            

    }

    void Draw() override {
        BeginMode2D(camera_view);
        DrawTextureEx(background,{0,-825},0,1.5, WHITE);
         for (int i = 0; i < y_dim; i++) {
            for (int j = 0; j < x_dim; j++) {
                if (grid[i][j] != 0) {
                    DrawTexturePro(tilemap, tile_list[grid[i][j]].loc_sprite_sheet, { j * tile_size, i * tile_size, tile_size, tile_size }, { 0, 0 }, 0, WHITE);
                } 
            }
         }

        for (int i = 0; i < magazine; ++i) {
            if (ammo[i]->exists) {
                DrawTextureEx(bullet_texture, ammo[i]->position, 0, 1, WHITE);
            }
        }

        for (int i = 0; i < boomer_pack; ++i) {
            if (throws[i]->exists) {
                DrawTextureEx(boomerang_texure, {throws[i]->position.x - 20, throws[i]->position.y - 20}, 0, 2.5, WHITE);
            }
        }

        
        for (int i = 0; i < enemy_count; i++) {
            if (enemies[i].isAlive) {
                enemies[i].Draw();
            }
        }

        player.Draw();
        EndMode2D();

        DrawHealth();
        ui_library.ProgressBar("Dash Cooldown", player.dash_cooldown, 2.0f, { 12, 675, 305, 30 });
        Vector2 score_size = MeasureTextEx(GetFontDefault(), std::to_string(score).c_str(), 32, 1);
        DrawTextEx(GetFontDefault(), std::to_string(score).c_str(), {(WINDOW_WIDTH - score_size.x)/2, 10}, 64, 1, WHITE);
    }

    void DrawHealth() {
        float healthPosX = 10; // initial X position for the first cross

        for (int i = 0; i < maxHealth; i++) {
            if (i < player.health) {
                // draw the crosses
                DrawTextureEx(healthTexture, { healthPosX, 10 }, 0, crossScale, WHITE);
            }

            // position of the other crosses
            healthPosX += 65;
        }
    }
};


class ScoreScene : public Scene {

    std::vector<ScoreEntry> top_scores;
    int header_size = 48;
    int body_size = 32;
    float button_width = 400.0f;
    float button_height = 100.0f;
    
public:
    void Begin() override {
        
        std::ifstream score_file("scores.txt");
        ScoreEntry score_entry;

        // Read scores from the file
        while (score_file >> score_entry.name >> score_entry.score) {
            top_scores.push_back(score_entry);
        }
        std::sort(top_scores.begin(), top_scores.end(), 
        [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score;
        });

        // top scores only
        if (top_scores.size() > 15) {
            top_scores.resize(15);
        }
    }

    void Update() override {}

    void End() override {
        top_scores.clear();
    }

    void Draw() override {
        const char* title = "HIGHSCORES";
        const char* name = "ABC";
        Vector2 title_size = MeasureTextEx(GetFontDefault(), title, header_size, 2);
        Vector2 name_size = MeasureTextEx(GetFontDefault(), name, body_size, 1);

        // Calculate the starting X position for centering
        float title_middle = (WINDOW_WIDTH/2) - (title_size.x/2);

        // Draw the title
        DrawTextEx(GetFontDefault(), title, {title_middle, 50}, header_size, 1, BLACK);

        float starting_height = 120.0f; 

        for (size_t i = 0; i < top_scores.size(); ++i) {

            DrawTextEx(GetFontDefault(), top_scores[i].name.c_str(), 
            {(WINDOW_WIDTH/2) - name_size.x - 25, starting_height + (i * 30)}, body_size, 1,BLACK);

            DrawTextEx(GetFontDefault(), std::to_string(top_scores[i].score).c_str(), 
            {WINDOW_WIDTH/2 + 25, starting_height + (i * 30)}, body_size, 1, BLACK);
        }

        if (ui_library.Button(0, "Return",{ 100,
        WINDOW_HEIGHT-(button_height*2), button_width, button_height}))
        {
            GetSceneManager()->SwitchScene(0);
        }


}

};


class DeathScene : public Scene {
    int max_char = 3;
    char name[4] = "\0"; // Increase array size to 4 to include the null terminator
    int letter_count = 0;
    Rectangle text_box = { WINDOW_WIDTH / 2 - 150, 300, 300, 50 }; // Centered text box
    bool mouse_over_text = false;
    Font custom_font;

    float button_width = 400;
    float button_height = 100;
    int previous_key; 

public:
    void Begin() override {
        int previous_key; 
    }

    void Update() override {
        // Input box functionality
        mouse_over_text = CheckCollisionPointRec(GetMousePosition(), text_box);

        if (mouse_over_text) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            
            int key = GetKeyPressed();

            if (key > 0 && key != previous_key) {
                previous_key = key; 

                if ((key >= 'A' && key <= 'Z') && (letter_count < max_char)) {
                    name[letter_count] = static_cast<char>(key);
                    name[letter_count + 1] = '\0';
                    letter_count++;
                }

                
            }
        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
                    letter_count--;
                    if (letter_count < 0) {
                        letter_count = 0;
                    }
                    name[letter_count] = '\0';
                }
    }


    void End() override {
        score = 0;
        name[0] = '\0';
        letter_count = 0;
    }

    void Draw() override {
    
        DrawTextEx(custom_font, "GAME OVER!", {(WINDOW_WIDTH - MeasureText("Game Over!", 64)) / 2.0f, 100}, 48, 1, RED);


        std::string score_text = "You scored " + std::to_string(score) + " points!";
        DrawTextEx(custom_font, score_text.c_str(), {(WINDOW_WIDTH - MeasureText(score_text.c_str(), 48)) / 2.0f, 150}, 48, 1, BLACK);

        // Draw text box
        DrawRectangleRec(text_box, LIGHTGRAY);
        if (mouse_over_text) {
            DrawRectangleLines(text_box.x, text_box.y, text_box.width, text_box.height, RED);
        } else {
            DrawRectangleLines(text_box.x, text_box.y, text_box.width, text_box.height, BLACK);
        }

        DrawTextEx(custom_font, name, {text_box.x + 15, text_box.y + 8}, 48, 1, MAROON);
        DrawTextEx(custom_font, TextFormat("INPUT NAME: %d/%d", letter_count, max_char), {text_box.x - 110, text_box.y - 50}, 32, 1, BLACK);

        if (ui_library.Button(1, "Return", {100, WINDOW_HEIGHT - button_height - 10, button_width, button_height})) {
            GetSceneManager()->SwitchScene(0);
        }

        if (ui_library.Button(2, "Save Score", {WINDOW_WIDTH - button_width - 100, WINDOW_HEIGHT - button_height - 10, button_width, button_height})) {
            std::ofstream score_file("scores.txt", std::ios::app);

            if (letter_count == max_char && score > 0) {
                if (score_file.is_open()) {
                    std::string name_str(name);
                    score_file << "\n" << name_str << " " << score;
                    score_file.close();  // Close the file after writing
                } else {
                    std::cerr << "Unable to open scores.txt for writing." << std::endl;
                }
                GetSceneManager()->SwitchScene(0);
            }
        }
    }
};


