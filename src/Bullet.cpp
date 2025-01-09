#include "Bullet.h"
#include <iostream>

#include "Enemy.h"
#include "Player.h"

sf::Texture Bullet::texture;
sf::SoundBuffer Bullet::shootSoundBuffer;
sf::SoundBuffer Bullet::hitSoundBuffer;

Bullet::Bullet(float x, float y, int dir, BulletType bulletType)
{
    initResources(x, y);

    shootSound.play();

    dx = dir * 300;
    life = true;
    isExploding = false;
    currentFrame = 0;

    if (bulletType == BulletType::player)
    {
        damage = 34;
    } else
    {
        damage = 10;
    }

    type = bulletType;
}

void Bullet::initResources(float x, float y)
{
    if (texture.getSize().x == 0)
    {
        texture.loadFromFile("resources/sprites/bullet.png");
    }

    if (shootSoundBuffer.getDuration() == sf::Time::Zero)
    {
        shootSoundBuffer.loadFromFile("resources/sounds/shoot_sound.wav");
    }
    shootSound.setBuffer(shootSoundBuffer);

    if (hitSoundBuffer.getDuration() == sf::Time::Zero)
    {
        hitSoundBuffer.loadFromFile("resources/sounds/bullet_hit.wav");
    }
    hitSound.setBuffer(hitSoundBuffer);

    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(620, 375, 20, 15));
    rect = sf::FloatRect(x, y, 20, 15);
}

void Bullet::explosionAnimate(float time)
{
    currentFrame += 12 * time;
    if (currentFrame > 3)
    {
        isExploding = true;
        return;
    }

    if (dx > 0)
    {
        sprite.setPosition(rect.left, rect.top - 10);
        sprite.setTextureRect(sf::IntRect(682 + 30 * int(currentFrame), 339, 28, 28));
    } else
    {
        sprite.setPosition(rect.left - 10, rect.top - 10);
        sprite.setTextureRect(sf::IntRect(682 + 28 + 30 * int(currentFrame), 339, -28, 28));
    }
}

void Bullet::update(float time, std::vector<TmxObject> &solidObjects)
{
    if (!life)
    {
        explosionAnimate(time);
        return;
    }
    rect.left += dx * time;
    sprite.setPosition(rect.left, rect.top);
    handleCollision(solidObjects);
}

void Bullet::updatePlayer(Player &player)
{
    if (!life || type != BulletType::enemy)
    {
        return;
    }

    if (rect.intersects(player.getRect()))
    {
        hitSound.play();
        player.health -= damage;
        life = false;
    }
}

void Bullet::updateEnemies(const std::list<Enemy *> &enemies)
{
    if (!life || type != BulletType::player)
    {
        return;
    }


    for (const auto &enemy: enemies)
    {
        if (!enemy->life || !rect.intersects(enemy->rect))
        {
            continue;
        }
        hitSound.play();
        life = false;
        enemy->health -= damage;
        break;
    }
}

void Bullet::updateDoors(const std::map<int, Door *> &doors)
{
    if (!life)
    {
        return;
    }

    for (auto &[index, door]: doors)
    {
        if (rect.intersects(door->doorRect))
        {
            life = false;
        }
    }
}

void Bullet::handleCollision(std::vector<TmxObject> &solidObjects)
{
    for (auto &solidObject: solidObjects)
    {
        if (!rect.intersects(solidObject.rect))
        {
            continue;
        }
        life = false;
        return;
    }
}
