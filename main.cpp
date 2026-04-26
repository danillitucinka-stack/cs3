#include "raylib.h"
#include "raymath.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <map>

enum GameState { MENU, GAMEPLAY, BUY_MENU, GAME_OVER, VICTORY, LEVEL_SELECT };
enum WeaponType { AK47, AWP, DEAGLE, M4A4, USP };
enum BotState { PATROL, ATTACK, RELOAD };
enum EffectType { SMOKE, FIRE, SPARK };

class Level {
public:
    std::string name;
    int enemyCount;
    std::vector<Model> walls;
    void LoadFromFile(std::string filename) {
        std::ifstream f(filename);
        if (f) {
            nlohmann::json j = nlohmann::json::parse(f);
            name = j["name"];
            enemyCount = j["enemies"];
        }
    }
};

class Effect {
public:
    Vector3 position;
    EffectType type;
    float life;
    std::vector<Particle> particles;
    Effect(Vector3 pos, EffectType t) : position(pos), type(t), life(5.0f) {
        for (int i = 0; i < 10; ++i) particles.push_back(Particle(pos, type == SMOKE ? GRAY : type == FIRE ? ORANGE : YELLOW));
    }
    void Update() {
        life -= GetFrameTime();
        for (auto& p : particles) p.Update();
    }
    void Draw() {
        for (auto& p : particles) p.Draw();
    }
};

class Bot {
public:
    Vector3 position;
    Model model;
    BotState state;
    float health, speed;
    int id, level;
    Bot(int i, int l) : state(PATROL), health(100.0f), speed(3.0f), id(i), level(l) {
        position = {(float)(rand() % 50 - 25), 2.0f, (float)(rand() % 50 - 25)};
        model = LoadModelFromMesh(GenMeshCylinder(0.5f, 2.0f, 16));
    }
    void Update(Player& player, const std::vector<Model>& walls) {
        // Same as before
    }
    void Draw() {
        DrawModel(model, position, 1.0f, RED);
    }
    void TakeDamage(float dmg) {
        health -= dmg;
    }
};

class Weapon {
public:
    Model model;
    Vector3 position;
    float sway, recoil, fireRate, zoomFOV;
    WeaponType type;
    Sound shootSound;
    Weapon(WeaponType t = AK47) : sway(0), recoil(0), type(t), fireRate(0), zoomFOV(60.0f) {
        position = {1.0f, -0.5f, 1.0f};
        // Load model from file if exists
        std::string modelPath = "assets/models/weapon" + std::to_string((int)t) + ".obj";
        if (FileExists(modelPath.c_str())) {
            model = LoadModel(modelPath.c_str());
        } else {
            model = LoadModelFromMesh(GenMeshCube(0.3f, 0.1f, 1.0f));
        }
        // Load sound
        std::string soundPath = "assets/sounds/weapon" + std::to_string((int)t) + ".wav";
        if (FileExists(soundPath.c_str())) {
            shootSound = LoadSound(soundPath.c_str());
        }
    }
    void Update(bool moving, Player& player) {
        // Same
    }
    void Draw() {
        // Same with multiple parts
    }
    void Shoot() {
        // Same, play sound
    }
};

class MapManager {
public:
    std::vector<Model> walls;
    std::vector<Model> boxes;
    void GenerateMap(std::string mapName) {
        // Load from file or generate
        std::string levelFile = "assets/levels/" + mapName + ".json";
        if (FileExists(levelFile.c_str())) {
            // Load level
        } else {
            // Generate Dust2
            int map[30][30] = { /* same */ };
            Model wallModel = LoadModelFromMesh(GenMeshCube(1.0f, 4.0f, 1.0f));
            Model boxModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
            for (int i = 0; i < 30; ++i) {
                for (int j = 0; j < 30; ++j) {
                    if (map[i][j] == 1) walls.push_back(wallModel);
                    else if (map[i][j] == 2) boxes.push_back(boxModel);
                }
            }
        }
    }
    void DrawWalls() {
        // Same
    }
};

