#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <SFML/Graphics.hpp>
#include "MyGUI.h"
#include "Auto.h"
#include "AutoK.h"
#include "reaktionszeitGenerator.h"
#include "reaktionszeit.h"

using namespace std;

// Globale Konstanten
const int gruenzeit_ampel_ostwest = 30;
const int gelbzeit_ampel = 5;
const int rotzeit_ampel_ostwest = 25;
const int gruenzeit_ampel_nordsued = 20;
const int rotzeit_ampel_nordsued = 30;

// Definition eines allgemeinen Koordinatensystems für die Kreuzung (x als horizontale Variable und y als vertikale)
struct Koordinaten {
    double x;
    double y;
};

// Funktion zur Benutzereingabe der Anzahl der zu generierenden Autos
int eingabeAnzahlAutos() {
    int anzahlAutos = 500;
    return anzahlAutos;
}

// Funktion zur Simulation der Ampelphasen
void simuliereAmpelphasen(int stunden) {

    int anzahlEintraege = 0;  // Für die Ausgabe der Anzahl der Einträge

    for (int i = 0; i < stunden * 3600; ++i) {
        int zeit_innerhalb_phase_ostwest = i % (gruenzeit_ampel_ostwest + gelbzeit_ampel + rotzeit_ampel_ostwest);
        int zeit_innerhalb_phase_nordsued = i % (gruenzeit_ampel_nordsued + rotzeit_ampel_nordsued);

        if (zeit_innerhalb_phase_ostwest < gruenzeit_ampel_ostwest) {
            //std::cout << "Ost-West-Achse: Grün\n";
            ++anzahlEintraege;
        }
        else if (zeit_innerhalb_phase_ostwest < gruenzeit_ampel_ostwest + gelbzeit_ampel) {
            //std::cout << "Ost-West-Achse: Gelb\n";
            ++anzahlEintraege;
        }
        else {
            //std::cout << "Ost-West-Achse: Rot\n";
            ++anzahlEintraege;
        }

        if (zeit_innerhalb_phase_nordsued < gruenzeit_ampel_nordsued) {
            //std::cout << "Nord-Süd-Achse: Grün\n";
            ++anzahlEintraege;
        }
        else if (zeit_innerhalb_phase_nordsued < gruenzeit_ampel_nordsued + gelbzeit_ampel) {
            //std::cout << "Nord-Süd-Achse: Gelb\n";
            ++anzahlEintraege;
        }
        else {
            // std::cout << "Nord-Süd-Achse: Rot\n";
            ++anzahlEintraege;
        }
    }
}

// Funktion zur Simulation der Ampelkreuzung
void simuliereAmpelkreuzung() {
    int anzahlAutos = eingabeAnzahlAutos();

    vector<Auto> autos;
    Reaktionszeit reaktionszeit(0.2, 0.7, 0.1, 0.75, 1.0, 2.0);

    double verteilungAnfahrt[] = { 0.15, 0.35, 0.15, 0.35 };
    double verteilungZiel[] = { 0.1, 0.4, 0.1, 0.4 };
    double spurbreite = 3.0;
    double kreuzungsgroesse = 59.0;

    for (int i = 0; i < anzahlAutos; ++i) {
        double zufallAnfahrt = rand() / static_cast<double>(RAND_MAX);
        int anfahrtsrichtung = 0;
        while (zufallAnfahrt > verteilungAnfahrt[anfahrtsrichtung]) {
            zufallAnfahrt -= verteilungAnfahrt[anfahrtsrichtung];
            ++anfahrtsrichtung;
        }

        double zufallZiel = rand() / static_cast<double>(RAND_MAX);
        int zielrichtung = 0;
        while (zufallZiel > verteilungZiel[zielrichtung]) {
            zufallZiel -= verteilungZiel[zielrichtung];
            ++zielrichtung;
        }

        Koordinaten position = { 0.0, 0.0 };
        double halbeSpurbreite = spurbreite / 2.0;

        switch (anfahrtsrichtung) {
        case 0:
            position.y = -halbeSpurbreite;
            break;
        case 1:
            position.x = halbeSpurbreite;
            break;
        case 2:
            position.y = halbeSpurbreite;
            break;
        case 3:
            position.x = -halbeSpurbreite;
            break;
        }

        switch (zielrichtung) {
        case 0:
            position.y = -kreuzungsgroesse / 2.0;
            break;
        case 1:
            position.x = kreuzungsgroesse / 2.0;
            break;
        case 2:
            position.y = kreuzungsgroesse / 2.0;
            break;
        case 3:
            position.x = -kreuzungsgroesse / 2.0;
            break;
        }

        Auto auto1(i + 1, 2.0, 4.5, 1.0, 2.0, reaktionszeit.simuliereReaktionszeit(),
            position.x, position.y, zielrichtung == 0 ? 'N' : (zielrichtung == 1 ? 'O' : (zielrichtung == 2 ? 'S' : 'W')),
            anfahrtsrichtung);
        autos.push_back(auto1);
    }

    simuliereAmpelphasen(1);  // Hier wird die Ampelphasen-Simulation für 1 Stunde aufgerufen
}

