#ifndef UTILIZATOR_H
#define UTILIZATOR_H

#include <string>
#include <vector>
#include "../utils/Criptare.h"
using namespace std;

class Utilizator {
private:
    string nume;
    int id;
    string username;
    string parolaCriptata;
    vector<string> listaImprumuturi;

public:
    Utilizator(string nume, int id, string username, string parola);

    string getNume() const;
    int getId() const;
    string getUsername() const;
    string getParolaCriptata() const;
    vector<string> getImprumuturi() const;

    bool verificaParola(const string& parola) const;

    void adaugaImprumut(const string& isbn);
    void eliminaImprumut(const string& isbn);
    void afiseazaImprumuturi() const;
};

#endif