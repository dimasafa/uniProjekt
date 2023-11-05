#pragma once
#include <cstdlib>
class Reaktionszeit {
public:
    Reaktionszeit(double haeufigkeitA, double haeufigkeitB, double haeufigkeitC,
        double reaktionszeitA, double reaktionszeitB, double reaktionszeitC)
        : haeufigkeitA(haeufigkeitA), haeufigkeitB(haeufigkeitB), haeufigkeitC(haeufigkeitC),
        reaktionszeitA(reaktionszeitA), reaktionszeitB(reaktionszeitB), reaktionszeitC(reaktionszeitC) {}

    double simuliereReaktionszeit() {
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

private:
    double haeufigkeitA;
    double haeufigkeitB;
    double haeufigkeitC;
    double reaktionszeitA;
    double reaktionszeitB;
    double reaktionszeitC;
};