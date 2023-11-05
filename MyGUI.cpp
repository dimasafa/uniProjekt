#include "MyGUI.h"
#include "Car.h"
#include <iostream>
#include "Auto.h"
#include "AutoK.h"
#include <vector>

MyGUI::MyGUI(std::vector<Auto>& autos) : window(sf::VideoMode(800, 1000), "SFML GUI Example"), autos(autos) {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Fehler mit Font" << std::endl;
    }

    resultText.setFont(font);
    resultText.setCharacterSize(16);
    resultText.setFillColor(sf::Color::White);
    resultText.setPosition(350, 10);


    // Erstellen die Buttons
    buttons[0] = sf::RectangleShape(sf::Vector2f(300, 50));
    buttons[0].setFillColor(sf::Color(50, 150, 250));
    buttons[0].setPosition(10, 50);

    buttons[1] = sf::RectangleShape(sf::Vector2f(300, 50));
    buttons[1].setFillColor(sf::Color(50, 150, 250));
    buttons[1].setPosition(10, 110);

    buttons[2] = sf::RectangleShape(sf::Vector2f(300, 50));
    buttons[2].setFillColor(sf::Color(50, 150, 250));
    buttons[2].setPosition(10, 170);

    buttons[3] = sf::RectangleShape(sf::Vector2f(300, 50));
    buttons[3].setFillColor(sf::Color(50, 150, 250));
    buttons[3].setPosition(10, 230);

    buttons[4] = sf::RectangleShape(sf::Vector2f(300, 50));
    buttons[4].setFillColor(sf::Color(50, 150, 250));
    buttons[4].setPosition(10, 290);


    for (int i = 0; i < 5; ++i) {
        buttonTexts[i].setFont(font);
        buttonTexts[i].setCharacterSize(20);
        buttonTexts[i].setFillColor(sf::Color::White);
        buttonTexts[i].setPosition(buttons[i].getPosition().x + 10, buttons[i].getPosition().y + 10);
    }

    buttonTexts[0].setString("Ampelkreuzung (1 Stunde)");
    buttonTexts[1].setString("Kreisverkehr (1 Stunde)");
    buttonTexts[2].setString("Ampelkreuzung (50 Stunden)");
    buttonTexts[3].setString("Kreisverkehr (50 Stunden)");
    buttonTexts[4].setString("Simulation");
}

void MyGUI::run() {
    while (window.isOpen()) {
        handleEvents();
        draw();
    }
}


// Generieren den RandomZahl
int randomZahl() {
    std::random_device rd;
    std::mt19937 gen(rd());
    int newRandomNumber = std::uniform_int_distribution<int>(1, 100)(gen);
    return newRandomNumber;
}

// Erstellung von neue Auto
void neueAutosErstellen(std::vector<Car>& cars, int vektorPosX, int vektorPosY, float startSpeedX, float startSpeedY, sf::Color farbe, int accelerationRichtung, int& StauAutoCounter, float offsetX, float offsetY) {
    sf::Vector2f position(vektorPosX, vektorPosY);
    float speed = startSpeedX;
    float speedY = startSpeedY;
    sf::Color color = farbe;
    int random = randomZahl();
    cars.emplace_back(position, speed, speedY, color, random, accelerationRichtung, offsetX, offsetY);
    StauAutoCounter++;
}

// Erstellung neue Position, nachdem der Drehpunkt erreicht wurde.
void neuePosition(Car& car, int newAccelerationRichtung, int PositionX, int PositionY, float neueSpeedX, float neueSpeedY, bool umdrehenSoll, bool umdrehenBool = false, int drehGrad = 1) {
    car.umdrehen = umdrehenBool;
    car.accelerationRichtung = newAccelerationRichtung;
    sf::Vector2f position(PositionX, PositionY);
    car.setSpeed(neueSpeedX, neueSpeedY, position, umdrehenSoll, drehGrad);
}

// Umschalter von Beschleunigung zum Bremse.
void bremseUmschltung(Car& car, int newAccelerationRichtung, int& accelerationOderBermseRichtung) {
    if (!car.reaktioansTrigger) {
        car.reaktion.restart();
        car.reaktioansTrigger = true;
    }
    car.ReaktioansZeitMessung();

    if (car.reaktionsZeitVobei >= car.reaktionsZeit) {
        car.startSpeedX = 0.0f;
        car.startSpeedY = 0.0f;
        if (!car.ClockTrigger) {
            car.startTime.restart();
            car.ClockTrigger = true;
        }

        car.accelerationRichtung = newAccelerationRichtung;
        accelerationOderBermseRichtung = 2;
    }
}

// Umschalter von Bremse zur Beschleunigung.
void beschleunigungUmschltung(Car& car, int newAccelerationRichtung, int& accelerationOderBermseRichtung) {
    if (car.ClockTrigger) {
        car.startTime.restart();
        car.accelerationY = 0.0f;
        car.accelerationRichtung = newAccelerationRichtung;
        accelerationOderBermseRichtung = 1;
    }
    car.ClockTrigger = false;
    car.reaktioansTrigger = false;

}

