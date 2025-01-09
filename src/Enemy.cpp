#include "Enemy.h"
#include "Player.h"
#include <cmath>

constexpr float GRAVITY = 1200;
constexpr float MELEE_FOLLOW_DISTANCE_MAX = 200;
constexpr float MELEE_FOLLOW_DISTANCE_MIN = 30;
constexpr float RANGE_FOLLOW_DISTANCE_MAX = 400;
constexpr float RANGE_FOLLOW_DISTANCE_MIN = 250;
constexpr int SHOOT_INTERVAL = 1;

sf::Texture Enemy::meleeTexture;
sf::Texture Enemy::rangeTexture;

Enemy::Enemy(float x, float y, const std::string& enemyType) {
    currentFrame = 0;
    onGround = false;
    life = true;
    allowFollow = true;
    health = 100;
    state = walk;
    if (enemyType == "melee") {
        type = EnemyType::melee;
    }
    else {
        type = EnemyType::range;
    }

    initResources(x, y);
    setRandomDirection();
}

void Enemy::initResources(float x, float y) {
    if (type == EnemyType::melee) {
        initMeleeResources(x, y);
    }
    else {
        initRangeResources(x, y);
    }

    rect.left = x;
    rect.top = y;
}

void Enemy::initMeleeResources(float x, float y) {
    if (meleeTexture.getSize().x == 0) {
        meleeTexture.loadFromFile("resources/sprites/enemy1.png");
    }
    sprite.setTexture(meleeTexture);
    rect = sf::FloatRect(120, 120, 40, 60);
}

void Enemy::initRangeResources(float x, float y) {
    if (rangeTexture.getSize().x == 0) {
        rangeTexture.loadFromFile("resources/sprites/rangeEnemy.png");
    }
    sprite.setTexture(rangeTexture);
    rect = sf::FloatRect(2, 116, 40, 40);
}

void Enemy::setRandomDirection() {
    direction = static_cast<Direction>(rand() % 2);
    if (direction == Direction::left) {
        dx = -100;
    }
    else {
        dx = 100;
    }
    dy = 100;
}

void Enemy::update(const sf::FloatRect& playerRect, std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors,
                   std::list<Bullet*>& bullets,
                   const float time) {
    if (!life || health < 1) {
        life = false;
        setDeadSprite();
        return;
    }

    updatePosition(solidObjects, doors, time);
    updateAction(playerRect, bullets, doors, time);
    updateAnimate(time);
}

void Enemy::updateAction(const sf::FloatRect& playerRect, std::list<Bullet*>& bullets, std::map<int, Door*>& doors,
                         float time) {
    EnemyPlayerData data;

    data.playerX = playerRect.left + rect.width / 2;
    data.enemyX = rect.left + rect.width / 2;
    data.playerY = playerRect.top + rect.height / 2;
    data.enemyY = rect.top + rect.height / 2;
    data.distanceToPlayer = std::hypot(data.playerX - data.enemyX, data.playerY - data.enemyY);
    data.playerIsRight = data.playerX > data.enemyX;

    state = walk;

    if (!checkAllowFollow(doors, playerRect, data)) {
        return;
    }

    if (type == EnemyType::melee) {
        updateMeleeAction(data);
    }
    else {
        lastShootTimer += time;
        updateRangeAction(data, bullets);
    }
}

void Enemy::updateRangeAction(EnemyPlayerData data, std::list<Bullet*>& bullets) {
    if (abs(data.playerY - data.enemyY) > 50 || abs(data.playerX - data.enemyX) < 10 || data.distanceToPlayer >
        RANGE_FOLLOW_DISTANCE_MAX) {
        return;
    }

    if (data.distanceToPlayer > RANGE_FOLLOW_DISTANCE_MIN) {
        if (data.playerIsRight && dx < 0) {
            dx *= -1;
            direction = Direction::right;
        }

        if (!data.playerIsRight && dx > 0) {
            dx *= -1;
            direction = Direction::left;
        }
        return;
    }

    direction = data.playerIsRight ? Direction::right : Direction::left;

    state = stay;

    shoot(data.playerIsRight, bullets);
}

void Enemy::shoot(bool playerIsRight, std::list<Bullet*>& bullets) {
    if (lastShootTimer < SHOOT_INTERVAL) {
        return;
    }

    lastShootTimer = 0;

    if (playerIsRight) {
        bullets.push_back(new Bullet(rect.left + rect.width / 2, rect.top + 6, 1,
                                     Bullet::BulletType::enemy));
    }

    if (!playerIsRight) {
        bullets.push_back(new Bullet(rect.left + rect.width / 2, rect.top + 6, -1,
                                     Bullet::BulletType::enemy));
    }
}