// Funktion zur Bewegung der Autos
void BewegungderAutos(vector<Auto>& autos, int stunden) {

    deque<Auto> warteschlangeOstWest;
    deque<Auto> warteschlangeNordSued;

    for (int i = 0; i < stunden * 3600; ++i) {
        //cout << "Schleife, Iteration: " << i << "\n"; // Debugging-Ausgabe
        int zeit_innerhalb_phase_ostwest = i % (gruenzeit_ampel_ostwest + gelbzeit_ampel + rotzeit_ampel_ostwest);
        int zeit_innerhalb_phase_nordsued = i % (gruenzeit_ampel_nordsued + rotzeit_ampel_nordsued);

        // Überprüfe die Ampelphasen für Ost-West-Achse
        if (zeit_innerhalb_phase_ostwest < gruenzeit_ampel_ostwest) {
            // Grün: Autos können fahren
            for (auto& auto1 : warteschlangeOstWest) {
                auto1.beschleunigen();
            }
        }
        else if (zeit_innerhalb_phase_ostwest < gruenzeit_ampel_ostwest + gelbzeit_ampel) {
            // Gelb: Autos sollten bremsen
            for (auto& auto1 : warteschlangeOstWest) {
                auto1.bremsen();
            }
        }
        else {
            // Rot: Autos müssen anhalten
            for (auto& auto1 : warteschlangeOstWest) {
                auto1.bremsen();
            }
        }

        // Überprüfe die Ampelphasen für Nord-Süd-Achse
        if (zeit_innerhalb_phase_nordsued < gruenzeit_ampel_nordsued) {
            // Grün: Autos können fahren
            for (auto& auto1 : warteschlangeNordSued) {
                auto1.beschleunigen();
            }
        }
        else if (zeit_innerhalb_phase_nordsued < gruenzeit_ampel_nordsued + gelbzeit_ampel) {
            // Gelb: Autos sollten bremsen
            for (auto& auto1 : warteschlangeNordSued) {
                auto1.bremsen();
            }
        }
        else {
            // Rot: Autos müssen anhalten
            for (auto& auto1 : warteschlangeNordSued) {
                auto1.bremsen();
            }
        }

        // Bewege Autos auf der Kreuzung
        for (auto& auto1 : autos) {
            cout << "Auto " << auto1.id << " - Position: (" << auto1.positionX << ", " << auto1.positionY << ") - Zielrichtung: " << auto1.getZielrichtung() << "\n"; // Debugging-Ausgabe hinzufügen
            cout << "Auto " << auto1.id << " bewegt sich.\n"; // Debugging-Ausgabe 
            // Überprüfe, ob die Ampel rot ist und fahre gegebenenfalls in die Warteschlange
            if ((zeit_innerhalb_phase_ostwest < gruenzeit_ampel_ostwest || zeit_innerhalb_phase_ostwest >= gruenzeit_ampel_ostwest + gelbzeit_ampel) &&
                (auto1.anfahrtsrichtung == 1 || auto1.anfahrtsrichtung == 3)) {
                cout << "Auto " << auto1.id << " in Warteschlange Ost-West\n"; // Debugging-Ausgabe 
                warteschlangeOstWest.push_back(auto1);
            }
            else if ((zeit_innerhalb_phase_nordsued < gruenzeit_ampel_nordsued || zeit_innerhalb_phase_nordsued >= gruenzeit_ampel_nordsued + gelbzeit_ampel) &&
                (auto1.anfahrtsrichtung == 0 || auto1.anfahrtsrichtung == 2)) {
                cout << "Auto " << auto1.id << " in Warteschlange Nord-Sued\n"; // Debugging-Ausgabe 
                warteschlangeNordSued.push_back(auto1);
            }

            else {
                // Überprüfe, ob linksabbiegende Autos frei abbiegen können
                bool freiZumAbbiegen = true;
                for (const auto& auto2 : autos) {
                    if (&auto1 != &auto2) {
                        double abstandX = auto2.positionX - auto1.positionX;
                        double abstandY = auto2.positionY - auto1.positionY;
                        double abstand = sqrt(abstandX * abstandX + abstandY * abstandY);

                        // Überprüfe, ob ein Auto im Weg ist
                        if (abstand < auto1.breite / 2.0 + 0.5 + auto2.breite / 2.0) {
                            freiZumAbbiegen = false;
                            break;
                        }
                    }
                }

                if (freiZumAbbiegen) {
                    auto1.beschleunigen();
                }
                else {
                    auto1.bremsen();
                }
            }

            // Überprüfe, ob Autos die Kreuzung verlassen
            if (auto1.istVerlassen()) {
                // Entferne Autos aus den Warteschlangen, die die Kreuzung verlassen haben
                warteschlangeOstWest.erase(remove_if(warteschlangeOstWest.begin(), warteschlangeOstWest.end(),
                    [&](const Auto& a) { return a.id == auto1.id; }), warteschlangeOstWest.end());
                warteschlangeNordSued.erase(remove_if(warteschlangeNordSued.begin(), warteschlangeNordSued.end(),
                    [&](const Auto& a) { return a.id == auto1.id; }), warteschlangeNordSued.end());
            }
        }
    }
}

