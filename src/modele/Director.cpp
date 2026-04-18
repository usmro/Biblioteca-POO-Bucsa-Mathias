#include "Director.h"
#include <iostream>

Director::Director(string nume, int id, string username,
                   string parola, double salariu)
    : Angajat(nume, id, username, parola, salariu,
              "Conducere", RolAngajat::DIRECTOR) {}

void Director::adaugaAngajat(Angajat* angajat) {
    echipa.push_back(angajat);
    cout << "[DIRECTOR] " << getNume() << " a adaugat angajatul: "
         << angajat->getNume() << " ("
         << angajat->getRolString() << ")" << endl;
}

void Director::acordaBonus(Angajat* angajat, double bonus) {
    double salariuVechi = angajat->getSalariu();
    angajat->setSalariu(salariuVechi + bonus);
    cout << "[DIRECTOR] Bonus de " << bonus << " lei acordat lui "
         << angajat->getNume() << ". Salariu nou: "
         << angajat->getSalariu() << " lei" << endl;
}

void Director::afiseazaRaportFinanciar(
        const Biblioteca& biblioteca,
        const vector<Utilizator>& utilizatori) const {
    cout << "\n=== Raport Financiar Complet ===" << endl;
    cout << "Numar utilizatori: " << utilizatori.size() << endl;
    cout << "Numar angajati in echipa: " << echipa.size() << endl;

    double totalSalarii = getSalariu();
    for (const auto& a : echipa)
        totalSalarii += a->getSalariu();
    cout << "Total salarii lunare: " << totalSalarii << " lei" << endl;

    cout << "\nAngajati:" << endl;
    afiseazaEchipa();
    cout << "=================================" << endl;
}

void Director::afiseazaEchipa() const {
    cout << "  [DIRECTOR] " << getNume()
         << " | Salariu: " << getSalariu() << " lei" << endl;
    for (const auto& a : echipa)
        a->afiseazaInfo();
}

void Director::afiseazaInfo() const {
    cout << "[DIRECTOR] ";
    Angajat::afiseazaInfo();
}

Director::~Director() {
    for (auto& a : echipa) delete a;
}