#ifndef FISIERHELPER_H
#define FISIERHELPER_H

#include <string>
#include <vector>
#include "../Utilizator.h"
using namespace std;

class FisierHelper {
public:
    // Salveaza toti utilizatorii in fisier
    static bool salveazaUtilizatori(const vector<Utilizator>& utilizatori,
                                     const string& caleFisier);

    // Incarca utilizatorii din fisier
    static vector<Utilizator> incarcaUtilizatori(const string& caleFisier);
};

#endif