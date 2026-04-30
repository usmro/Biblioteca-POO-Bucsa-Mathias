#include "Utilizator.h"
#include <iostream>
#include <algorithm>

Utilizator::Utilizator(string nume, int id, string username, string parola)
    : nume(nume), id(id), username(username) {
    // Parola e criptata automat la creare
    parolaCriptata = Criptare::sha256(parola);}

string Utilizator::getNume() const { return nume; }
int Utilizator::getId() const { return id; }
string Utilizator::getUsername() const { return username; }
string Utilizator::getParolaCriptata() const { return parolaCriptata; }
vector<string> Utilizator::getImprumuturi() const { return listaImprumuturi; }

bool Utilizator::verificaParola(const string& parola) const {
    return Criptare::verificaParola(parola, parolaCriptata);
}

void Utilizator::adaugaImprumut(const string& isbn) {
    listaImprumuturi.push_back(isbn);
}

void Utilizator::eliminaImprumut(const string& isbn) {
    listaImprumuturi.erase(
        remove(listaImprumuturi.begin(), listaImprumuturi.end(), isbn),
        listaImprumuturi.end()
    );
}

void Utilizator::afiseazaImprumuturi() const {
    cout << "Utilizator: " << nume << " (ID: " << id << ")" << endl;
    if (listaImprumuturi.empty()) {
        cout << "  Niciun imprumut activ." << endl;
    } else {
        for (const auto& isbn : listaImprumuturi)
            cout << "  - ISBN: " << isbn << endl;
    }
}

void Utilizator::setParolaCriptata(const string& hash) {
    parolaCriptata = hash;
}