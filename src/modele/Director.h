#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "Angajat.h"
#include "../servicii/Biblioteca.h"
#include <vector>

class Director : public Angajat {
private:
    vector<Angajat*> echipa; // angajatii din subordine

public:
    Director(string nume, int id, string username, string parola,
             double salariu);

    // Adauga un angajat in echipa
    void adaugaAngajat(Angajat* angajat);

    // Acorda bonus unui angajat
    void acordaBonus(Angajat* angajat, double bonus);

    // Afiseaza raport financiar complet
    void afiseazaRaportFinanciar(const Biblioteca& biblioteca,
                                  const vector<Utilizator>& utilizatori) const;

    // Afiseaza echipa
    void afiseazaEchipa() const;

    void afiseazaInfo() const override;

    ~Director();
};

#endif