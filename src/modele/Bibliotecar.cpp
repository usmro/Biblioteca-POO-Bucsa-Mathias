#include "Bibliotecar.h"
#include <iostream>

Bibliotecar::Bibliotecar(string nume, int id, string username,
                          string parola, double salariu)
    : Angajat(nume, id, username, parola, salariu,
              "Gestiune Carti", RolAngajat::BIBLIOTECAR) {}

void Bibliotecar::adaugaCarte(Biblioteca& biblioteca, Carte* carte) {
    cout << "[BIBLIOTECAR] " << getNume() << " adauga carte: "
         << carte->getTitlu() << endl;
    biblioteca.adaugaCarte(carte);
}

void Bibliotecar::eliminaCarte(Biblioteca& biblioteca, const string& isbn) {
    cout << "[BIBLIOTECAR] " << getNume()
         << " elimina cartea cu ISBN: " << isbn << endl;
    biblioteca.eliminaCarte(isbn);
}

void Bibliotecar::afiseazaTotiUtilizatorii(
        const vector<Utilizator>& utilizatori) const {
    cout << "\n=== Toti Utilizatorii ===" << endl;
    for (const auto& u : utilizatori) {
        cout << "  - " << u.getNume()
             << " (ID: " << u.getId()
             << ", Username: " << u.getUsername() << ")" << endl;
    }
    cout << "=========================" << endl;
}

void Bibliotecar::afiseazaInfo() const {
    cout << "[BIBLIOTECAR] ";
    Angajat::afiseazaInfo();
}