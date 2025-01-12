#include "GameHUD.h"

#include "Player.h"

GameHUD::GameHUD() {
    healthTexture.loadFromFile("resources/sprites/healthbar.png");
    healthSprite.setTexture(healthTexture);
    healthSprite.setTextureRect(sf::IntRect(0, 43, 297, 76));
    healthSprite.setScale(0.5f, 0.5f);
    healthBar.setFillColor(sf::Color(0, 0, 0));

    bulletTexture.loadFromFile("resources/sprites/bulletHUD.png");
    bulletSprite.setTexture(bulletTexture);
    bulletSprite.setTextureRect(sf::IntRect(0, 0, 77, 77));
    bulletSprite.setScale(0.5f, 0.5f);
    font.loadFromFile("resources/fonts/Badlychipped66.ttf");

    initText();
}

void GameHUD::initText() {
    countBullets.setFont(font);
    countBullets.setCharacterSize(20);
    countBullets.setString("0");
    countBullets.setFillColor(sf::Color(0, 0, 0));
}

void GameHUD::updateHealth(float x, float y, const int health) {
    healthSprite.setPosition(x, y);

    const float segWidth = 2.75;
    healthBar.setSize(sf::Vector2f(segWidth * (100 - health), 50));
    healthBar.setScale(0.5f, 0.5f);
    healthBar.setPosition(x + health * segWidth * 0.5 + 6.5, y + 14 * 0.5);
}

void GameHUD::updateCountBullets(float x, float y, const int count_bullets) {
    bulletSprite.setPosition(x, y);
    countBullets.setString(std::to_string(count_bullets));
    countBullets.setPosition(x + 35, y + 10);
}

void GameHUD::update(const sf::Vector2f& center, const sf::Vector2f& size, const Player& player) {
    const float x = center.x - size.x / 2 + shiftX;
    const float y = center.y - size.y / 2 + shiftY;
    updateHealth(x, y, player.health);
    updateCountBullets(x + 170, y, player.countBullets);
}

void GameHUD::drawEnemyHP(sf::RenderWindow& window, Enemy& enemy) {
    sf::RectangleShape enemyBar;
    enemyBar.setFillColor(sf::Color(0, 0, 0));
    enemyBar.setSize(sf::Vector2f(enemy.rect.width, 5));
    enemyBar.setPosition(enemy.rect.left, enemy.rect.top - 10);

    sf::RectangleShape enemyHealthBar;
    enemyHealthBar.setFillColor(sf::Color(255, 0, 0));
    enemyHealthBar.setSize(sf::Vector2f(enemy.rect.width * (static_cast<float>(enemy.health) / 100), 5));
    enemyHealthBar.setPosition(enemy.rect.left, enemy.rect.top - 10);

    window.draw(enemyBar);
    window.draw(enemyHealthBar);
}

void GameHUD::drawEnemiesHP(sf::RenderWindow& window, const std::list<Enemy*>& enemies) {
    for (auto const& enemy : enemies) {
        if (enemy->life) {
            drawEnemyHP(window, *enemy);
        }
    }
}

void GameHUD::draw(sf::RenderWindow& window, const std::list<Enemy*>& enemies) {
    window.draw(healthSprite);
    window.draw(healthBar);
    window.draw(bulletSprite);
    window.draw(countBullets);
    drawEnemiesHP(window, enemies);
}