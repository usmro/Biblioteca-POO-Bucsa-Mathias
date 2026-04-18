#ifndef BIBLIOTECAR_H
#define BIBLIOTECAR_H

#include "Angajat.h"
#include "../servicii/Biblioteca.h"

class Bibliotecar : public Angajat {
public:
    Bibliotecar(string nume, int id, string username, string parola,
                double salariu);

    // Adauga o carte noua in catalog
    void adaugaCarte(Biblioteca& biblioteca, Carte* carte);

    // Sterge o carte din catalog
    void eliminaCarte(Biblioteca& biblioteca, const string& isbn);

    // Afiseaza toti utilizatorii si imprumuturile lor
    void afiseazaTotiUtilizatorii(const vector<Utilizator>& utilizatori) const;

    void afiseazaInfo() const override;
};

#endif