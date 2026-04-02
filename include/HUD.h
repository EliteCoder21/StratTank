#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class PlayerTank;
class AllyTank;

class HUD {
public:
    HUD();

    void update(int wave, int score, int enemiesRemaining);
    void render(sf::RenderWindow& window);

    void setPlayerHealth(int health, int maxHealth);
    void setSelectedAlly(int allyId);
    void showCommandIndicator(sf::Vector2f from, sf::Vector2f to);
    sf::Font& getFont() { return font; }

private:
    sf::Font font;
    sf::Text waveText;
    sf::Text scoreText;
    sf::Text enemiesText;
    sf::Text playerHealthText;
    sf::Text allySelectionText;
    sf::RectangleShape healthBar;
    sf::RectangleShape healthBarBg;
    sf::VertexArray commandLine;

    int currentWave = 1;
    int currentScore = 0;
    int enemiesLeft = 0;
    int selectedAlly = -1;
    bool showCommand = false;
    bool fontLoaded = false;
};
