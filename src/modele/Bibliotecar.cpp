#include "Bibliotecar.h"
#include "../utils/FisierHelper.h"
#include "../utils/Exceptii.h"
#include <iostream>
#include <algorithm>

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


void Bibliotecar::stergeUtilizator(Biblioteca& biblioteca,
                                    vector<Utilizator>& utilizatori,
                                    const string& fisierUtilizatori, int id) {
    cout << "[BIBLIOTECAR] " << getNume()
         << " sterge utilizatorul cu ID: " << id << endl;
    biblioteca.stergeUtilizator(id);

    // Stergem si din lista locala
    utilizatori.erase(
        remove_if(utilizatori.begin(), utilizatori.end(),
            [id](const Utilizator& u) { return u.getId() == id; }),
        utilizatori.end()
    );

    FisierHelper::salveazaUtilizatori(utilizatori, fisierUtilizatori);
}