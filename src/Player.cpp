#include "Player.h"
#include <iostream>
#include <list>
#include "Bullet.h"
#include "Door.h"

constexpr float GRAVITY = 1200;

Player::Player() {
    initResources();
    rect = sf::FloatRect(100, 100, 36, 39);
    dx = dy = 0;
    currentFrame = 0;
    health = 100;
    onGround = false;
    isShoot = false;
    isAttack = false;
    direction = Direction::right;
    state = walk;
}

void Player::init(float x, float y) {
    rect.left = x;
    rect.top = y;
    countBullets = 15;
    lastHitSpikesTimer = 0;
}

void Player::initResources() {
    texture.loadFromFile("resources/sprites/doomguy.png");
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(23, 2200, 50, 39));
    sprite.setOrigin(7, 0);

    hitSoundBuffer.loadFromFile("resources/sounds/deadly_hit.wav");
    hitSound.setBuffer(hitSoundBuffer);

    meleeHitSoundBuffer.loadFromFile("resources/sounds/melee_hit.wav");
    meleeHitSound.setBuffer(meleeHitSoundBuffer);

    enemyHitSoundBuffer.loadFromFile("resources/sounds/enemy_hit.wav");
    enemyHitSound.setBuffer(enemyHitSoundBuffer);

    openDoorSoundBuffer.loadFromFile("resources/sounds/open_door_sound.wav");
    openDoorSound.setBuffer(openDoorSoundBuffer);
}

Player::Direction Player::getDirection() const {
    return direction;
}

void Player::handleInput(std::list<Bullet*>& bullets, sf::Time& gameTime) {
    keyActivationPressed = false;
    state = state != melee_attack ? stay : melee_attack;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        dx = -150;
        direction = Direction::left;
        state = state != melee_attack ? walk : melee_attack;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        dx = 150;
        direction = Direction::right;
        state = state != melee_attack ? walk : melee_attack;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && onGround) {
        dy = -600;
        onGround = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        shoot(bullets, gameTime);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && state != melee_attack) {
        meleeAttack(gameTime);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        keyActivationPressed = true;
    }
}

void Player::meleeAttack(sf::Time& gameTime) {
    if (gameTime.asMilliseconds() - lastMeleeAttackTime.asMilliseconds() < 500) {
        return;
    }
    lastMeleeAttackTime = gameTime;
    currentFrame = 0;
    canAttack = true;
    state = melee_attack;
}

void Player::shoot(std::list<Bullet*>& bullets, sf::Time& gameTime) {
    if (state == melee_attack || countBullets < 1) {
        return;
    }

    if (gameTime.asMilliseconds() - lastShootTime.asMilliseconds() < 500) {
        return;
    }

    countBullets--;

    lastShootTime = gameTime;

    const float bulletX = rect.left + rect.width / 2;
    const float bulletY = rect.top + 10;
    if (direction == Direction::right) {
        bullets.push_back(new Bullet(bulletX, bulletY, 1, Bullet::BulletType::player));
    }
    else {
        bullets.push_back(new Bullet(bulletX, bulletY, -1, Bullet::BulletType::player));
    }
}

void Player::updateEnemy(Enemy& enemy, sf::Time& gameTime) {
    if (!enemy.rect.intersects(rect)) {
        return;
    }

    if (enemy.type == Enemy::EnemyType::melee) {
        hit(gameTime);
    }

    if (isAttack) {
        meleeHitSound.play();
        enemy.health -= 50;
        isAttack = false;
    }
}

void Player::hit(sf::Time& gameTime) {
    if (gameTime.asMilliseconds() - lastHitTime.asMilliseconds() > 500 && health > 0) {
        enemyHitSound.play();
        lastHitTime = gameTime;
        health -= 10;
        if (health < 0) {
            health = 0;
        }
    }
}

void Player::update(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, float time) {
    lastHitSpikesTimer += time;
    updatePosition(solidObjects, doors, time);
    updateAnimate(time);
    sprite.setPosition(rect.left, rect.top);
    dx = 0;
}

void Player::updateAnimate(float time) {
    if (state == melee_attack) {
        meleeAttackAnimate(time);
        return;
    }
    if (state == walk) {
        walkAnimate(time);
        return;
    }
    if (state == stay) {
        stayAnimate();
        return;
    }
}

void Player::meleeAttackAnimate(float time) {
    currentFrame += 6 * time;
    if (int(currentFrame) > 1 && canAttack) {
        canAttack = false;
        isAttack = true;
    }
    if (currentFrame > 4) {
        isAttack = false;
        currentFrame = 0;
        state = walk;
        return;
    }

    if (direction == Direction::right) {
        sprite.setTextureRect(sf::IntRect(50 * int(currentFrame) + 22, 650, 50, 39));
    }
    if (direction == Direction::left) {
        sprite.setTextureRect(sf::IntRect(50 * int(currentFrame) + 22 + 50, 650, -50, 39));
    }
}

void Player::walkAnimate(float time) {
    currentFrame += 6 * time;
    if (currentFrame > 6) currentFrame -= 6;

    if (dx > 0) sprite.setTextureRect(sf::IntRect(50 * int(currentFrame) + 22, 2200, 50, 39));
    if (dx < 0) sprite.setTextureRect(sf::IntRect(50 * int(currentFrame) + 22 + 50, 2200, -50, 39));
}

void Player::stayAnimate() {
    currentFrame = 0;
    if (direction == Direction::right) sprite.setTextureRect(sf::IntRect(22, 2153, 50, 39));
    if (direction == Direction::left) sprite.setTextureRect(sf::IntRect(22 + 50, 2153, -50, 39));
}

void Player::updatePosition(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, float time) {
    rect.left += dx * time;
    handleCollision(solidObjects, doors, 0);

    if (!onGround) dy = dy + GRAVITY * time;
    rect.top += dy * time;
    onGround = false;
    handleCollision(solidObjects, doors, 1);
}

void Player::resolveCollision(const sf::FloatRect& objectRect, int dir) {
    if (dir == 0 && dx > 0) rect.left = objectRect.left - rect.width;
    if (dir == 0 && dx < 0) rect.left = objectRect.left + objectRect.width;

    if (dir == 1 && dy > 0) {
        rect.top = objectRect.top - rect.height;
        dy = 0;
        onGround = true;
    }
    if (dir == 1 && dy < 0) {
        rect.top = objectRect.top + objectRect.height;
        dy = 0;
    }
}

void Player::handleDeadlyObject() {
    if (lastHitSpikesTimer < 1) {
        return;
    }

    lastHitSpikesTimer = 0;
    health -= 15;
    rect.top -= 7;
    hitSound.play();
}

void Player::handleCollision(std::vector<TmxObject>& solidObjects, std::map<int, Door*>& doors, int dir) {
    for (auto& solidObject : solidObjects) {
        if (!rect.intersects(solidObject.rect)) {
            continue;
        }
        resolveCollision(solidObject.rect, dir);
        if (solidObject.type == "deadly") {
            handleDeadlyObject();
        }
    }

    for (auto const & [index, door]: doors) {
        if (!door->isOpen && rect.intersects(door->keyRect)) {
            openDoor(*door);
            continue;
        }

        if (!rect.intersects(door->doorRect)) {
            continue;
        }

        resolveCollision(door->doorRect, dir);
    }
}

void Player::openDoor(Door &door) {
    if (keyActivationPressed) {
        door.isOpen = true;
        openDoorSound.play();
    }
}

sf::FloatRect Player::getRect() const {
    return rect;
}