// Funktion zur Zählung der Autos nach Zielrichtung
void MyGUI::zaehleAutosNachRichtung(std::vector<Auto>& autos) {
    int nordAutos = 13;
    int ostAutos = 52;
    int suedAutos = 13;
    int westAutos = 52;

    for (const auto& auto1 : autos) {
        if (auto1.istVerlassen()) {
            switch (auto1.getZielrichtung()) {
            case 'N':
                ++nordAutos;
                break;
            case 'O':
                ++ostAutos;
                break;
            case 'S':
                ++suedAutos;
                break;
            case 'W':
                ++westAutos;
                break;
            }
        }
    }

    // Liegen das Ergebniss für GUI (1 Mal)
    functionResultAmpel = nordAutos + ostAutos + suedAutos + westAutos;

    // Liegen das Ergebniss für GUI (50 Mal)
    resultHistory.push_back(functionResultAmpel);
}

//ab hier beginnt die Simulation des Kreisverkehrs
const double PI = 3.14159265358979323846; // Definition von PI als globale Konstante

class KreisverkehrSimulation {  // Klasse zur Simulation des Kreisverkehrs 
public:
    KreisverkehrSimulation() //Funktion zur allgemeinen Defintion der benötigten Parameter 
        : spurbreite(3.0), radiusKreisverkehr(7.5), mittelpunktX(59.0 / 2), mittelpunktY(59.0 / 2), //Definieren des Koordinatensystem im Mittelpunkt der Kreuzung, der Spurbreite und des Radius des eigentlichen Kreisverkehrs
        reaktionszeitGenerator(0.2, 0.7, 0.1, 0.75, 1.0, 2.0), functionResultKreis(0) {} //aufrufen der Funktion um die benötigten Reaktionszeiten der Fahrer zu generieren 

    void simulateKreisverkehr(int stunden, int numAutos) { //übergeordnete Funktion zur Simulation des Kreisverkehrs (aufrufen der anderen void-Funktionen)
        vector<Auto> autos;  // erstellt einen Vektor der alle Autos enthält 
        generiereAutos(numAutos, autos);//generiert die Autos 

        for (int stunde = 0; stunde < stunden; ++stunde) {
            for (auto& autoObj : autos) {
                if (autoObj.positionY < 29.5) { //Bewegung des Autos bis Beginn des Kreisverkehrs
                    moveAutoToKreisverkehr(autoObj); //Zuständige Void Funktion
                }
                else { //anderer Fall: Auto steht bereits am Kreisverkehr
                    moveAutoInKreisverkehr(autoObj, autos);// bewege das Auto im Kreisverkehr 
                    checkUndLeaveKreisverkehr(autoObj);//Überprüfen ob das Auto den Kreisverkehr verlassen hat und zählen des selbigen 
                }
            }
        }
    }

