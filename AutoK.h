#pragma once
class AutoK {  //erstellen einer Klasse, welche jedes Auto mit seinen spezifischen Variablen definiert
public:
    int id; //zuweisen einer Einzigartigen ID für jedes Auto um das Debugging zu erleichtern 
    double breite; //definieren der Breite für jedes Auto um spätere Ausgabe in der Konsole vollständig zu machen 
    double laenge;  //gleiches Vorgehen für die Länge wie für die Breite 
    double beschleunigung; //ebenfalls bei der Beschleunigung und der Bremsbeschleunigung wurde dies in der Klasse jedem Auto der Übersichtlichkeit wegen zugeordnet
    double bremsbeschleunigung;
    double reaktionszeit; //Zuweisen der Reaktionszeit anhand des vorliegenden Fahrertyps, siehe spätere Klasse zur Reaktionszeit
    double geschwindigkeit; //Definition einer Anfangsgeschwindigkeit (wird später=0 gesetzt)
    double positionX;  //Definition der Position eines jeden Autos anhand einer X- und einer Y-Koordinate
    double positionY;
    int zielrichtung;  // Übersetzung der Zielrichtung in Zahlen um den Umgang zu erleichtern 
    int anfahrtsrichtung;

    AutoK(int id, double breite, double laenge, double beschleunigung, double bremsbeschleunigung, //Definition der Autos
        double reaktionszeit, double positionX, double positionY, int zielrichtung, int anfahrtsrichtung)
        : id(id), breite(breite), laenge(laenge), beschleunigung(beschleunigung),
        bremsbeschleunigung(bremsbeschleunigung), reaktionszeit(reaktionszeit),
        positionX(positionX), positionY(positionY), zielrichtung(zielrichtung),
        anfahrtsrichtung(anfahrtsrichtung) {
        geschwindigkeit = 0.0;
    }

    void beschleunigen() { //void Funktion ist zuständig für das beschleunigen der Autos
        geschwindigkeit += beschleunigung;
    }

    void bremsen() { //void Funktion ist zuständig für das beschleunigen der Autos
        geschwindigkeit -= bremsbeschleunigung;
        if (geschwindigkeit < 0.0) { //if Bedingung um das Bremsen auf eine negative Geschwindigkeit zu vermeiden 
            geschwindigkeit = 0.0;
        }
    }

};