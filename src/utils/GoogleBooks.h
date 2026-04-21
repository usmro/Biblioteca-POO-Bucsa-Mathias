#ifndef GOOGLEBOOKS_H
#define GOOGLEBOOKS_H

#include <string>
#include <vector>
using namespace std;

struct RezultatCarte {
    string titlu;
    string autor;
    string isbn;
    string descriere;
    string gen;
};

class GoogleBooks {
public:
    // Cauta carti dupa titlu
    static vector<RezultatCarte> cauta(const string& query);

    // Afiseaza rezultatele
    static void afiseazaRezultate(const vector<RezultatCarte>& rezultate);
};

#endif