// Аuto markieren zum Entfernung beim Kreuzung von LöschLinien und counten der gelöschten Autos
void removeCarsMark(std::vector<Car>& cars, const std::vector<sf::RectangleShape>& Remove, int& removedCarCountLinks, int& removedCarCountRechts, int& removedCarCountOben, int& removedCarCountUnten) {
    for (auto& car : cars) {
        for (int i = 0; i < 4; i++) {
            if (car.getBounds().intersects(Remove[i].getGlobalBounds())) {
                car.istZumEntfernenMarkiert = true;
                if (i == 0) {
                    removedCarCountLinks++;
                }
                else if (i == 1) {
                    removedCarCountRechts++;
                }
                else if (i == 2) {
                    removedCarCountOben++;
                }
                else if (i == 3) {
                    removedCarCountUnten++;
                }
            }
        }
    }
}

// markierte Autos löschen
void removeCars(std::vector<Car>& cars) {
    cars.erase(std::remove_if(cars.begin(), cars.end(), [](const Car& car) {
        return car.istZumEntfernenMarkiert;
        }), cars.end());
}

void MyGUI::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (int i = 0; i < 5; ++i) {
                if (buttons[i].getGlobalBounds().contains(mousePos)) {
                    // Logik beim Click
                    if (i == 0) {
                        zaehleAutosNachRichtung(autos);
                        // Löschen die Daten aus 
                        clearHistory();
                    }
                    else if (i == 1) {
                        getKreis();
                        clearHistory();
                    }
                    else if (i == 2) {
                        clearHistory();
                        for (int j = 0; j < 50; ++j) {
                            zaehleAutosNachRichtung(autos);  // Rufen 50 Mal an
                        }
                    }
                    else if (i == 3) {
                        clearHistory();
                        for (int j = 0; j < 50; ++j) {
                            getKreis();  // Rufen 50 Mal an
                        }
                    }
                    // Starten Simulation
                    else if (i == 4) {

                        // Counter für gelöschtes Auto
                        int removedCarCountLinks = 0;
                        int removedCarCountRechts = 0;
                        int removedCarCountOben = 0;
                        int removedCarCountUnten = 0;

                        sf::RenderWindow window(sf::VideoMode(700, 638), "Straße mit Autos");

                        // background

                        sf::Texture backgroundTexture;
                        if (!backgroundTexture.loadFromFile("ampelkreuzung.jpg"))
                        {
                            std::cerr << "Fehler mit Background" << std::endl;
                        }

                        sf::Sprite backgroundSprite;
                        backgroundSprite.setTexture(backgroundTexture);

                        // Erstellen drei Kreise für die Ampel (oben)
                        sf::CircleShape redLight(6);
                        redLight.setFillColor(sf::Color::Red);
                        redLight.setPosition(280, 236);

                        sf::CircleShape yellowLight(6);
                        yellowLight.setFillColor(sf::Color::Yellow);
                        yellowLight.setPosition(280, 219);

                        sf::CircleShape greenLight(6);
                        greenLight.setFillColor(sf::Color::Green);
                        greenLight.setPosition(280, 203);

                        // Erstellen drei Kreise für die Ampel (unten)
                        sf::CircleShape redLight1(6);
                        redLight1.setFillColor(sf::Color::Red);
                        redLight1.setPosition(408, 409);

                        sf::CircleShape yellowLight1(6);
                        yellowLight1.setFillColor(sf::Color::Yellow);
                        yellowLight1.setPosition(408, 425);

                        sf::CircleShape greenLight1(6);
                        greenLight1.setFillColor(sf::Color::Green);
                        greenLight1.setPosition(408, 442);

                        // Erstellen drei Kreise für die Ampel (links)
                        sf::CircleShape redLight2(6);
                        redLight2.setFillColor(sf::Color::Red);
                        redLight2.setPosition(275, 404);

                        sf::CircleShape yellowLight2(6);
                        yellowLight2.setFillColor(sf::Color::Yellow);
                        yellowLight2.setPosition(257, 404);

                        sf::CircleShape greenLight2(6);
                        greenLight2.setFillColor(sf::Color::Green);
                        greenLight2.setPosition(239, 404);

                        // Erstellen drei Kreise für die Ampel (rechts)
                        sf::CircleShape redLight3(6);
                        redLight3.setFillColor(sf::Color::Red);
                        redLight3.setPosition(414, 241);

                        sf::CircleShape yellowLight3(6);
                        yellowLight3.setFillColor(sf::Color::Yellow);
                        yellowLight3.setPosition(431, 241);

                        sf::CircleShape greenLight3(6);
                        greenLight3.setFillColor(sf::Color::Green);
                        greenLight3.setPosition(448, 241);

                        // Erstellen die Stopplinie (von oben)
                        std::vector<sf::RectangleShape> ampelLine;
                        sf::RectangleShape aline(sf::Vector2f(40, 180));
                        aline.setFillColor(sf::Color::Green);
                        aline.setPosition(306, 80);
                        ampelLine.push_back(aline);

                        // Erstellen die Stopplinie (von unten)
                        sf::RectangleShape aline2(sf::Vector2f(40, 160));
                        aline2.setFillColor(sf::Color::Red);
                        aline2.setPosition(355, 420);
                        ampelLine.push_back(aline2);

                        // Erstellen die Stopplinie (von links)
                        sf::RectangleShape aline3(sf::Vector2f(10, 80));
                        aline3.setFillColor(sf::Color::Green);
                        aline3.setPosition(300, 310);
                        ampelLine.push_back(aline3);

                        // Erstellen die Stopplinie (von links)
                        sf::RectangleShape aline4(sf::Vector2f(210, 40));
                        aline4.setFillColor(sf::Color::White);
                        aline4.setPosition(80, 310);
                        ampelLine.push_back(aline4);

                        // Erstellen die Stopplinie (von rechts)
                        sf::RectangleShape aline5(sf::Vector2f(10, 80));
                        aline5.setFillColor(sf::Color::Black);
                        aline5.setPosition(430, 270);
                        ampelLine.push_back(aline5);

                        // Erstellen die Stopplinie (von rechts)
                        sf::RectangleShape aline6(sf::Vector2f(180, 40));
                        aline6.setFillColor(sf::Color::Black);
                        aline6.setPosition(440, 310);
                        ampelLine.push_back(aline6);

                        // Ersrellen Drehpunkte
                        // Von Oben
                        std::vector<sf::RectangleShape> vonOben;
                        sf::RectangleShape nachLinks(sf::Vector2f(5, 3));
                        nachLinks.setFillColor(sf::Color::Red);
                        nachLinks.setPosition(355, 293);
                        vonOben.push_back(nachLinks);

                        sf::RectangleShape nachRechts(sf::Vector2f(40, 3));
                        nachRechts.setFillColor(sf::Color::Red);
                        nachRechts.setPosition(310, 380);
                        vonOben.push_back(nachRechts);

                        // Von Unten
                        std::vector<sf::RectangleShape> vonUnten;
                        sf::RectangleShape nachLinks1(sf::Vector2f(50, 3));
                        nachLinks1.setFillColor(sf::Color::Blue);
                        nachLinks1.setPosition(350, 390);
                        vonUnten.push_back(nachLinks1);

                        sf::RectangleShape nachRechts1(sf::Vector2f(50, 3));
                        nachRechts1.setFillColor(sf::Color::Blue);
                        nachRechts1.setPosition(350, 285);
                        vonUnten.push_back(nachRechts1);

                        // Von Links
                        std::vector<sf::RectangleShape> vonLinks;
                        sf::RectangleShape verteiler(sf::Vector2f(1, 70));
                        verteiler.setFillColor(sf::Color::Blue);
                        verteiler.setPosition(110, 300);
                        vonLinks.push_back(verteiler);

                        sf::RectangleShape verteiler1(sf::Vector2f(3, 10));
                        verteiler1.setFillColor(sf::Color::Blue);
                        verteiler1.setPosition(230, 380);
                        vonLinks.push_back(verteiler1);

                        sf::RectangleShape nachLinks2(sf::Vector2f(3, 10));
                        nachLinks2.setFillColor(sf::Color::Black);
                        nachLinks2.setPosition(425, 320);
                        vonLinks.push_back(nachLinks2);

                        sf::RectangleShape nachRechts2(sf::Vector2f(3, 5));
                        nachRechts2.setFillColor(sf::Color::Blue);
                        nachRechts2.setPosition(345, 360);
                        vonLinks.push_back(nachRechts2);

                        // Von Rechts
                        std::vector<sf::RectangleShape> vonRechts;
                        sf::RectangleShape verteiler2(sf::Vector2f(3, 50));
                        verteiler2.setFillColor(sf::Color::Blue);
                        verteiler2.setPosition(610, 300);
                        vonRechts.push_back(verteiler2);

                        sf::RectangleShape verteiler3(sf::Vector2f(3, 10));
                        verteiler3.setFillColor(sf::Color::Blue);
                        verteiler3.setPosition(470, 265);
                        vonRechts.push_back(verteiler3);

                        sf::RectangleShape nachLinks3(sf::Vector2f(3, 5));
                        nachLinks3.setFillColor(sf::Color::Blue);
                        nachLinks3.setPosition(345, 290);
                        vonRechts.push_back(nachLinks3);

                        sf::RectangleShape nachRechts4(sf::Vector2f(3, 5));
                        nachRechts4.setFillColor(sf::Color::Red);
                        nachRechts4.setPosition(355, 340);
                        vonRechts.push_back(nachRechts4);

                        // Erstellen den LöscheLinien (Löschen die Autos, nachdem sie Kreuzung verlassen)
                        std::vector<sf::RectangleShape> Remove;
                        sf::RectangleShape RemoveLinks(sf::Vector2f(3, 650));
                        RemoveLinks.setFillColor(sf::Color::Yellow);
                        RemoveLinks.setPosition(-200, 0);
                        Remove.push_back(RemoveLinks);

                        sf::RectangleShape RemoveRechts(sf::Vector2f(3, 600));
                        RemoveRechts.setFillColor(sf::Color::Yellow);
                        RemoveRechts.setPosition(850, 0);
                        Remove.push_back(RemoveRechts);

                        sf::RectangleShape RemoveOben(sf::Vector2f(700, 3));
                        RemoveOben.setFillColor(sf::Color::Yellow);
                        RemoveOben.setPosition(0, -200);
                        Remove.push_back(RemoveOben);

                        sf::RectangleShape RemoveUnten(sf::Vector2f(700, 3));
                        RemoveUnten.setFillColor(sf::Color::Yellow);
                        RemoveUnten.setPosition(0, 850);
                        Remove.push_back(RemoveUnten);

                        // Erstellen einen Vektor, um alle Autos zu halten
                        std::vector<Car> cars;
                        std::vector<Car> oppositeCars;
                        std::vector<Car> obenCars;
                        std::vector<Car> untenCars;

                        // Zufallszahlengenerator für Autoerstellungszeiten
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_real_distribution<> distr(2000, 3000); // Zufällige Zeit zwischen 2 und 3 Sekunden
                        float random_seconds = static_cast<float>(distr(gen)) / 1000.0f; // liegen zufällige sekunden zum random_seconds

                        bool shouldCreateNewInstance = false; // ob ein neues Auto erstellen soll

                        sf::Clock clock; // Uhr für deltaTime

                        // Counter von Autos, die im Stau vor dem Ampel
                        int verticalAutoCounterOben = 0;
                        int verticalAutoCounterUnten = 0;
                        int horizontalAutoCounterLinks = 0;
                        int horizontalAutoCounterRechts = 0;

                        // Timer für Halt der Erstellung neue Autos, nach dem grüne Farbe.
                        sf::Clock resetTimer; // Reset Timer
                        sf::Time resetDelay = sf::seconds(1.0f); // Halt von 1 secunden.

                        // Aktuelle Trigger für die Function, ob Auto beschleunigen oder bremsen muss.
                        int accelerationOderBermseRechts = 1;
                        int accelerationOderBermseLinks = 1;
                        int accelerationOderBermseOben = 1;
                        int accelerationOderBermseUnten = 1;

                        sf::Clock zeit; // Zeit in Screen

                        // Font
                        sf::Font font;
                        if (!font.loadFromFile("arial.ttf")) {
                            std::cerr << "Fehler mit Font" << std::endl;;
                        }

                        sf::Text text;
                        text.setFont(font);
                        text.setCharacterSize(22);
                        text.setFillColor(sf::Color::Black);
                        text.setPosition(20.0f, 50.0f);

                        sf::Text timeText;
                        timeText.setFont(font);
                        timeText.setCharacterSize(22);
                        timeText.setFillColor(sf::Color::Black);
                        timeText.setPosition(20.0f, 20.0f);

                        while (window.isOpen()) {

                            // Process events
                            sf::Event event;
                            while (window.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    window.close();
                                }
                            }

                            float deltaTime = clock.restart().asSeconds();
                            random_seconds -= deltaTime; // Jede Bild reduzieren den random_seconds.

                            // Überprüfen , ob es Zeit ist, ein neues Auto zu erstellen. Erstellen neue random_seconds
                            if (random_seconds <= 0.0f) {
                                shouldCreateNewInstance = true;
                                // Timer mit einem neuen Zufallswert zwischen 3 und 5 Sekunden zurücksetzen
                                random_seconds = static_cast<float>(distr(gen)) / 1000.0f;
                            }

                            // Bestimmen die aktuelle aktive Farbe der Ampel
                            auto currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
                            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count() % 93; // Gesamtzyklusdauer: 50 + 3 + 40 = 93 Sekunden (50s - grun, 3s - gelb, 40s - rot)
                            sf::Color activeColor = sf::Color::Red;
                            sf::Color activeColorGegenAmpel = sf::Color::Green;

                            if (seconds < 50) {
                                activeColor = sf::Color::Green;
                                activeColorGegenAmpel = sf::Color::Red;
                            }
                            else if (seconds < 53) {
                                activeColor = sf::Color::Yellow;
                                activeColorGegenAmpel = sf::Color::Yellow;
                            }
                            else {
                                activeColor = sf::Color::Red;
                                activeColorGegenAmpel = sf::Color::Green;
                            }

                            // Stellen die graue Farbe für inaktive Farben der Ampel ein (oben)
                            redLight.setFillColor(activeColorGegenAmpel == sf::Color::Red ? activeColorGegenAmpel : sf::Color(128, 128, 128));
                            yellowLight.setFillColor(activeColorGegenAmpel == sf::Color::Yellow ? activeColorGegenAmpel : sf::Color(128, 128, 128));
                            greenLight.setFillColor(activeColorGegenAmpel == sf::Color::Green ? activeColorGegenAmpel : sf::Color(128, 128, 128));

                            // Stellen die graue Farbe für inaktive Farben der Ampel ein (unten)
                            redLight1.setFillColor(activeColorGegenAmpel == sf::Color::Red ? activeColorGegenAmpel : sf::Color(128, 128, 128));
                            yellowLight1.setFillColor(activeColorGegenAmpel == sf::Color::Yellow ? activeColorGegenAmpel : sf::Color(128, 128, 128));
                            greenLight1.setFillColor(activeColorGegenAmpel == sf::Color::Green ? activeColorGegenAmpel : sf::Color(128, 128, 128));

                            // Stellen die graue Farbe für inaktive Farben der Ampel ein (links)
                            redLight2.setFillColor(activeColor == sf::Color::Red ? activeColor : sf::Color(128, 128, 128));
                            yellowLight2.setFillColor(activeColor == sf::Color::Yellow ? activeColor : sf::Color(128, 128, 128));
                            greenLight2.setFillColor(activeColor == sf::Color::Green ? activeColor : sf::Color(128, 128, 128));

                            // Stellen die graue Farbe für inaktive Farben der Ampel ein (rechts)
                            redLight3.setFillColor(activeColor == sf::Color::Red ? activeColor : sf::Color(128, 128, 128));
                            yellowLight3.setFillColor(activeColor == sf::Color::Yellow ? activeColor : sf::Color(128, 128, 128));
                            greenLight3.setFillColor(activeColor == sf::Color::Green ? activeColor : sf::Color(128, 128, 128));


                            // Counter von Autos im Stau auf Null stellen. Benutzen den Halt von Timer.
                            if (activeColor == sf::Color::Green && horizontalAutoCounterLinks > 0) {
                                if (resetTimer.getElapsedTime() >= resetDelay) {
                                    horizontalAutoCounterLinks = 0;
                                    horizontalAutoCounterRechts = 0;
                                    resetTimer.restart(); // Reset Timer
                                }
                            }
                            if (activeColorGegenAmpel == sf::Color::Green && verticalAutoCounterOben > 0) {
                                if (resetTimer.getElapsedTime() >= resetDelay) {
                                    verticalAutoCounterOben = 0;
                                    verticalAutoCounterUnten = 0;
                                    resetTimer.restart(); // Reset Timer
                                }
                            }

                            // Neue Autos erstellen
                            int randomNumber = randomZahl();

                            if (shouldCreateNewInstance) {
                                if (horizontalAutoCounterRechts < 4 && randomNumber > 0 && randomNumber < 36) {
                                    // Erstellen ein Auto auf der rechten Straßenseite
                                    neueAutosErstellen(cars, 760, 336, -30.0f, 0.0f, sf::Color::Red, 1, horizontalAutoCounterRechts, 20.0f, 0.0f);
                                }
                                if (horizontalAutoCounterLinks < 4 && randomNumber > 35 && randomNumber < 71) {
                                    // Erstellen ein Auto auf der linken Straßenseite
                                    neueAutosErstellen(oppositeCars, -90, 320, 30.0f, 0.0f, sf::Color(255, 200, 0), 2, horizontalAutoCounterLinks, 20.0f, 0.0f);
                                }
                                if (verticalAutoCounterOben < 3 && randomNumber > 70 && randomNumber < 86) {
                                    // Erstellen ein Auto von oben
                                    neueAutosErstellen(obenCars, 335, -70, 0.0f, 30.0f, sf::Color::Blue, 3, verticalAutoCounterOben, 0.0f, 40.0f);
                                }
                                else if (verticalAutoCounterUnten < 3 && randomNumber > 86 && randomNumber < 101) {
                                    // Erstellen ein Auto von unten
                                    neueAutosErstellen(untenCars, 365, 680, 0.0f, -30.0f, sf::Color::Green, 4, verticalAutoCounterUnten, 0.0f, 40.0f);
                                }
                                shouldCreateNewInstance = false;
                            }

                            // Autos bewegen (Bedingungen) (von rechts)
                            for (auto& car : cars) {
                                for (const auto& otherCar : cars) {

                                    car.updateBeschleunigung(accelerationOderBermseRechts);

                                    // Überprüfen, ob das Auto an der Ampellinie stopen soll
                                    if (redLight3.getFillColor() == sf::Color::Red || yellowLight3.getFillColor() == sf::Color::Yellow && car.umdrehen == false && otherCar.umdrehen == false) {
                                        // Auto nähert sich der Ampellinie, halte das Auto an (für Auto nach Verteiler)
                                        if (car.getBounds().intersects(ampelLine[4].getGlobalBounds()) && car.path > 50 && car.path < 101) {
                                            car.Nullen();
                                            break;
                                        }
                                        // Auto nähert sich der Ampellinie, halte das Auto an (für Auto mit gerade Richtung)
                                        if (car.getBounds().intersects(ampelLine[5].getGlobalBounds()) && car.path > 0 && car.path < 51 && car.umdrehen == false && otherCar.umdrehen == false) {
                                            // Bremsen, wenn Geschwindigkeit > 0
                                            if (car.accelerationX <= 0) {
                                                bremseUmschltung(car, 2, accelerationOderBermseRechts);
                                            }
                                            else {
                                                car.Nullen();
                                                break;
                                            }
                                        }
                                    }

                                    if (activeColor == sf::Color::Green) {
                                        beschleunigungUmschltung(car, 1, accelerationOderBermseRechts);
                                    }


                                    if (&car != &otherCar) { // Selbstkollisionsprüfung überspringen
                                        // Autos kollidieren, halten das Auto an
                                        if ((activeColor == sf::Color::Red || activeColor == sf::Color::Yellow) && car.getBounds().intersects(otherCar.getBounds()) && car.umdrehen == false && otherCar.umdrehen == false) {
                                            car.Nullen();
                                            break;
                                        }
                                    }

                                    // Erste Verteiler (rechts)
                                    if (car.path > 50 && car.path < 101) {
                                        if (car.getBounds().intersects(vonRechts[0].getGlobalBounds())) {
                                            car.Nullen();
                                            neuePosition(car, 1, 595, 300, -60.0f, -25.0f, false, false, 25);
                                        }
                                    }

                                    // Zweite Verteiler (rechts)
                                    if (car.path > 50 && car.path < 101) {
                                        if (car.getBounds().intersects(vonRechts[1].getGlobalBounds())) {
                                            neuePosition(car, 1, 500, 294, -40.0f, 0.0f, true);
                                        }
                                    }

                                    // Zusätzliche Überprüfung, das Auto Richtige Platz besitzen nach zweiter verteiler
                                    if (car.path > 50 && car.path < 101 && car.getBounds().intersects(ampelLine[4].getGlobalBounds()) && activeColor == sf::Color::Green && car.rotationWinkel == 180) {
                                        car.Nullen();
                                        neuePosition(car, 1, 440, 294, -40.0f, 0.0f, true);
                                    }

                                    // Nach Oben(nord) oder nach links(west)
                                    if (car.path > 50 && car.path < 101) {
                                        //Nach Oben(nord)
                                        if (car.getBounds().intersects(vonRechts[2].getGlobalBounds()) && car.path > 50 && car.path < 61) {
                                            neuePosition(car, 4, 385, 310, 0.0f, -40.0f, true);
                                        }
                                        // nach links(west) default
                                    }

                                    // Nach Unten(süd) oder nach rechts(ost)
                                    if (car.path > 0 && car.path < 51) {
                                        //Nach Unten(süd)
                                        if (car.getBounds().intersects(vonRechts[3].getGlobalBounds())) {
                                            if (car.path > 0 && car.path < 11) {
                                                neuePosition(car, 3, 315, 300, 0.0f, 40.0f, true);
                                            }
                                            // nach rechts(ost)
                                            if (car.path > 10 && car.path < 51) {
                                                neuePosition(car, 2, 410, 362, 40.0f, 0.0f, true, true);
                                            }
                                        }
                                    }
                                }
                            }


                            // Gleich für Gegenzug (von links) Von West
                            for (auto& car : oppositeCars) {
                                for (const auto& otherCar : oppositeCars) {

                                    car.updateBeschleunigung(accelerationOderBermseLinks);

                                    // Überprüfen, ob das Auto an der Ampellinie stopen soll
                                    if (redLight2.getFillColor() == sf::Color::Red || yellowLight2.getFillColor() == sf::Color::Yellow && car.umdrehen == false && otherCar.umdrehen == false) {
                                        // Auto nähert sich der Ampellinie, halte das Auto an (für Auto nach Verteiler)
                                        if (car.getBounds().intersects(ampelLine[2].getGlobalBounds()) && car.path > 0 && car.path < 51) {
                                            car.Nullen();
                                            break;
                                        }
                                        // Auto nähert sich der Ampellinie, halte das Auto an (für Auto mit gerade Richtung)
                                        if (car.getBounds().intersects(ampelLine[3].getGlobalBounds()) && car.path > 50 && car.path < 101 && car.umdrehen == false && otherCar.umdrehen == false) {
                                            // Bremsen, wenn Geschwindigkeit > 0
                                            if (car.accelerationX >= 0) {
                                                bremseUmschltung(car, 1, accelerationOderBermseLinks);
                                            }
                                            else {
                                                car.Nullen();
                                                break;
                                            }
                                        }
                                    }

                                    if (&car != &otherCar) { // Selbstkollisionsprüfung überspringen
                                        // Autos kollidieren, halten das Auto an
                                        if ((activeColor == sf::Color::Red || activeColor == sf::Color::Yellow) && car.getBounds().intersects(otherCar.getBounds()) && car.umdrehen == false && otherCar.umdrehen == false) {
                                            car.Nullen();
                                            break;
                                        }
                                    }

                                    if (activeColor == sf::Color::Green) {
                                        beschleunigungUmschltung(car, 2, accelerationOderBermseLinks);
                                    }

                                    // Erste Verteiler

                                    if (car.path > 0 && car.path < 51) {
                                        if (car.getBounds().intersects(vonLinks[0].getGlobalBounds())) {
                                            car.Nullen();
                                            neuePosition(car, 2, 190, 320, 60.0f, 25.0f, false, false, 25);
                                        }
                                    }

                                    // Zweite Verteiler
                                    if (car.path > 0 && car.path < 51) {
                                        if (car.getBounds().intersects(vonLinks[1].getGlobalBounds())) {
                                            car.Nullen();
                                            neuePosition(car, 2, 300, 362, 40.0f, 0.0f, true);
                                        }
                                    }

                                    // Zusätzliche Überprüfung, das Auto Richtige Platz besitzen nach zweiter verteiler
                                    if (car.path > 0 && car.path < 51 && car.getBounds().intersects(ampelLine[2].getGlobalBounds()) && activeColor == sf::Color::Green && car.rotationWinkel != 90) {
                                        car.Nullen();
                                        neuePosition(car, 2, 300, 362, 40.0f, 0.0f, true);
                                    }


                                    // Nach Oben(nord) oder nach links(west)
                                    if (car.path > 50 && car.path < 101) {
                                        //Nach Oben(nord)
                                        if (car.getBounds().intersects(vonLinks[2].getGlobalBounds()) && car.path > 50 && car.path < 61) {
                                            neuePosition(car, 4, 385, 350, 0.0f, -40.0f, true);
                                        }
                                        // nach links(west)
                                        if (car.getBounds().intersects(vonLinks[2].getGlobalBounds()) && car.path > 60 && car.path < 101) {
                                            neuePosition(car, 1, 370, 293, -40.0f, 0.0f, true, true);
                                        }
                                    }

                                    // Nach Unten(süd) oder nach rechts(ost)
                                    if (car.path > 0 && car.path < 51) {
                                        //Nach Unten(süd)
                                        if (car.getBounds().intersects(vonLinks[3].getGlobalBounds()) && car.path > 0 && car.path < 11) {
                                            neuePosition(car, 3, 315, 370, 0.0f, 40.0f, true);
                                        }
                                        // nach rechts(ost) default
                                    }

                                }

                            }

                            // Gleich für oben Car
                            for (auto& car : obenCars) {
                                for (auto& otherCar : obenCars) {

                                    car.updateBeschleunigung(accelerationOderBermseOben);

                                    // Überprüfen, ob das Auto an der Ampellinie stopen soll
                                    if (redLight.getFillColor() == sf::Color::Red || yellowLight.getFillColor() == sf::Color::Yellow && car.umdrehen == false) {
                                        // Auto nähert sich der Ampellinie, halte das Auto an
                                        if (car.getBounds().intersects(ampelLine[0].getGlobalBounds())) {
                                            // Bremsen, wenn Geschwindigkeit > 0
                                            if (car.accelerationY >= 0) {
                                                // Addieren Reaktioanszeit
                                                bremseUmschltung(car, 4, accelerationOderBermseOben);
                                            }
                                            else {
                                                car.Nullen();
                                                break;
                                            }
                                        }
                                    }
                                    if (activeColorGegenAmpel == sf::Color::Green) {
                                        beschleunigungUmschltung(car, 3, accelerationOderBermseOben);
                                    }

                                    // Nach Links (west)
                                    if (car.path > 50 && car.path < 91) {
                                        if (car.getBounds().intersects(vonOben[0].getGlobalBounds())) {
                                            neuePosition(car, 1, 340, 293, -40.0f, 0.0f, true);
                                        }
                                    }
                                    // Nach Rechts (ost)
                                    if (car.path > 10 && car.path < 51) {
                                        if (car.getBounds().intersects(vonOben[1].getGlobalBounds())) {
                                            neuePosition(car, 2, 400, 362, 40.0f, 0.0f, true);
                                        }
                                    }
                                    // Nach Oben (nord)
                                    if (car.path > 0 && car.path < 11) {
                                        if (car.getBounds().intersects(vonOben[0].getGlobalBounds())) {
                                            neuePosition(car, 4, 385, 300, 0.0f, -40.0f, true, true);
                                        }
                                    }
                                    // Nach Unten (süd) als default


                                    if (&car != &otherCar) { // Selbstkollisionsprüfung überspringen
                                        // Autos kollidieren, halten das Auto an (wenn beide car die umdrehen in false)
                                        if ((activeColorGegenAmpel == sf::Color::Red || activeColorGegenAmpel == sf::Color::Yellow) && car.getBounds().intersects(otherCar.getBounds()) && car.umdrehen == false && otherCar.umdrehen == false) {
                                            // nullen von Beschleunigung
                                            car.Nullen();
                                        }

                                    }
                                }

                            }

                            // Gleich für unten Car
                            for (auto& car : untenCars) {
                                for (const auto& otherCar : untenCars) {

                                    car.updateBeschleunigung(accelerationOderBermseUnten);

                                    // Überprüfen, ob das Auto an der Ampellinie stopen soll
                                    if (redLight1.getFillColor() == sf::Color::Red || yellowLight1.getFillColor() == sf::Color::Yellow && car.umdrehen == false) {
                                        // Auto nähert sich der Ampellinie, halte das Auto an
                                        if (car.getBounds().intersects(ampelLine[1].getGlobalBounds())) {
                                            // Bremsen, wenn Geschwindigkeit > 0
                                            if (car.accelerationY <= 0) {
                                                // Addieren Reaktioanszeit
                                                bremseUmschltung(car, 3, accelerationOderBermseUnten);
                                            }
                                            else {
                                                car.Nullen();
                                                break;
                                            }
                                        }
                                    }

                                    if (activeColorGegenAmpel == sf::Color::Green) {
                                        beschleunigungUmschltung(car, 4, accelerationOderBermseUnten);
                                    }

                                    // Nach Links (west)
                                    if (car.path > 0 && car.path < 41) {
                                        if (car.getBounds().intersects(vonUnten[1].getGlobalBounds())) {
                                            neuePosition(car, 1, 390, 293, -40.0f, 0.0f, true);
                                        }
                                    }

                                    // Nach Rechts (ost)
                                    if (car.path > 40 && car.path < 81) {
                                        if (car.getBounds().intersects(vonUnten[0].getGlobalBounds())) {
                                            neuePosition(car, 2, 450, 362, 40.0f, 0.0f, true);
                                        }
                                    }

                                    // Nach Unten (süd)
                                    if (car.path > 80 && car.path < 91) {
                                        if (car.getBounds().intersects(vonUnten[0].getGlobalBounds())) {
                                            neuePosition(car, 3, 315, 395, 0.0f, 40.0f, true, true);
                                        }
                                    }

                                    // Nach Oben (nord) als default

                                    if (&car != &otherCar) { // Selbstkollisionsprüfung überspringen
                                        // Autos kollidieren, halten das Auto an
                                        if ((activeColorGegenAmpel == sf::Color::Red || activeColorGegenAmpel == sf::Color::Yellow) && car.getBounds().intersects(otherCar.getBounds()) && car.umdrehen == false && otherCar.umdrehen == false) {
                                            car.Nullen();
                                        }

                                    }
                                }

                            }

                            // Zusammenstoßen unterschidliche Typen von Auto
                            for (auto& car : cars) {
                                for (auto& otherCar : oppositeCars) {
                                    // Stoßen von car und otherCar
                                    if (car.getBounds().intersects(otherCar.getBounds()) && activeColor != sf::Color::Red) {
                                        car.Nullen();
                                    }
                                }

                                for (auto& otherCar : obenCars) {
                                    // Stoßen von car und otherCar
                                    if (car.getBounds().intersects(otherCar.getBounds())) {
                                        car.Nullen();
                                    }
                                }

                                for (auto& otherCar : untenCars) {
                                    // Stoßen von car und otherCar
                                    if (car.getBounds().intersects(otherCar.getBounds())) {
                                        car.Nullen();
                                    }
                                }
                            }

                            for (auto& car : oppositeCars) {

                                for (auto& otherCar : obenCars) {
                                    // Stoßen von car und otherCar
                                    if (car.getBounds().intersects(otherCar.getBounds())) {
                                        car.Nullen();
                                    }
                                }

                                for (auto& otherCar : untenCars) {
                                    // Stoßen von car und otherCar
                                    if (car.getBounds().intersects(otherCar.getBounds())) {
                                        car.Nullen();
                                    }
                                }
                            }

                            for (auto& car : obenCars) {

                                for (auto& otherCar : untenCars) {
                                    // Stoßen von car und otherCar
                                    if (car.getBounds().intersects(otherCar.getBounds()) && activeColorGegenAmpel != sf::Color::Red) {
                                        car.Nullen();
                                    }
                                }
                            }

                            // Remove die Autos und count die Menge.

                            removeCarsMark(cars, Remove, removedCarCountLinks, removedCarCountRechts, removedCarCountOben, removedCarCountUnten);
                            removeCarsMark(oppositeCars, Remove, removedCarCountLinks, removedCarCountRechts, removedCarCountOben, removedCarCountUnten);
                            removeCarsMark(obenCars, Remove, removedCarCountLinks, removedCarCountRechts, removedCarCountOben, removedCarCountUnten);
                            removeCarsMark(untenCars, Remove, removedCarCountLinks, removedCarCountRechts, removedCarCountOben, removedCarCountUnten);

                            removeCars(cars);
                            removeCars(oppositeCars);
                            removeCars(obenCars);
                            removeCars(untenCars);

                            // Text auf Screen einstellen
                            // Text für Gesamtzahl der Auto
                            text.setString("Gesamtzahl der Autos: " + std::to_string(removedCarCountLinks + removedCarCountRechts + removedCarCountOben + removedCarCountUnten));

                            // Text für Zeit
                            float zeitVorbei = zeit.getElapsedTime().asSeconds();

                            // Umwandeln zeitVorbei zum Stunden, Minuten und Sekunden
                            int stunden = static_cast<int>(zeitVorbei) / 3600;
                            int minuten = static_cast<int>(zeitVorbei) / 60;
                            int sekunden = static_cast<int>(zeitVorbei) % 60;

                            // Erstellen gesamte String von Zeit
                            std::string zeitString = "Zeit: " + std::to_string(stunden) + ":" + (minuten < 10 ? "0" : "") + std::to_string(minuten) + ":" + (sekunden < 10 ? "0" : "") + std::to_string(sekunden);

                            // Geben zeitString zum timeText ab, die schon auf dem Screen darstellen kann.
                            timeText.setString(zeitString);


                            // Bewegung von Autos gemäß deltaTime
                            for (auto& car : cars) {
                                car.move(deltaTime, car.accelerationRichtung);
                            }

                            for (auto& car : oppositeCars) {
                                car.move(deltaTime, car.accelerationRichtung);
                            }

                            for (auto& car : obenCars) {
                                car.move(deltaTime, car.accelerationRichtung);
                            }

                            for (auto& car : untenCars) {
                                car.move(deltaTime, car.accelerationRichtung);
                            }

                            // Draw alles
                            window.clear();
                            window.draw(backgroundSprite);

                            for (auto& aline : Remove) {
                                window.draw(aline);
                            }
                            for (auto& car : cars) {
                                car.draw(window);
                            }
                            for (auto& car : oppositeCars) {
                                car.draw(window);
                            }
                            for (auto& car : obenCars) {
                                car.draw(window);
                            }
                            for (auto& car : untenCars) {
                                car.draw(window);
                            }

                            // Draw Ampeln
                            window.draw(redLight);
                            window.draw(yellowLight);
                            window.draw(greenLight);

                            window.draw(redLight1);
                            window.draw(yellowLight1);
                            window.draw(greenLight1);

                            window.draw(redLight2);
                            window.draw(yellowLight2);
                            window.draw(greenLight2);

                            window.draw(redLight3);
                            window.draw(yellowLight3);
                            window.draw(greenLight3);

                            // Draw Texte
                            window.draw(text);
                            window.draw(timeText);

                            window.display();
                        }
                    }
                }
            }
        }
    }
}

