#include "Utilizator.h"
#include <iostream>
#include <algorithm>

Utilizator::Utilizator(string nume, int id) : nume(nume), id(id) {}

string Utilizator::getNume() const { return nume; }
int Utilizator::getId() const { return id; }
vector<string> Utilizator::getImprumuturi() const { return listaImprumuturi; }

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