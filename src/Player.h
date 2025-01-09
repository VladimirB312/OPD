#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "Door.h"
#include "TmxLevel.h"

class Player {
public:
    sf::Sprite sprite;
    std::vector<TmxObject> gameObjects;
    int health;
    int countBullets;
    bool isAttack;

    enum class Direction { left, right };

    Player();

    void init(float x, float y);
    void initResources();

    Direction getDirection() const;

    sf::FloatRect getRect() const;

    void handleInput(std::list<Bullet*>& bullets, sf::Time& gameTime);

    void update(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, float time);
    void updateEnemy(Enemy& enemy, sf::Time& gameTime);


private:
    float dx, dy;
    bool onGround;
    bool isShoot;
    bool canAttack;
    bool keyActivationPressed;
    float lastHitSpikesTimer;
    float currentFrame;

    enum { stay, walk, melee_attack } state;
    Direction direction;

    sf::Time lastShootTime;
    sf::Time lastMeleeAttackTime;
    sf::Time lastHitTime;
    sf::FloatRect rect;
    sf::Texture texture;
    sf::SoundBuffer hitSoundBuffer;
    sf::SoundBuffer meleeHitSoundBuffer;
    sf::Sound meleeHitSound;
    sf::Sound hitSound;
    sf::SoundBuffer enemyHitSoundBuffer;
    sf::Sound enemyHitSound;
    sf::SoundBuffer openDoorSoundBuffer;
    sf::Sound openDoorSound;
    std::vector<TmxObject> solidObjects;

    void updatePosition(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, float time);
    void resolveCollision(const sf::FloatRect& objectRect, int dir);
    void handleDeadlyObject();
    void updateAnimate(float time);
    void meleeAttackAnimate(float time);
    void walkAnimate(float time);
    void stayAnimate();
    void shoot(std::list<Bullet*>& bullets, sf::Time& gameTime);
    void meleeAttack(sf::Time& gameTime);
    void hit(sf::Time& gameTime);
    void handleCollision(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, int dir);
    void openDoor(Door& door);
};

#endif //PLAYER_H
