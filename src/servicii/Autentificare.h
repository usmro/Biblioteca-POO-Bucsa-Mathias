#ifndef AUTENTIFICARE_H
#define AUTENTIFICARE_H

#include <string>
#include <vector>
#include "../modele/Utilizator.h"
using namespace std;

class Autentificare {
private:
    vector<Utilizator>& utilizatori;
    int nextId;

public:
    Autentificare(vector<Utilizator>& utilizatori);

    // Incearca sa autentifice un utilizator
    // Returneaza pointer la utilizator sau nullptr daca esueaza
    Utilizator* login(const string& username, const string& parola);

    // Creeaza un cont nou
    bool creeazaCont(const string& nume, const string& username,
                     const string& parola);

    // Verifica daca username-ul exista deja
    bool usernameExista(const string& username) const;
};

#endif