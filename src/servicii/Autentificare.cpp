#include "Autentificare.h"
#include "../utils/Exceptii.h"
#include <iostream>

Autentificare::Autentificare(vector<Utilizator>& utilizatori)
    : utilizatori(utilizatori), nextId(1) {
    for (const auto& u : utilizatori)
        if (u.getId() >= nextId)
            nextId = u.getId() + 1;
}

Utilizator* Autentificare::login(const string& username,
                                   const string& parola) {
    for (auto& u : utilizatori) {
        if (u.getUsername() == username) {
            if (!u.verificaParola(parola))
                throw ParolaIncorectaException();
            cout << "[LOG] Login reusit: " << username << endl;
            return &u;
        }
    }
    throw UtilizatorNegasitException(0);
}

bool Autentificare::creeazaCont(const string& nume,
                                  const string& username,
                                  const string& parola) {
    if (usernameExista(username))
        throw UsernameExistentException(username);

    utilizatori.push_back(Utilizator(nume, nextId++, username, parola));
    cout << "[LOG] Cont creat cu succes pentru: " << username << endl;
    return true;
}

bool Autentificare::usernameExista(const string& username) const {
    for (const auto& u : utilizatori)
        if (u.getUsername() == username)
            return true;
    return false;
}