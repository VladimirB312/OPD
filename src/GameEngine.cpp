#include "GameEngine.h"
#include "GameScreens.h"

GameEngine::GameEngine() : gameScreens(window, mainView) {
    window.create(sf::VideoMode(1920, 1080), "game", sf::Style::Fullscreen);
    initResources();

    state = State::paused;
    level.LoadFromFile("resources/levels/level.tmx");
    initLevel();
}

void GameEngine::initResources() {
    backgroundTexture.loadFromFile("resources/sprites/background.jpg");
    backgroundSprite.setTexture(backgroundTexture);

    deadSoundBuffer.loadFromFile("resources/sounds/dead_sound.wav");
    deadSound.setBuffer(deadSoundBuffer);

    winSoundBuffer.loadFromFile("resources/sounds/win_sound.wav");
    winSound.setBuffer(winSoundBuffer);

    gameMusic.openFromFile("resources/sounds/game_music.ogg");
    gameMusic.setVolume(30);
}

void GameEngine::initLevel() {
    mapWidth = level.GetTilemapWidth();
    mapHeight = level.GetTilemapHeight();

    solidObjects = level.GetAllObjects("solid");

    objectsForEnemies = level.GetAllObjects("solidForEnemy");
    objectsForEnemies.insert(objectsForEnemies.end(), solidObjects.begin(), solidObjects.end());

    TmxObject playerObject = level.GetFirstObject("player");
    player.init(playerObject.rect.left, playerObject.rect.top);

    std::vector<TmxObject> enemiesObjects = level.GetAllObjects("enemy");
    for (const auto& enemy : enemiesObjects) {
        enemies.push_back(new Enemy(enemy.rect.left, enemy.rect.top, enemy.type));
    }

    std::vector<TmxObject> ammoObjects = level.GetAllObjects("ammoPack");
    for (const auto& ammo : ammoObjects) {
        pickupItems.push_back(new PickupItem(ammo.rect.left, ammo.rect.top, PickupItem::Item::ammoPack));
    }

    std::vector<TmxObject> healthPackObjects = level.GetAllObjects("healthPack");
    for (const auto& healthPack : healthPackObjects) {
        pickupItems.push_back(new PickupItem(healthPack.rect.left, healthPack.rect.top, PickupItem::Item::healthPack));
    }

    initDoors();

    exitObject = level.GetFirstObject("exit");
}

void GameEngine::initDoors() {
    std::vector<TmxObject> doorsObjects = level.GetAllObjects("door");
    for (const auto& door : doorsObjects) {
        int index = std::stoi(door.type);

        if (doors.find(index) == doors.end()) {
            doors[index] = new Door();
        }

        doors[index]->setDoorPosition(door.rect.left, door.rect.top);
    }

    std::vector<TmxObject> keysObjects = level.GetAllObjects("key");
    for (const auto& key : keysObjects) {
        int index = std::stoi(key.type);
        doors[index]->setKeyPosition(key.rect.left, key.rect.top);
    }
}

void GameEngine::clearLevel() {
    for (auto it = bullets.begin(); it != bullets.end();) {
        Bullet* bullet = *it;
        it = bullets.erase(it);
        delete bullet;
    }

    for (auto it = enemies.begin(); it != enemies.end();) {
        Enemy* enemy = *it;
        it = enemies.erase(it);
        delete enemy;
    }

    for (auto it = pickupItems.begin(); it != pickupItems.end();) {
        PickupItem* item = *it;
        it = pickupItems.erase(it);
        delete item;
    }

    for (auto& [index, door] : doors) {
        delete door;
    }

    player.health = 100;
    solidObjects.clear();
    objectsForEnemies.clear();
    pickupItems.clear();
    doors.clear();
}

void GameEngine::resetLevel() {
    clearLevel();
    initLevel();
}

void GameEngine::run() {
    while (window.isOpen()) {
        switch (state) {
        case State::paused:
            gameMusic.pause();
            gameScreens.openPauseMenuScreen(state);
            break;
        case State::playing:
            gameMusic.play();
            play();
            break;
        case State::reset:
            gameMusic.stop();
            gameMusic.play();
            resetLevel();
            state = State::playing;
            break;
        case State::game_over:
            gameMusic.stop();
            gameScreens.openDeathScreen(state);
            resetLevel();
            break;
        case State::win:
            gameMusic.stop();
            gameScreens.openWinScreen(state);
            resetLevel();
        default:
            break;
        }
    }
}

