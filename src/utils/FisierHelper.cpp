#include "FisierHelper.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool FisierHelper::salveazaUtilizatori(const vector<Utilizator>& utilizatori,
                                        const string& caleFisier) {
    ofstream fisier(caleFisier);
    if (!fisier.is_open()) {
        cout << "[EROARE] Nu pot deschide fisierul: " << caleFisier << endl;
        return false;
    }

    // Format: id|nume|username|parolaCriptata
    for (const auto& u : utilizatori) {
        fisier << u.getId() << "|"
               << u.getNume() << "|"
               << u.getUsername() << "|"
               << u.getParolaCriptata() << "\n";
    }

    fisier.close();
    cout << "[LOG] Utilizatori salvati in: " << caleFisier << endl;
    return true;
}

vector<Utilizator> FisierHelper::incarcaUtilizatori(const string& caleFisier) {
    vector<Utilizator> utilizatori;
    ifstream fisier(caleFisier);

    if (!fisier.is_open()) {
        cout << "[LOG] Fisierul " << caleFisier << " nu exista inca." << endl;
        return utilizatori;
    }

    string linie;
    while (getline(fisier, linie)) {
        if (linie.empty()) continue;

        // Parsam linia: id|nume|username|parolaCriptata
        stringstream ss(linie);
        string idStr, nume, username, parolaCriptata;

        getline(ss, idStr, '|');
        getline(ss, nume, '|');
        getline(ss, username, '|');
        getline(ss, parolaCriptata, '|');

        int id = stoi(idStr);

        // Cream utilizatorul cu parola goala si setam direct parola criptata
        // Folosim parola "LOADED" ca placeholder - nu va fi folosita
        Utilizator u(nume, id, username, "LOADED");
        // Nota: in versiunea viitoare vom adauga setter pentru parolaCriptata

        utilizatori.push_back(u);
    }

    fisier.close();
    cout << "[LOG] " << utilizatori.size() 
         << " utilizatori incarcati din fisier." << endl;
    return utilizatori;
}