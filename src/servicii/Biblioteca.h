#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "../modele/Carte.h"
#include "../modele/Utilizator.h"
#include "../modele/Imprumut.h"
#include <vector>
#include <unordered_map>

class Biblioteca {
private:
    vector<Carte*> carti;
    vector<Utilizator> utilizatori;
    vector<Imprumut> imprumuturiActive;
    
    // Indexuri pentru cautare rapida
    unordered_map<string, Carte*> indexIsbn;
    unordered_map<string, vector<Carte*>> indexAutor;
    unordered_map<string, vector<Carte*>> indexTip;
    unordered_map<string, vector<Carte*>> indexGen;

public:
// Cautare si filtrare
void cautaDupaAutor(const string& autor) const;
void cautaDupaTitlu(const string& titlu) const;
void cautaDupaGen(const string& gen) const;
void filtreazaDupaDisponibilitate(bool disponibile) const;
void filtreazaDupaTip(const string& tip) const;
    void adaugaCarte(Carte* carte);
    void eliminaCarte(const string& isbn);
    void afiseazaCarti() const;

    void adaugaUtilizator(const Utilizator& utilizator);

    bool imprumutaCarte(int idUtilizator, const string& isbn, int zileLimita = 14);
    bool returneazaCarte(int idUtilizator, const string& isbn);

    // NOU - raport penalitati
    void afiseazaRaportPenalitati() const;
    Utilizator* getUtilizator(int id);

    void salveazaCarti(const string& caleFisier) const;
    void incarcaCarti(const string& caleFisier);
    int getNumarCarti() const;
    bool stergeUtilizator(int id);
    string genereazaIsbnNou() const;

    ~Biblioteca();
};

#endif