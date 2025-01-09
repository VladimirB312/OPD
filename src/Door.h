#ifndef DOOR_H
#define DOOR_H
#include <SFML/Graphics.hpp>


class Door {
public:
    sf::Sprite doorSprite;
    sf::Sprite keySprite;
    sf::FloatRect doorRect;
    sf::FloatRect keyRect;
    bool isOpen;

    Door();
    void setDoorPosition(float x, float y);
    void setKeyPosition(float x, float y);
    void update();
    void draw(sf::RenderWindow& window) const;

private:
    static sf::Texture doorTexture;
    static sf::Texture keyTexture;
};


#endif //DOOR_H
