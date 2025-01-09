#ifndef GAMESCREENS_H
#define GAMESCREENS_H
#include <SFML/Graphics.hpp>
#include <SFML/System/String.hpp>

enum class State;

class GameScreens {
    float menuX;
    float menuY;
    int menuStep;
    int itemsCount;
    int fontSize;
    int mainMenuSelected;
    sf::Font font;
    sf::Text* mainMenu;
    sf::Text deathScreen;
    sf::Text winScreen;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Color textColor = sf::Color::White;
    sf::Color selectedColor = sf::Color::Yellow;
    sf::Color borderColor = sf::Color::Black;

    void setInitText(sf::Text& text, sf::String str, float x, float y) const;
    sf::RenderWindow& window;
    sf::View& mainView;

public:
    GameScreens(sf::RenderWindow& window, sf::View& mainView);
    void initPauseMenu();
    void initDeathScreen();
    void initWinScreen();

    ~GameScreens() {
        delete[] mainMenu;
    }

    void drawMenuItems() const;
    void MoveUp();
    void MoveDown();
    void resetSelection();
    void centerText(sf::Text& text);
    void drawDeathScreen();
    void drawWinScreen() const;
    void openWinScreen(State& state);
    void openDeathScreen(State& state);
    void setColorTextMenu(sf::Color text_color, sf::Color selected_color, sf::Color border_color);
    void drawPauseMenu();
    void openPauseMenuScreen(State& state);
    void handleInput(State& state);
    void changeStateFromMenu(State &state);
    int getSelectedMenuNumber() const;
};

#endif //GAMESCREENS_H