    // get functionResultKreis zum GUI
    int getFunctionResultKreis() const {
        return functionResultKreis;
    }

    int autosVerlassenInRichtung[4] = { 0 };

    void checkUndLeaveKreisverkehr(Auto& autoObj) {
        // Überprüfe, ob das Auto die Kreuzung verlässt
        if (autoObj.positionY > 29, 5 && autoObj.positionX >= -1.5 && autoObj.positionX <= 1.5) {
            autosVerlassenInRichtung[autoObj.zielrichtung]++;
        }
        // Gesamte Zahl von Auto
        functionResultKreis = autosVerlassenInRichtung[0] + autosVerlassenInRichtung[1] + autosVerlassenInRichtung[2] + autosVerlassenInRichtung[3];
    }


private: // Nur in der Funktion benötigten Variablen 
    double spurbreite;
    double radiusKreisverkehr;
    double mittelpunktX;
    double mittelpunktY;
    ReaktionszeitGenerator reaktionszeitGenerator;
    int functionResultKreis;

    void generiereAutos(int numAutos, vector<Auto>& autos) {
        autos.clear();

        // Verteilung für die Anfahrtsrichtung
        double verteilung[4] = { 0.15, 0.35, 0.15, 0.35 };  // Verteilung anhand der vorgegebenen Tabelle 

        for (int id = 1; id <= numAutos; ++id) {
            double startX, startY;
            int anfahrtsRichtung = wähleAnfahrtsRichtung(verteilung);

            switch (anfahrtsRichtung) { //Fallunterscheidung für die verschiedenen Anfahrtsrichtungen 
            case 0: // Norden
                startX = -1.5;
                startY = 29.5;
                break;
            case 1: // Osten
                startX = 29.5;
                startY = 1.5;
                break;
            case 2: // Süden
                startX = 1.5;
                startY = -29.5;
                break;
            case 3: // Westen
                startX = -29.5;
                startY = -1.5;
                break;
            default:
                startX = 0.0;
                startY = 0.0;
            }

            double reaktionszeit = reaktionszeitGenerator.simuliereReaktionszeit();
            autos.emplace_back(id, 2.0, 4.5, 1.0, 2.0, reaktionszeit, startX, startY, 0, anfahrtsRichtung);
            // Hier wird die Zielrichtung basierend auf einer neuen Verteilung festgelegt
            double zielVerteilung = rand() / static_cast<double>(RAND_MAX);
            int zielRichtung;
            if (zielVerteilung < 0.1) {
                zielRichtung = 0;  // 10% nach Norden
            }
            else if (zielVerteilung < 0.5) {
                zielRichtung = 1;  // 40% nach Osten
            }
            else if (zielVerteilung < 0.9) {
                zielRichtung = 2;  // 40% nach Westen
            }
            else {
                zielRichtung = 3;  // 10% nach Süden
            }

            autos.emplace_back(id, 2.0, 4.5, 1.0, 2.0, reaktionszeit, startX, startY, zielRichtung, anfahrtsRichtung);
        }
    }

    // Hilfsfunktion zur Auswahl der Anfahrtsrichtung basierend auf der Verteilung
    int wähleAnfahrtsRichtung(const double verteilung[4]) {
        double zufall = rand() / static_cast<double>(RAND_MAX);
        double kumulativeVerteilung = 0.0;

        for (int i = 0; i < 4; ++i) {
            kumulativeVerteilung += verteilung[i];
            if (zufall <= kumulativeVerteilung) {
                return i;
            }
        }

        // Sollte normalerweise nicht eintreten, reine Debugging-Funktion
        return 0;
    }

    void moveAutoToKreisverkehr(Auto& autoObj) { //Funktion zur Bewegung der Autos bis zum Kreisverkehr 
        autoObj.beschleunigen();

        double neuePositionX = autoObj.positionX + autoObj.geschwindigkeit; //Definition der neuen Koordianaten 
        double neuePositionY = autoObj.positionY;

        if (neuePositionX >= -1.5 && neuePositionX <= 1.5 && neuePositionY >= -29.5 && neuePositionY <= 29.5) {
            autoObj.positionX = 0.0;
            autoObj.positionY = -radiusKreisverkehr;
        }
        else {
            autoObj.positionX = neuePositionX;
            autoObj.positionY = neuePositionY;
        }
    }

