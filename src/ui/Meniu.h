#ifndef MENIU_H
#define MENIU_H

#include "../servicii/Biblioteca.h"
#include "../servicii/Autentificare.h"
#include "../utils/FisierHelper.h"
#include "../modele/Bibliotecar.h"
#include "../modele/Director.h"


class Meniu {
private:
    Biblioteca& biblioteca;
    Autentificare& auth;
    vector<Utilizator>& utilizatori;
    const string FISIER_UTILIZATORI = "date/utilizatori.txt";
    const string FISIER_CARTI = "date/carti.txt";
    const string FISIER_ANGAJATI = "date/angajati.txt";

    Utilizator* utilizatorCurent = nullptr;
    Angajat* angajatCurent = nullptr;      // NOU
    Director* director = nullptr;           // NOU
    Bibliotecar* bibliotecar = nullptr;     // NOU

    void afiseazaMenuPrincipal();
    void afiseazaMenuUtilizator();
    void afiseazaMenuBibliotecar();         // NOU
    void afiseazaMenuDirector();            // NOU

    void handleLogin();
    void handleCreareCont();
    void handleVezeCatalog();
    void handleImprumut();
    void handleReturnare();
    void handleImprumuturileMele();
    void handleRaportPenalitati();

    // NOU - actiuni bibliotecar
    void handleAdaugaCarte();
    void handleEliminaCarte();
    void handleVeziUtilizatori();

    // NOU - actiuni director
    void handleRaportFinanciar();
    void handleAcordaBonus();
    void handleVeziEchipa();

    void clearScreen();
    void asteaptaEnter();
    void afiseazaMenuCautare();
    void handleFiltrareCatalog();
    int citesteOptiune();
    void handleStergeUtilizator();
    void handleGestionareAngajati();

public:
    Meniu(Biblioteca& biblioteca, Autentificare& auth,
          vector<Utilizator>& utilizatori);
    void ruleaza();
    ~Meniu();
};

#endif