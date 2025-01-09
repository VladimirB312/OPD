#ifndef PLAYERHUD_H
#define PLAYERHUD_H
#include <SFML/Graphics.hpp>

#include "Player.h"


class GameHUD {
public:
    GameHUD();

    void update(const sf::Vector2f& center, const sf::Vector2f& size, const Player& player);

    void draw(sf::RenderWindow& window, const std::list<Enemy*>& enemies);

private:
    const float shiftX = 20;
    const float shiftY = 20;
    sf::Sprite healthSprite;
    sf::Texture healthTexture;
    sf::FloatRect healthRect;
    sf::RectangleShape healthBar;

    sf::Text countBullets;
    sf::Sprite bulletSprite;
    sf::Texture bulletTexture;

    sf::Font font;

    void initText();

    void updateHealth(float x, float y, int health);

    void updateCountBullets(float x, float y, int count_bullets);

    void drawEnemyHP(sf::RenderWindow& window, Enemy& enemy);

    void drawEnemiesHP(sf::RenderWindow& window, const std::list<Enemy*>& enemies);
};


#endif //PLAYERHUD_H
