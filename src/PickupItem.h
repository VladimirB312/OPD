#ifndef PICKUPITEM_H
#define PICKUPITEM_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"

class PickupItem {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::FloatRect rect;
    bool pickedUp;
    bool life;
    enum class Item { healthPack, ammoPack };

    Item itemType;

    PickupItem(float x, float y, Item itemType);

    void updatePlayer(Player& player, float deltaTime);

private:
    void initSound();
    static sf::SoundBuffer pickSoundBuffer;
    sf::Sound pickSound;
    float pickSoundDuration;
    float pickSoundCurrent;
};


#endif //PICKUPITEM_H
