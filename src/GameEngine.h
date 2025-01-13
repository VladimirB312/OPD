#ifndef GAMEENGINE_H
#define GAMEENGINE_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <list>
#include "Bullet.h"
#include "Door.h"
#include "Player.h"
#include "Enemy.h"
#include "GameScreens.h"
#include "PickupItem.h"
#include "GameHUD.h"
#include "TmxLevel.h"

constexpr float WINDOW_HEIGHT = 480;
constexpr float WINDOW_WIDTH = 854;


enum class State { paused, playing, reset, game_over, win };

class GameEngine {
public:
    State state;
    GameEngine();

    void run();

private:
    sf::RenderWindow window;

    TmxLevel level;
    std::vector<TmxObject> solidObjects;
    std::vector<TmxObject> objectsForEnemies;
    TmxObject exitObject;
    sf::Time gameTime;

    float mapWidth;
    float mapHeight;
    sf::View mainView = sf::View(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

    Player player;
    GameHUD gameHUD;
    GameScreens gameScreens;
    std::list<Enemy*> enemies;
    std::list<Bullet*> bullets;
    std::list<PickupItem*> pickupItems;
    std::map<int, Door*> doors;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::SoundBuffer deadSoundBuffer;
    sf::SoundBuffer winSoundBuffer;
    sf::Sound deadSound;
    sf::Sound winSound;
    sf::Music gameMusic;

    void play();

    void input();

    void update(float deltaTime);
    void updateView();
    void updateBullets(float deltaTime);
    void updateEnemies(float deltaTime);
    void updatePickupItems(float deltaTime);

    void draw();
    void drawBullets();
    void drawEnemies();
    void drawPickupItems();
    void drawDoors();

    void initResources();
    void initLevel();
    void initDoors();

    bool checkExit();
    void updateDoors();
    void clearLevel();
    void resetLevel();
};


#endif //GAMEENGINE_H
