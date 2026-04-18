#ifndef FISIERHELPER_H
#define FISIERHELPER_H

#include <string>
#include <vector>
#include "../modele/Utilizator.h"
#include "../modele/Carte.h"
#include "../modele/Angajat.h"
#include "../modele/Bibliotecar.h"
#include "../modele/Director.h"

using namespace std;

class FisierHelper {
public:
    // Utilizatori
    static bool salveazaUtilizatori(const vector<Utilizator>& utilizatori,
                                     const string& caleFisier);
    static vector<Utilizator> incarcaUtilizatori(const string& caleFisier);

    // Carti
    static bool salveazaCarti(const vector<Carte*>& carti,
                               const string& caleFisier);
    static vector<Carte*> incarcaCarti(const string& caleFisier);

    static bool salveazaAngajati(Director* director, Bibliotecar* bibliotecar,
                              const string& caleFisier);
    
    static pair<Director*, Bibliotecar*> incarcaAngajati(const string& caleFisier);
};

#endif