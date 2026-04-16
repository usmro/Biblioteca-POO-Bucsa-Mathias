#ifndef MENIU_H
#define MENIU_H

#include "Biblioteca.h"
#include "Autentificare.h"
#include "utils/FisierHelper.h"

class Meniu {
private:
    Biblioteca& biblioteca;
    Autentificare& auth;
    vector<Utilizator>& utilizatori;
    const string FISIER_UTILIZATORI = "date/utilizatori.txt";

    Utilizator* utilizatorCurent = nullptr;

    // Meniuri
    void afiseazaMenuPrincipal();
    void afiseazaMenuUtilizator();

    // Actiuni
    void handleLogin();
    void handleCreareCont();
    void handleVezeCatalog();
    void handleImprumut();
    void handleReturnare();
    void handleImprumuturileMele();
    void handleRaportPenalitati();

    // Utilitare
    void clearScreen();
    void asteaptaEnter();

public:
    Meniu(Biblioteca& biblioteca, Autentificare& auth, 
          vector<Utilizator>& utilizatori);
    void ruleaza();
};

#endif