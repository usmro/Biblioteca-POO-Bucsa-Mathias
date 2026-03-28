#ifndef CARTE_H
#define CARTE_H

#include <string>
using namespace std;

class Carte {
private:
    string titlu;
    string autor;
    string isbn;
    bool disponibila;

public:
    Carte(string titlu, string autor, string isbn);

    string getTitlu() const;
    string getAutor() const;
    string getIsbn() const;
    bool esteDisponibila() const;
    void setDisponibila(bool status);

    virtual void afiseazaDetalii() const;
    virtual ~Carte() {}
};

class CarteFictiune : public Carte {
private:
    string gen;
public:
    CarteFictiune(string titlu, string autor, string isbn, string gen);
    void afiseazaDetalii() const override;
};

class CarteTehnica : public Carte {
private:
    string domeniu;
public:
    CarteTehnica(string titlu, string autor, string isbn, string domeniu);
    void afiseazaDetalii() const override;
};

#endif