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

// Clasa derivata - Carte Digitala
class CarteDigitala : public Carte {
private:
    string format;   // PDF, EPUB, MOBI
    string linkDownload;
    double dimensiuneMB;

public:
    CarteDigitala(string titlu, string autor, string isbn, 
                  string format, string linkDownload, double dimensiuneMB);
    
    string getFormat() const;
    string getLinkDownload() const;
    double getDimensiuneMB() const;
    
    void afiseazaDetalii() const override;
};

// Clasa derivata - Audiobook
class Audiobook : public Carte {
private:
    string narator;
    int durataMinte;   // durata in minute

public:
    Audiobook(string titlu, string autor, string isbn,
              string narator, int durataMinte);
    
    string getNarator() const;
    int getDurataMinute() const;
    string getDurataFormata() const;  // ex: "2h 30min"
    
    void afiseazaDetalii() const override;
};

#endif