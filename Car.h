#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>

const float PI = 3.14159265358979323846;

class Car {
public:
    float rotationWinkel;
    bool umdrehen;
    int path;
    float beschleunigungsSteigerungProSekunde = 10.0f;  // BeschleunigungsZahl

    float accelerationX;
    float accelerationY;
    float startSpeedX;
    float startSpeedY;

    sf::Clock startTime; // Eigene Uhr für jede Auto
    float elapsedTime;

    int accelerationRichtung;
    bool ClockTrigger; // ób startTime für Beschleunigung oder Bremsen benutzt.

    sf::Clock timer;
    float interval = 1.0f; // Zahlen 1 Sekunde für Beschleunigung und Bremsen

    float reaktionsZeit;
    float reaktionsZeitVobei;
    bool reaktioansTrigger;
    sf::Clock reaktion;

    // Änderung von zusätzliche Grenzen
    float offsetx;
    float offsety;

    // ob auto schon gelöscht hat
    bool istZumEntfernenMarkiert;

    Car(sf::Vector2f position, float speedX, float speedY, sf::Color color, int random = 1, int richtung = 1, float offsetX = 0.0f, float offsetY = 0.0f) {
        startSpeedX = speedX;
        startSpeedY = speedY;

        accelerationX = 0.0f;
        accelerationY = 0.0f;

        accelerationRichtung = richtung;

        offsetx = offsetX;// Abstand zwischen Autos (wenn sie stoppen) Achse x
        offsety = offsetY;// Abstand zwischen Autos (wenn sie stoppen) Achse y

        car.setSize(sf::Vector2f(45, 20));
        car.setFillColor(color);
        car.setPosition(position);
        velocity.x = speedX + accelerationX;
        velocity.y = speedY + accelerationY;

        umdrehen = false;

        // Erstellen die StartZeit
        elapsedTime = 0.0f;

        // randomZahl, um Richtige Weg zu wahlen
        path = random;

        rotationWinkel = std::atan2(speedY, speedX) * 180.0f / PI;
        car.setRotation(rotationWinkel);

        // Erstellen eigene Uhr für Auto
        startTime = sf::Clock();
        startTime.restart();

        // Trigger, für Beschleunigung oder Bremesen
        ClockTrigger = false;

        // ReaktionsZeit
        if (random > 0 && random < 21) {
            reaktionsZeit = 0.75f;
        }
        else if (random > 20 && random < 91) {
            reaktionsZeit = 1.00f;
        }
        else {
            reaktionsZeit = 2.00f;
        }
        // Erstellen ReaktioansTimer
        reaktion = sf::Clock();
        reaktion.restart();

        // Auto noch nicht removed
        istZumEntfernenMarkiert = false;
    }

    // Zahlen aktuelle ReaktioansZeit
    void ReaktioansZeitMessung() {
        reaktionsZeitVobei = reaktion.getElapsedTime().asSeconds();
    }

    // Funktion von ReaktioansZahl
    void ReaktionsErmitteln(int richtung) {
        if (!reaktioansTrigger) {
            reaktion.restart();
            reaktioansTrigger;
        }
        ReaktioansZeitMessung();

        if (reaktionsZeitVobei >= reaktionsZeit) {
            startSpeedX = 0.0f;
            startSpeedY = 0.0f;
            if (!ClockTrigger) {
                startTime.restart();
                ClockTrigger = true;
            }
            accelerationRichtung = richtung;
        }
    }

    // Update Geschwindikeit
    void updateBeschleunigung(int accelerationOderBermse) {
        velocity.x = startSpeedX + accelerationX * accelerationOderBermse;
        velocity.y = startSpeedY + accelerationY * accelerationOderBermse;
    }

    // Update die Zeit, wenn die Beschleunigung stattgefunden
    void updateElapsedTime() {
        elapsedTime = startTime.getElapsedTime().asSeconds();
    }

    // Function von Bewegung des Autos
    void move(float deltaTime, int richtung = 0) {
        updateElapsedTime();
        updateAcceleration(elapsedTime, richtung);

        sf::Vector2f deltaPosition = velocity * deltaTime;
        car.move(deltaPosition);
    }

    // In welche Richtung wirkt die Beschleunigung aus.
    void updateAcceleration(float newTime, int richtung) {
        // gesamte Beschleunigung
        float gesamtAcceleration = beschleunigungsSteigerungProSekunde * newTime;

        // 4 wege von Beschleunigung
        float elapsedSeconds = timer.getElapsedTime().asSeconds();
        if (elapsedSeconds >= interval && richtung == 1) {
            accelerationX -= gesamtAcceleration;
            timer.restart();
        }

        if (elapsedSeconds >= interval && richtung == 2) {
            accelerationX += gesamtAcceleration;
            timer.restart();
        }

        if (elapsedSeconds >= interval && richtung == 3) {
            accelerationY += gesamtAcceleration;
            timer.restart();
        }

        if (elapsedSeconds >= interval && richtung == 4) {
            accelerationY -= gesamtAcceleration;
            timer.restart();
        }
    }

    sf::Vector2f getPosition() const {
        return car.getPosition();
    }

    void setBound(float offsetX, float offsetY) {
        offsetx = offsetX;
        offsety = offsetY;
    }

    // Abmessungen des Autos
    sf::FloatRect getBounds() const {
        sf::Vector2f position = car.getPosition();
        sf::Vector2f size = car.getSize();
        return sf::FloatRect(position.x, position.y, size.x + offsetx, size.y + offsety);
    }

    // Erneuen die Bewegungsdaten
    void setSpeed(float newSpeedx, float newSpeedY, sf::Vector2f position, bool drehen, int drehGrad = 1) {
        accelerationX = 0.0f;
        accelerationY = 0.0f;
        startTime.restart();
        startSpeedX = newSpeedx;
        startSpeedY = newSpeedY;

        car.setPosition(position);
        if (drehen) {
            car.setSize(sf::Vector2f(20, 45));
        }
        float aktuelleGeschwindigkeit = velocity.x + velocity.y;

        // Speichern die Geschwindigkeit beim Drehen.
        if (accelerationRichtung == 1) {
            velocity.x = newSpeedx + accelerationX;
            velocity.y = 0.0f;
        }
        if (accelerationRichtung == 2) {
            velocity.x = -newSpeedx + accelerationX;
            velocity.y = 0.0f;
        }
        if (accelerationRichtung == 3) {
            velocity.x = 0.0f;
            velocity.y = newSpeedx + accelerationY;
        }
        if (accelerationRichtung == 4) {
            velocity.x = 0.0f;
            velocity.y = -newSpeedx + accelerationY;
        }

        // Drehen nach Verteiler
        if (drehGrad == 1) {
            rotationWinkel = std::atan2(newSpeedx, newSpeedY) * 180.0f / PI;
            car.setRotation(rotationWinkel);
            car.move(velocity);
        }
        // Manuell den DrehGrad zu ermitteln.
        else {
            car.setRotation(drehGrad);
            car.move(velocity);
        }
    }

    // Geschwindigkeit zu ernullen.
    void Nullen() {
        accelerationX = 0.0f;
        accelerationY = 0.0f;
        startTime.restart();
        startSpeedX = 0.0f;
        startSpeedY = 0.0f;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(car);
    }
private:
    sf::RectangleShape car;
    sf::Vector2f velocity; // aktuelle Geschwindigkeit
};