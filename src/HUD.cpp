#include "HUD.h"
#include <string>

HUD::HUD() : waveText(font, "", 20), scoreText(font, "", 18), enemiesText(font, "", 16),
             playerHealthText(font, "", 16), allySelectionText(font, "", 14),
             commandLine(sf::PrimitiveType::Lines, 2) {
    fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) {
        fontLoaded = font.openFromFile("build/arial.ttf");
    }
    if (!fontLoaded) {
        fontLoaded = font.openFromFile("arial.ttf");
    }
    
    waveText.setFillColor(sf::Color::White);
    waveText.setPosition({20, 20});
    
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({20, 50});
    
    enemiesText.setFillColor(sf::Color(200, 200, 200));
    enemiesText.setPosition({20, 80});
    
    playerHealthText.setFillColor(sf::Color::White);
    playerHealthText.setPosition({20, 680});
    
    allySelectionText.setFillColor(sf::Color(150, 255, 150));
    allySelectionText.setPosition({20, 650});
    
    healthBar.setFillColor(sf::Color(0, 200, 0));
    healthBar.setPosition({20, 700});
    
    healthBarBg.setFillColor(sf::Color(50, 50, 50));
    healthBarBg.setPosition({20, 700});
}

void HUD::update(int wave, int score, int enemiesRemaining) {
    currentWave = wave;
    currentScore = score;
    enemiesLeft = enemiesRemaining;
    
    waveText.setString("Wave: " + std::to_string(currentWave));
    scoreText.setString("Score: " + std::to_string(currentScore));
    enemiesText.setString("Enemies: " + std::to_string(enemiesLeft));
}

void HUD::render(sf::RenderWindow& window) {
    window.draw(waveText);
    window.draw(scoreText);
    window.draw(enemiesText);
    window.draw(playerHealthText);
    window.draw(allySelectionText);
    
    window.draw(healthBarBg);
    window.draw(healthBar);
    
    if (showCommand) {
        window.draw(commandLine);
    }
}

void HUD::setPlayerHealth(int health, int maxHealth) {
    playerHealthText.setString("HP: " + std::to_string(health) + "/" + std::to_string(maxHealth));
    
    healthBarBg.setSize({200, 15});
    healthBar.setSize({200 * static_cast<float>(health) / maxHealth, 15});
}

void HUD::setSelectedAlly(int allyId) {
    selectedAlly = allyId;
    if (allyId >= 0) {
        allySelectionText.setString("Selected: Ally " + std::to_string(allyId + 1));
    } else {
        allySelectionText.setString("Selected: Player");
    }
}

void HUD::showCommandIndicator(sf::Vector2f from, sf::Vector2f to) {
    commandLine[0].position = from;
    commandLine[1].position = to;
    commandLine[0].color = sf::Color(100, 255, 100);
    commandLine[1].color = sf::Color(100, 255, 100);
    showCommand = true;
}
