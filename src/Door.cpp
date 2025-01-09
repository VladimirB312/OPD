#include "Door.h"

#include "Player.h"

sf::Texture Door::doorTexture;
sf::Texture Door::keyTexture;

Door::Door() {
    if (doorTexture.getSize().x == 0) {
        doorTexture.loadFromFile("resources/sprites/door.png");
    }
    if (keyTexture.getSize().x == 0) {
        keyTexture.loadFromFile("resources/sprites/key.png");
    }

    doorSprite.setTexture(doorTexture);
    keySprite.setTexture(keyTexture);
    keySprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    doorRect = sf::FloatRect(0,0, 16, 116);
    keyRect = sf::FloatRect(0, 0, 32, 32);
    isOpen = false;
}

void Door::setDoorPosition(float x, float y) {
    doorRect.left = x;
    doorRect.top = y;
    doorSprite.setPosition(x, y);
}

void Door::setKeyPosition(float x, float y) {
    keyRect.left = x;
    keyRect.top = y;
    keySprite.setPosition(x, y);
}

void Door::update() {
    if (isOpen) {
        keySprite.setTextureRect(sf::IntRect(0, 32, 32, 32));
        doorSprite.setTextureRect(sf::IntRect(0,0, 16, 16));
        doorRect.height = 16;
    }
}

void Door::draw(sf::RenderWindow &window) const {
    window.draw(keySprite);
    window.draw(doorSprite);
}