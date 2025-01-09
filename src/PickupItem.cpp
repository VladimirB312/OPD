#include "PickupItem.h"
#include "Player.h"

sf::SoundBuffer PickupItem::pickSoundBuffer;

PickupItem::PickupItem(float x, float y, Item item_Type)
{
    if (item_Type == Item::ammoPack)
    {
        texture.loadFromFile("resources/sprites/ammo.png");
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 52, 24));
        rect = sf::FloatRect(x, y, 52, 24);
    }
    if (item_Type == Item::healthPack)
    {
        texture.loadFromFile("resources/sprites/health_pack.png");
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 41, 32));
        rect = sf::FloatRect(x, y, 41, 32);
    }
    sprite.setPosition(x, y);
    itemType = item_Type;
    pickedUp = false;
    life = true;
    initSound();
}

void PickupItem::initSound() {
    if (pickSoundBuffer.getDuration() == sf::Time::Zero)
    {
        pickSoundBuffer.loadFromFile("resources/sounds/pick_item_sound.wav");
    }
    pickSound.setBuffer(pickSoundBuffer);
    pickSoundDuration = pickSoundBuffer.getDuration().asSeconds();
    pickSoundCurrent = 0;
}

void PickupItem::updatePlayer(Player &player, const float deltaTime)
{
    if (pickedUp) {
        pickSoundCurrent += deltaTime;
        if (pickSoundCurrent < pickSoundDuration) {
            return;
        }
        life = false;
        return;
    }

    if (!rect.intersects(player.getRect()))
    {
        return;
    }

    pickSound.play();

    if (itemType == Item::healthPack && player.health < 100)
    {
        pickedUp = true;
        player.health += 20;
        if (player.health > 100) player.health = 100;
        return;
    }

    if (itemType == Item::ammoPack)
    {
        pickedUp = true;
        player.countBullets += 10;
    }
}