void GameEngine::play() {
    sf::Clock clock;

    while (window.isOpen() && state == State::playing) {
        float deltaTime = clock.getElapsedTime().asSeconds();
        gameTime += clock.restart();
        input();
        update(deltaTime);
        draw();
    }
}

void GameEngine::input() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type != sf::Event::KeyPressed) {
            continue;
        }
        if (event.key.code == sf::Keyboard::Escape) {
            state = State::paused;
            return;
        }
    }

    player.handleInput(bullets, gameTime);
}

void GameEngine::updateView() {
    sf::Vector2f offset;
    offset.x = player.getRect().left + player.getRect().width / 2;
    offset.y = player.getRect().top + player.getRect().height / 2;

    if (offset.x < WINDOW_WIDTH / 2) {
        offset.x = WINDOW_WIDTH / 2;
    }

    if (offset.x > mapWidth - WINDOW_WIDTH / 2) {
        offset.x = mapWidth - WINDOW_WIDTH / 2;
    }

    if (offset.y < WINDOW_HEIGHT / 2) {
        offset.y = WINDOW_HEIGHT / 2;
    }

    if (offset.y > mapHeight - WINDOW_HEIGHT / 2) {
        offset.y = mapHeight - WINDOW_HEIGHT / 2;
    }

    mainView.setCenter(offset);
    window.setView(mainView);
}

void GameEngine::updateBullets(const float deltaTime) {
    for (auto it = bullets.begin(); it != bullets.end();) {
        Bullet* bullet = *it;
        bullet->update(deltaTime, solidObjects);
        bullet->updatePlayer(player);
        bullet->updateEnemies(enemies);
        bullet->updateDoors(doors);
        if (bullet->isExploding) {
            it = bullets.erase(it);
            delete bullet;
            continue;
        }

        ++it;
    }
}

void GameEngine::updateEnemies(float deltaTime) {
    for (auto const& enemy : enemies) {
        if (!enemy->life) {
            continue;
        }

        enemy->update(player.getRect(), objectsForEnemies, doors, bullets, deltaTime);
        player.updateEnemy(*enemy, gameTime);
    }
}

void GameEngine::updatePickupItems(const float deltaTime) {
    for (auto it = pickupItems.begin(); it != pickupItems.end();) {
        PickupItem* item = *it;
        item->updatePlayer(player, deltaTime);

        if (!item->life) {
            it = pickupItems.erase(it);
            delete item;
            continue;
        }

        ++it;
    }
}

bool GameEngine::checkExit() {
    for (const auto& [index, door] : doors) {
        if (!door->isOpen) {
            return false;
        }
    }
    return exitObject.rect.intersects(player.getRect());
}

void GameEngine::updateDoors() {
    for (const auto& [index, door] : doors) {
        door->update();
    }
}

void GameEngine::update(const float deltaTime) {
    updateBullets(deltaTime);
    player.update(solidObjects, doors, deltaTime);
    updateEnemies(deltaTime);
    updatePickupItems(deltaTime);
    updateDoors();
    updateView();
    gameHUD.update(mainView.getCenter(), mainView.getSize(), player);

    if (player.health <= 0) {
        deadSound.play();
        state = State::game_over;
    }

    if (checkExit()) {
        state = State::win;
        winSound.play();
    }
}

void GameEngine::drawBullets() {
    for (auto& bullet : bullets) {
        window.draw(bullet->sprite);
    }
}

void GameEngine::drawEnemies() {
    for (auto const& enemy : enemies) {
        window.draw(enemy->sprite);
    }
}

void GameEngine::drawPickupItems() {
    for (auto const& item : pickupItems) {
        if (item->pickedUp) {
            continue;
        }
        window.draw(item->sprite);
    }
}

void GameEngine::drawDoors() {
    for (auto const& [index, door] : doors) {
        door->draw(window);
    }
}

void GameEngine::draw() {
    window.clear(sf::Color(0xFF, 0xFF, 0xCC));
    window.draw(backgroundSprite);
    level.Draw(window);
    drawDoors();
    drawBullets();
    window.draw(player.sprite);
    drawEnemies();
    drawPickupItems();
    gameHUD.draw(window, enemies);
    window.display();
}