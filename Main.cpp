#include <raylib.h>
#include <C:\raylib\raylib\src\raymath.h>
#include "scene_manager.hpp"
#include "scenes.cpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ChuaColladoParalejasFinalProj");
    SetTargetFPS(FPS);

    SceneManager scene_manager;
    
    TitleScene title_scene;
    title_scene.SetSceneManager(&scene_manager);

    GameScene game_scene;
    game_scene.SetSceneManager(&scene_manager);

    ScoreScene score_scene;
    score_scene.SetSceneManager(&scene_manager);

    DeathScene death_scene;
    death_scene.SetSceneManager(&scene_manager);
    
    scene_manager.RegisterScene(&title_scene, 0);
    scene_manager.RegisterScene(&game_scene, 1);
    scene_manager.RegisterScene(&score_scene, 2);
    scene_manager.RegisterScene(&death_scene, 3);

    scene_manager.SwitchScene(0);

    InitAudioDevice();

    while(!WindowShouldClose()) {
        Scene* active_scene = scene_manager.GetActiveScene();

        BeginDrawing();
        ClearBackground(WHITE);

        if (active_scene != nullptr) {
            active_scene->Update();
            active_scene->Draw();
        }
        EndDrawing();
    }

    Scene* active_scene = scene_manager.GetActiveScene();
    if (active_scene != nullptr) {
        active_scene->End();
    }

    ResourceManager::GetInstance()->UnloadAllTextures();

    CloseWindow();
    return 0;
}