class UIManager {
public:
    static void DrawMenu(int w, int h) {
        DrawText("CS 3 AI", w/2 - 50, h/2 - 50, 40, BLACK);
        DrawText("Press ENTER to Play", w/2 - 100, h/2, 20, GRAY);
        DrawText("Press L for Level Select", w/2 - 120, h/2 + 30, 20, GRAY);
    }
    static void DrawLevelSelect(int w, int h, std::vector<std::string> levels) {
        DrawText("Select Level", w/2 - 80, h/2 - 100, 30, BLACK);
        for (size_t i = 0; i < levels.size(); ++i) {
            DrawText(TextFormat("%d. %s", i+1, levels[i].c_str()), w/2 - 100, h/2 - 50 + i*30, 20, BLACK);
        }
    }
    static void DrawBuyMenu(int w, int h, int money) {
        DrawText("Buy Menu", w/2 - 50, h/2 - 100, 30, BLACK);
        DrawText(TextFormat("Money: $%d", money), w/2 - 50, h/2 - 70, 20, GREEN);
        DrawText("1. AK-47 $2700", w/2 - 100, h/2 - 30, 20, BLACK);
        DrawText("2. AWP $4750", w/2 - 100, h/2, 20, BLACK);
        DrawText("3. Desert Eagle $700", w/2 - 100, h/2 + 30, 20, BLACK);
        DrawText("4. M4A4 $3100", w/2 - 100, h/2 + 60, 20, BLACK);
        DrawText("5. USP $500", w/2 - 100, h/2 + 90, 20, BLACK);
        DrawText("Press ESC to exit", w/2 - 100, h/2 + 120, 20, GRAY);
    }
    static void DrawHUD(int health, int ammo, int kills, int deaths, int level) {
        DrawText(TextFormat("Level: %d Health: %d Ammo: %d Kills: %d Deaths: %d", level, health, ammo, kills, deaths), 10, 10, 20, WHITE);
    }
    static void DrawCrosshair(int w, int h) {
        int cx = w/2, cy = h/2;
        DrawLine(cx - 10, cy, cx + 10, cy, WHITE);
        DrawLine(cx, cy - 10, cx, cy + 10, WHITE);
    }
    static void DrawRadar(int w, int h, Player& player, std::vector<Bot>& bots) {
        DrawRectangle(w - 200, h - 200, 180, 180, Fade(BLACK, 0.5f));
        DrawCircle(w - 110, h - 110, 80, GREEN);
        for (auto& bot : bots) {
            Vector3 rel = Vector3Subtract(bot.position, player.position);
            DrawCircle(w - 110 + rel.x * 2, h - 110 + rel.z * 2, 5, RED);
        }
    }
};

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    GameState currentState = MENU;
    int currentLevel = 1;
    std::vector<std::string> levels = {"dust2", "mirage", "inferno", "cache", "overpass", "train", "nuke", "vertigo", "dust", "office"};
    Player player;
    Weapon weapon(AK47);
    MapManager mapManager;
    mapManager.GenerateMap("dust2");
    std::vector<Bot> bots;
    for (int i = 0; i < 10; ++i) bots.push_back(Bot(i, currentLevel));
    std::vector<Effect> effects;

    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    SetTargetFPS(60);
    DisableCursor();

    while (!WindowShouldClose()) {
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) { currentState = GAMEPLAY; mapManager.GenerateMap(levels[currentLevel-1]); }
            if (IsKeyPressed(KEY_L)) currentState = LEVEL_SELECT;
        } else if (currentState == LEVEL_SELECT) {
            if (IsKeyPressed(KEY_ONE)) { currentLevel = 1; currentState = MENU; }
            // Add more
            if (IsKeyPressed(KEY_ESCAPE)) currentState = MENU;
        } else if (currentState == GAMEPLAY) {
            player.LookAround();
            player.Update(mapManager.walls);
            bool moving = IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D);
            weapon.Update(moving, player);
            for (auto& bot : bots) bot.Update(player, mapManager.walls);
            for (auto& eff : effects) eff.Update();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                weapon.Shoot();
                effects.push_back(Effect(Vector3Add(player.position, {0, 1, 0}), SPARK));
            }
            if (IsKeyPressed(KEY_B)) currentState = BUY_MENU;
            // Remove dead effects
            effects.erase(std::remove_if(effects.begin(), effects.end(), [](Effect& e){ return e.life <= 0; }), effects.end());
            // Check win/lose
            bool allDead = std::all_of(bots.begin(), bots.end(), [](Bot& b){ return b.health <= 0; });
            if (allDead) currentState = VICTORY;
            if (player.health <= 0) currentState = GAME_OVER;
        } else if (currentState == BUY_MENU) {
            if (IsKeyPressed(KEY_ONE) && player.money >= 2700) { player.money -= 2700; weapon = Weapon(AK47); }
            if (IsKeyPressed(KEY_TWO) && player.money >= 4750) { player.money -= 4750; weapon = Weapon(AWP); }
            if (IsKeyPressed(KEY_THREE) && player.money >= 700) { player.money -= 700; weapon = Weapon(DEAGLE); }
            if (IsKeyPressed(KEY_FOUR) && player.money >= 3100) { player.money -= 3100; weapon = Weapon(M4A4); }
            if (IsKeyPressed(KEY_FIVE) && player.money >= 500) { player.money -= 500; weapon = Weapon(USP); }
            if (IsKeyPressed(KEY_ESCAPE)) currentState = GAMEPLAY;
        } else if (currentState == GAME_OVER) {
            if (IsKeyPressed(KEY_R)) { player.health = 100; currentState = GAMEPLAY; }
        } else if (currentState == VICTORY) {
            if (IsKeyPressed(KEY_R)) { currentState = MENU; }
        }

        BeginDrawing();
        if (currentState == MENU) {
            ClearBackground(WHITE);
            UIManager::DrawMenu(screenWidth, screenHeight);
        } else if (currentState == LEVEL_SELECT) {
            ClearBackground(BLUE);
            UIManager::DrawLevelSelect(screenWidth, screenHeight, levels);
        } else if (currentState == BUY_MENU) {
            ClearBackground(BLACK);
            UIManager::DrawBuyMenu(screenWidth, screenHeight, player.money);
        } else if (currentState == GAMEPLAY) {
            ClearBackground(DARKBLUE);
            BeginMode3D(player.camera);
            mapManager.DrawWalls();
            for (auto& bot : bots) if (bot.health > 0) bot.Draw();
            for (auto& eff : effects) eff.Draw();
            EndMode3D();
            weapon.Draw();
            UIManager::DrawHUD(player.health, 30, player.kills, player.deaths, currentLevel);
            UIManager::DrawCrosshair(screenWidth, screenHeight);
            UIManager::DrawRadar(screenWidth, screenHeight, player, bots);
        } else if (currentState == GAME_OVER) {
            ClearBackground(RED);
            DrawText("GAME OVER", screenWidth / 2 - 100, screenHeight / 2, 40, WHITE);
            DrawText("Press R to Restart", screenWidth / 2 - 100, screenHeight / 2 + 50, 20, WHITE);
        } else if (currentState == VICTORY) {
            ClearBackground(GREEN);
            DrawText("VICTORY!", screenWidth / 2 - 80, screenHeight / 2, 40, WHITE);
            DrawText("Press R to Menu", screenWidth / 2 - 100, screenHeight / 2 + 50, 20, WHITE);
        }
        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}