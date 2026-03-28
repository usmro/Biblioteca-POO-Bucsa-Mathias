#ifndef UTILIZATOR_H
#define UTILIZATOR_H

#include <string>
#include <vector>
using namespace std;

class Utilizator {
private:
    string nume;
    int id;
    vector<string> listaImprumuturi;

public:
    Utilizator(string nume, int id);

    string getNume() const;
    int getId() const;
    vector<string> getImprumuturi() const;

    void adaugaImprumut(const string& isbn);
    void eliminaImprumut(const string& isbn);
    void afiseazaImprumuturi() const;
};

#endif