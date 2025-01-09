#ifndef ENEMY_H
#define ENEMY_H
#include <list>
#include <SFML/Graphics.hpp>

#include "Door.h"
#include "TmxLevel.h"

class Bullet;
class Player;

class Enemy {
public:
    sf::Sprite sprite;
    sf::FloatRect rect;
    bool life;
    int health;

    enum class EnemyType { melee, range };

    EnemyType type;

    Enemy(float x, float y, const std::string& enemyType);

    void update(const sf::FloatRect& playerRect, std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors,
                std::list<Bullet*>& bullets,
                float time);

private:
    static sf::Texture meleeTexture;
    static sf::Texture rangeTexture;
    float dx, dy;
    float currentFrame;
    float lastShootTimer;
    bool onGround;
    bool allowFollow;
    std::vector<TmxObject> solidObjects;

    enum { stay, walk } state;

    enum class Direction { right, left };

    Direction direction;

    struct EnemyPlayerData {
        float playerX;
        float enemyX;
        float playerY;
        float enemyY;
        float distanceToPlayer;
        bool playerIsRight;
    };

    void initResources(float x, float y);
    void initMeleeResources(float x, float y);
    void initRangeResources(float x, float y);
    void setRandomDirection();

    void updateAnimate(float time);
    void updateMeleeAnimate(float time);
    void updateRangeAnimate(float time);
    void updateRangeWalkAnimate(float time);
    void updateRangeStayAnimate();

    void setDeadSprite();

    void updateAction(const sf::FloatRect& playerRect, std::list<Bullet*>& bullets, std::map<int, Door*>& doors,
                          float time);
    void updateRangeAction(EnemyPlayerData data, std::list<Bullet*>& bullets);
    void updateMeleeAction(EnemyPlayerData data);
    void shoot(bool playerIsRight, std::list<Bullet*>& bullets);

    void updatePosition(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, float time);
    void resloveCollision(const sf::FloatRect& objectRect, int dir);

    bool checkAllowFollow(std::map<int, Door*>& doors, const sf::FloatRect& playerRect, EnemyPlayerData data) const;
    void handleCollision(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, int dir);
};


#endif //ENEMY_H
