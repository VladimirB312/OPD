#include "GameScreens.h"
#include <SFML/Graphics/Text.hpp>

#include "GameEngine.h"

GameScreens::GameScreens(sf::RenderWindow& window, sf::View& mainView)
    : window(window), mainView(mainView) {
    if (!font.loadFromFile("resources/fonts/Badlychipped66.ttf")) {
        exit(1);
    };

    backgroundTexture.loadFromFile("resources/sprites/background.jpg");
    backgroundSprite.setTexture(backgroundTexture);

    initPauseMenu();
    initWinScreen();
    initDeathScreen();
}

void GameScreens::initPauseMenu() {
    sf::String names_menu[]{"Play", "Restart", "Exit"};
    fontSize = 60;
    menuStep = 80;
    menuX = 370;
    menuY = 100;
    itemsCount = 3;
    mainMenu = new sf::Text[itemsCount];

    for (int i = 0, ypos = menuY; i < itemsCount; i++, ypos += menuStep)
        setInitText(mainMenu[i], names_menu[i], menuX, ypos);

    mainMenuSelected = 0;
    mainMenu[mainMenuSelected].setFillColor(selectedColor);
    setColorTextMenu(sf::Color(237, 147, 0), sf::Color::Red, sf::Color::Black);
}

void GameScreens::initDeathScreen() {
    setInitText(deathScreen, "YOU DIE", 150, 150);
    deathScreen.setFillColor(sf::Color(237, 0, 0));
    deathScreen.setCharacterSize(200);
}

void GameScreens::initWinScreen() {
    setInitText(winScreen, "YOU WIN !", 150, 150);
    winScreen.setFillColor(sf::Color(20, 147, 0));
    winScreen.setCharacterSize(200);
}

void GameScreens::setInitText(sf::Text& text, sf::String str, float x, float y) const {
    text.setFont(font);
    text.setFillColor(textColor);
    text.setString(str);
    text.setCharacterSize(fontSize);
    text.setPosition(x, y);
    text.setOutlineThickness(3);
    text.setOutlineColor(borderColor);
}

void GameScreens::setColorTextMenu(sf::Color text_color, sf::Color selected_color, sf::Color border_color) {
    textColor = text_color;
    selectedColor = selected_color;
    borderColor = border_color;

    for (int i = 0; i < itemsCount; i++) {
        mainMenu[i].setFillColor(textColor);
        mainMenu[i].setOutlineColor(borderColor);
    }

    mainMenu[mainMenuSelected].setFillColor(selectedColor);
}

int GameScreens::getSelectedMenuNumber() const {
    return mainMenuSelected;
}

void GameScreens::MoveUp() {
    mainMenuSelected--;
    if (mainMenuSelected >= 0) {
        mainMenu[mainMenuSelected].setFillColor(selectedColor);
        mainMenu[mainMenuSelected + 1].setFillColor(textColor);
    }
    else {
        mainMenu[0].setFillColor(textColor);
        mainMenuSelected = itemsCount - 1;
        mainMenu[mainMenuSelected].setFillColor(selectedColor);
    }
}

void GameScreens::MoveDown() {
    mainMenuSelected++;

    if (mainMenuSelected < itemsCount) {
        mainMenu[mainMenuSelected - 1].setFillColor(textColor);
        mainMenu[mainMenuSelected].setFillColor(selectedColor);
    }
    else {
        mainMenu[itemsCount - 1].setFillColor(textColor);
        mainMenuSelected = 0;
        mainMenu[mainMenuSelected].setFillColor(selectedColor);
    }
}

void GameScreens::resetSelection() {
    mainMenuSelected = 0;
    for (int i = 0; i < itemsCount; i++) {
        mainMenu[i].setFillColor(textColor);
    }
    mainMenu[mainMenuSelected].setFillColor(selectedColor);
}

void GameScreens::centerText(sf::Text& text) {
    sf::FloatRect textBounds = text.getLocalBounds();
    float x = (window.getSize().x - textBounds.width) / 2;
    float y = (window.getSize().y - textBounds.height) / 2;
    text.setPosition(x, y);
}

void GameScreens::drawDeathScreen() {
    window.clear(sf::Color::Blue);
    window.draw(backgroundSprite);
    window.draw(deathScreen);
    window.display();
}

void GameScreens::openDeathScreen(State& state) {
    mainView.setCenter({1920 / 2, 1080 / 2});
    window.setView(mainView);
    centerText(deathScreen);
    sf::Clock clock;

    while (window.isOpen() && state == State::game_over) {
        drawDeathScreen();
        float screenTime = clock.getElapsedTime().asSeconds();
        if (screenTime > 3) {
            state = State::paused;
        }
    }
}

void GameScreens::drawWinScreen() const {
    window.clear(sf::Color::Blue);
    window.draw(backgroundSprite);
    window.draw(winScreen);
    window.display();
}

void GameScreens::openWinScreen(State& state) {
    mainView.setCenter({1920 / 2, 1080 / 2});
    window.setView(mainView);
    centerText(winScreen);
    sf::Clock clock;

    while (window.isOpen() && state == State::win) {
        drawWinScreen();
        float screenTime = clock.getElapsedTime().asSeconds();
        if (screenTime > 3) {
            state = State::paused;
        }
    }
}

void GameScreens::drawMenuItems() const {
    for (int i = 0; i < itemsCount; i++) window.draw(mainMenu[i]);
}

void GameScreens::drawPauseMenu() {
    window.clear(sf::Color::Blue);
    window.draw(backgroundSprite);
    drawMenuItems();
    window.display();
}

void GameScreens::openPauseMenuScreen(State& state) {
    mainView.setCenter({WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2});
    window.setView(mainView);
    sf::Music music;
    music.openFromFile("resources/sounds/8_bit_ice_cave_lofi.ogg");
    music.play();
    while (window.isOpen() && state == State::paused) {
        handleInput(state);
        drawPauseMenu();
    }
    music.stop();
}

void GameScreens::handleInput(State& state) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type != sf::Event::KeyReleased) {
            continue;
        }
        if (event.key.code == sf::Keyboard::Up) {
            MoveUp();
        }
        if (event.key.code == sf::Keyboard::Down) {
            MoveDown();
        }
        if (event.key.code == sf::Keyboard::Return) {
            changeStateFromMenu(state);
        }
    }
}

void GameScreens::changeStateFromMenu(State& state) {
    switch (getSelectedMenuNumber()) {
    case 0:
        state = State::playing;
        resetSelection();
        return;
    case 1:
        state = State::reset;
        resetSelection();
        break;
    case 2:
        exit(0);
    default: break;
    }
}