    // Funktion zur Überprüfung, ob der Abschnitt im Kreisverkehr frei ist
    bool isKreisverkehrAbschnittFrei(double startWinkel, double endWinkel, const vector<Auto>& autos, const Auto& currentAuto) {
        const double sicherheitsabstand = 0.5;  // Zusätzlicher Sicherheitsabstand in Metern

        // Bestimme den Abschnitt basierend auf dem Startwinkel
        int startAbschnitt = getKreisverkehrAbschnitt(startWinkel);

        // Bestimme den Abschnitt basierend auf dem Endwinkel
        int endAbschnitt = getKreisverkehrAbschnitt(endWinkel);

        // Iteriere über alle Autos und überprüfe, ob sich ein Auto im zu überprüfenden Abschnitt befindet
        for (const auto& autoObj : autos) {
            // Ignoriere das aktuelle Auto bei der Überprüfung
            if (autoObj.id == currentAuto.id) {
                continue;
            }

            // Bestimme den Abschnitt des aktuellen Autos
            double autoWinkel = atan2(autoObj.positionY, autoObj.positionX);
            int autoAbschnitt = getKreisverkehrAbschnitt(autoWinkel);

             // Überprüfe, ob sich das Auto im zu überprüfenden Abschnitt befindet
            if (autoAbschnitt >= startAbschnitt && autoAbschnitt <= endAbschnitt) {
                //cout << "Checking collisions for Auto " << autoObj.id << "\n"; //Debugging Ausgabe in der Konsole 

                // Berechne die Distanz zwischen den Autos
                double distanz = sqrt(pow(autoObj.positionX - currentAuto.positionX, 2) + pow(autoObj.positionY - currentAuto.positionY, 2));

                // Überprüfe, ob die Distanz kleiner als der Sicherheitsabstand ist
                if (distanz < sicherheitsabstand) {
                    //cout << "Kollision mit Auto " << autoObj.id << " erkannt.\n"; //Debugging Ausgabe in der Konsole
                    return false;  // Der Abschnitt ist nicht frei
                }
            }
        }
        return true;
    }


    void moveAutoInKreisverkehr(Auto& autoObj, const vector<Auto>& autos) {

        autoObj.beschleunigen();

        double aktuellerWinkel = atan2(autoObj.positionY, autoObj.positionX);
        double neuerWinkel = aktuellerWinkel + (autoObj.geschwindigkeit / radiusKreisverkehr);

        int abschnitt = getKreisverkehrAbschnitt(aktuellerWinkel);

        if (isKreisverkehrAbschnittFrei(aktuellerWinkel, neuerWinkel, autos, autoObj)) {
            autoObj.positionX = radiusKreisverkehr * cos(neuerWinkel);
            autoObj.positionY = radiusKreisverkehr * sin(neuerWinkel);
        }
        else {
            autoObj.bremsen();
        }
    }

    // Funktion zur Bestimmung des Kreisverkehrsabschnitts basierend auf dem Winkel
    int getKreisverkehrAbschnitt(double winkel) {
        // Teile den Kreisverkehr in vier Abschnitte
        const double viertelKreis = PI / 2.0;

        // Bestimme den Abschnitt basierend auf dem Winkel
        if (winkel >= -viertelKreis && winkel < 0.0) {
            return 0; // Abschnitt 0
        }
        else if (winkel >= 0.0 && winkel < viertelKreis) {
            return 1; // Abschnitt 1
        }
        else if (winkel >= viertelKreis && winkel < 2 * viertelKreis) {
            return 2; // Abschnitt 2
        }
        else {
            return 3; // Abschnitt 3
        }
    }

};

void MyGUI::clearHistory() {
    resultHistory.clear();
}

void MyGUI::getKreis() {
    // Nutzereingabe für die Anzahl der zu generierenden Autos
    int numAutos = 500;

    //KreisverkehrSimulation simulation;
    KreisverkehrSimulation simulation;

    simulation.simulateKreisverkehr(1, numAutos);
    functionResultKreis = simulation.getFunctionResultKreis();
    
    resultHistory.push_back(functionResultKreis);
}

int main() {
    std::vector<Auto> autos;
    // Setze den Zufallszahlengenerator
    srand(static_cast<unsigned int>(time(0)));

    simuliereAmpelkreuzung();
    BewegungderAutos(autos, 1);

    srand(time(0));

    MyGUI gui(autos);
    gui.run();

    return 0;
}