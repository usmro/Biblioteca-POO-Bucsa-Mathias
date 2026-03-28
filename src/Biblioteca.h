#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "Carte.h"
#include "Utilizator.h"
#include "Imprumut.h"
#include <vector>

class Biblioteca {
private:
    vector<Carte*> carti;
    vector<Utilizator> utilizatori;
    vector<Imprumut> imprumuturiActive; // NOU

public:
    void adaugaCarte(Carte* carte);
    void eliminaCarte(const string& isbn);
    void afiseazaCarti() const;

    void adaugaUtilizator(const Utilizator& utilizator);

    bool imprumutaCarte(int idUtilizator, const string& isbn, int zileLimita = 14);
    bool returneazaCarte(int idUtilizator, const string& isbn);

    // NOU - raport penalitati
    void afiseazaRaportPenalitati() const;

    ~Biblioteca();
};

#endif