void Enemy::updateMeleeAction(EnemyPlayerData data) {
    if (data.distanceToPlayer < MELEE_FOLLOW_DISTANCE_MIN || data.distanceToPlayer > MELEE_FOLLOW_DISTANCE_MAX) {
        return;
    }

    if (abs(data.playerY - data.enemyY) < 100 && abs(data.playerX - data.enemyX) > 10) {
        if (data.playerIsRight && dx < 0) {
            dx *= -1;
        }

        if (!data.playerIsRight && dx > 0) {
            dx *= -1;
        }
    }
}

bool Enemy::checkAllowFollow(std::map<int, Door*>& doors, const sf::FloatRect& playerRect, EnemyPlayerData data) const {
    sf::FloatRect rectBetweenPlayerRight = sf::FloatRect(rect.left, rect.top, data.distanceToPlayer,
                                                         rect.height);
    sf::FloatRect rectBetweenPlayerLeft = sf::FloatRect(playerRect.left, rect.top, data.distanceToPlayer,
                                                        rect.height);

    for (const auto& [index, door] : doors) {
        if (data.playerIsRight && rectBetweenPlayerRight.intersects(door->doorRect)) {
            return false;
        }
        if (!data.playerIsRight && rectBetweenPlayerLeft.intersects(door->doorRect)) {
            return false;
        }
    }
    return true;
}


void Enemy::updatePosition(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, float time) {
    if (state == walk) {
        rect.left += dx * time;
    }

    handleCollision(solidObjects, doors, 0);

    if (!onGround) dy = dy + GRAVITY * time;
    rect.top += dy * time;
    onGround = false;
    handleCollision(solidObjects, doors, 1);
}

void Enemy::resloveCollision(const sf::FloatRect& objectRect, int dir) {
    if (dir == 0 && dx > 0) {
        rect.left = objectRect.left - rect.width;
        dx *= -1;
    }
    else if (dir == 0 && dx < 0) {
        rect.left = objectRect.left + objectRect.width;
        dx *= -1;
    }

    if (dir == 1 && dy > 0) {
        rect.top = objectRect.top - rect.height;
        dy = 0;
        onGround = true;
    }
}


void Enemy::handleCollision(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, int dir) {
    for (auto& solidObject : solidObjects) {
        if (!rect.intersects(solidObject.rect)) {
            continue;
        }
        resloveCollision(solidObject.rect, dir);
    }

    for (const auto& [index, door] : doors) {
        if (!rect.intersects(door->doorRect)) {
            continue;
        }
        resloveCollision(door->doorRect, dir);
    }
}

void Enemy::setDeadSprite() {
    if (type == EnemyType::melee) {
        sprite.setTextureRect(sf::IntRect(80 * 4, 0, 80, 120));
        sprite.setScale(0.5f, 0.5f);
        sprite.setPosition(rect.left, rect.top);
    }
    else {
        sprite.setTextureRect(sf::IntRect(215, 328, 48, 40));
        sprite.setPosition(rect.left, rect.top);
    }
}

void Enemy::updateAnimate(float time) {
    if (dx > 0 && state != stay) {
        direction = Direction::right;
    }

    if (dx < 0 && state != stay) {
        direction = Direction::left;
    }

    if (type == EnemyType::melee) {
        updateMeleeAnimate(time);
    }
    else {
        updateRangeAnimate(time);
    }

    sprite.setPosition(rect.left, rect.top);
}

void Enemy::updateMeleeAnimate(float time) {
    currentFrame += 6 * time;
    if (currentFrame > 4) currentFrame -= 4;

    if (direction == Direction::right) {
        sprite.setTextureRect(sf::IntRect(80 * int(currentFrame), 0, 80, 120));
    }
    if (direction == Direction::left) {
        sprite.setTextureRect(sf::IntRect(80 * int(currentFrame) + 80, 0, -80, 120));
    }
    sprite.setScale(0.5f, 0.5f);
}

void Enemy::updateRangeAnimate(float time) {
    if (state == stay) {
        updateRangeStayAnimate();
    }
    else {
        updateRangeWalkAnimate(time);
    }
}

void Enemy::updateRangeWalkAnimate(float time) {
    currentFrame += 6 * time;
    if (currentFrame > 4) currentFrame -= 4;


    if (direction == Direction::right) {
        sprite.setTextureRect(sf::IntRect(43 * int(currentFrame) + 45, 116, -43, 40));
    }
    if (direction == Direction::left) {
        sprite.setTextureRect(sf::IntRect(2 + 43 * int(currentFrame), 116, 43, 40));
    }
}

void Enemy::updateRangeStayAnimate() {
    if (direction == Direction::right) {
        sprite.setTextureRect(sf::IntRect(123, 7, -40, 40));
    }
    if (direction == Direction::left) {
        sprite.setTextureRect(sf::IntRect(83, 7, 40, 40));
    }
}
