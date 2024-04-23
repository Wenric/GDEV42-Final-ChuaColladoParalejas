#include <raylib.h>
#include "scene_manager.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Entity.hpp"
#include "PlayerStateMachine.cpp"


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

    Vector2 player_pos = {20, -80};
    float player_rad = 32.0f;
    float player_speed = 100.0f;

    Player player{player_pos, player_rad, player_speed};

    //variables for camera
    Camera2D camera_view = { 0 };

    //variables for entities

public:
    void Begin() override {
        //camera stuff
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
                std::cout << "pog" << std::endl;
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
        tile_size = static_cast<int>(tile_list[0].loc_sprite_sheet.width);

    }   

    void End() override {}

    void Update() override {
        
        float deltaTime = GetFrameTime();
        player.Update(deltaTime);
        accumulator += deltaTime;
        while (accumulator >= TIMESTEP) {
            // Calculate next position, next velocity, etc.
            player.PhysicsUpdate(TIMESTEP);
            accumulator -= TIMESTEP;
        }
        
        for (int i = 0; i < y_dim; i++) {
            for (int j = 0; j < x_dim; j++) {
                if (tile_list[grid[i][j]].has_collision == true) {
                    player.CheckTileCollision({ j * tile_size, i * tile_size, tile_size, tile_size });
                    
                }
            }
        }
    }

    void Draw() override {
        BeginMode2D(camera_view);
         for (int i = 0; i < y_dim; i++) {
            for (int j = 0; j < x_dim; j++) {
                if (grid[i][j] != 0) {
                    DrawTexturePro(tilemap, tile_list[grid[i][j]].loc_sprite_sheet, { j * tile_size, i * tile_size, tile_size, tile_size }, { 0, 0 }, 0, WHITE);
                } 
            }
         }
        player.Draw();
        EndMode2D();
    }
};