void MyGUI::draw() {
    window.clear(sf::Color(30, 30, 30));
    for (int i = 0; i < 5; ++i) {
        window.draw(buttons[i]);
        window.draw(buttonTexts[i]);
    }

    std::string resultString; // Ergebniss nach Screen

    // Ergebniss für erste 2 Buttons
    if (functionResultAmpel != -1 && resultHistory.size() < 1) {  // Prüfen, ob Ergebniss existieren und es ist nicht button 3 und 4
        resultString += "Ergebniss von Ampelkreuzung: " + std::to_string(functionResultAmpel) + "\n";
    }

    if (functionResultKreis != -1 && resultHistory.size() < 1) {  // Prüfen, ob Ergebniss existieren und es ist nicht button 3 und 4
        resultString += "Ergebniss von Kreisverkehr: " + std::to_string(functionResultKreis) + "\n";
    }

    // Darstellung für button 3 und 4 (50 mal)
    if (resultHistory.size() > 1) {
        for (int i = 0; i < resultHistory.size(); ++i) {
            resultString += "Ergebniss " + std::to_string(i + 1) + ": " + std::to_string(resultHistory[i]) + "\n";
        }
        // Mittelwert berechnen
        int sum = 0;
        for (int i : resultHistory) {
            sum += i;
        }
        int mittelWert = resultHistory.empty() ? 0 : sum / resultHistory.size();

        resultString += "Mittelwert ist: " + std::to_string(mittelWert);
    }

    resultText.setString(resultString);

    window.draw(resultText);
    window.display();
}