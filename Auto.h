#pragma once
#include <iostream>

class Auto {
public:

    int id;
    double breite;
    double laenge;
    double beschleunigung;
    double bremsbeschleunigung;
    double reaktionszeit;
    double geschwindigkeit;
    double positionX;
    double positionY;
    char zielrichtung;
    int anfahrtsrichtung;


    Auto(int id, double breite, double laenge, double beschleunigung, double bremsbeschleunigung,   //Definition aller Variablen die später im Bezug auf ein generiertes Auto von Interesse werden
        double reaktionszeit, double positionX, double positionY, char zielrichtung, int anfahrtsrichtung)
        : id(id), breite(breite), laenge(laenge), beschleunigung(beschleunigung),
        bremsbeschleunigung(bremsbeschleunigung), reaktionszeit(reaktionszeit),
        positionX(positionX), positionY(positionY), zielrichtung(zielrichtung),
        anfahrtsrichtung(anfahrtsrichtung) {
        geschwindigkeit = 0.0;
    }

    void beschleunigen() { //void Funktion ist zuständig für das beschleunigen der Autos
        geschwindigkeit += beschleunigung;
        if (geschwindigkeit > 40.0) { //festlegen einer maximalen Geschwindigkeit im Kreuzungsbereich 
            geschwindigkeit = 40.0;
        }
    }

    void bremsen() { //void Funktion ist zuständig für das bremsen der Autos 
        geschwindigkeit -= bremsbeschleunigung;
        if (geschwindigkeit < 0.0) { //Die Geschwindigkeit darf niemals negativ werden daher hier der Ausschluss
            geschwindigkeit = 0.0;
        }
    }

    // Funktion zur Simulation der Reaktionszeit
    void starteNachReaktionszeit() {
        // Verzögert den Start des Autos entsprechend der Reaktionszeit
        double zeitverzoegerung = reaktionszeit * beschleunigung;
        positionY -= zeitverzoegerung;
    }

    char getZielrichtung() const {
        return zielrichtung;
    }

    bool istVerlassen() const {
        switch (zielrichtung) {
        case 'N':
            if (positionY < -30.0) {
                //cout << "Auto " << id << " hat die Kreuzung nach Norden verlassen.\n";
                return true;
            }
            break;
        case 'O':
            if (positionX > 30.0) {
                //cout << "Auto " << id << " hat die Kreuzung nach Osten verlassen.\n";
                return true;
            }
            break;
        case 'S':
            if (positionY > 30.0) {
                //cout << "Auto " << id << " hat die Kreuzung nach Süden verlassen.\n";
                return true;
            }
            break;
        case 'W':
            if (positionX < -30.0) {
                //cout << "Auto " << id << " hat die Kreuzung nach Westen verlassen.\n";
                return true;
            }
            break;
        default:
            return false;
        }

        return false;
    }


private:

};