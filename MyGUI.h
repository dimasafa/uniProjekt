#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Auto.h"

class MyGUI {
public:
    MyGUI(std::vector<Auto>& autos);

    void run();

    // Nötige Funktionen aus main in Gui
    void zaehleAutosNachRichtung(std::vector<Auto>& autos);
    void getKreis();
    void clearHistory();

    // Ergebniss für 1 Mal
    int functionResultAmpel;
    int functionResultKreis;

    // Ergebnisse für 50 Mal
    std::vector<int> resultHistory;

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text resultText;
    sf::RectangleShape buttons[5];
    sf::Text buttonTexts[5];
    std::vector<Auto>& autos;

    void handleEvents();
    void draw();
};