#ifndef BULLET_H
#define BULLET_H
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Enemy.h"
#include "Player.h"
#include "TmxLevel.h"

class Bullet {
public:
    sf::Sprite sprite;
    bool life;
    bool isExploding;
    int damage;
    float dx;
    float currentFrame;
    sf::FloatRect rect;

    enum class BulletType { player, enemy };

    BulletType type;

    Bullet(float x, float y, int dir, BulletType bulletType);

    void explosionAnimate(float time);

    void update(float time, std::vector<TmxObject>& solidObjects);
    void updatePlayer(Player& player);
    void updateEnemies(const std::list<Enemy*>& enemies);
    void updateDoors(const std::map<int, Door*>& doors);

private:
    static sf::Texture texture;
    static sf::SoundBuffer shootSoundBuffer;
    static sf::SoundBuffer hitSoundBuffer;
    sf::Sound shootSound;
    sf::Sound hitSound;

    void initResources(float x, float y);
    void handleCollision(std::vector<TmxObject>& solidObjects);
};

#endif //BULLET_H
