#pragma once
class ReaktionszeitGenerator {  //Klasse um die Reaktionszeiten der einzelnen Fahrertypen abzurufen 
public:
    ReaktionszeitGenerator(double haeufigkeitA, double haeufigkeitB, double haeufigkeitC,  //Struktur für das generieren der Fahrertyp spezifischen Reaktionszeit
        double reaktionszeitA, double reaktionszeitB, double reaktionszeitC)
        : haeufigkeitA(haeufigkeitA), haeufigkeitB(haeufigkeitB), haeufigkeitC(haeufigkeitC),
        reaktionszeitA(reaktionszeitA), reaktionszeitB(reaktionszeitB), reaktionszeitC(reaktionszeitC) {}

    double simuliereReaktionszeit() {  //Wahl des Fahrertyps anhand der ihm zugeordneten Wahrscheinlichkeit
        double zufall = rand() / static_cast<double>(RAND_MAX);
        char fahrertyp;

        if (zufall < haeufigkeitA) {
            fahrertyp = 'A';
        }
        else if (zufall < haeufigkeitA + haeufigkeitB) {
            fahrertyp = 'B';
        }
        else {
            fahrertyp = 'C';
        }

        switch (fahrertyp) {
        case 'A':
            return reaktionszeitA;
        case 'B':
            return reaktionszeitB;
        case 'C':
            return reaktionszeitC;
        }

        return 0.0;
    }

private: // Variablen, welche nur in dieser Funktion benötigt werden
    double haeufigkeitA;
    double haeufigkeitB;
    double haeufigkeitC;
    double reaktionszeitA;
    double reaktionszeitB;
    double reaktionszeitC;
};