#ifndef CARTE_H
#define CARTE_H

#include <string>
using namespace std;

// ============================================
// CLASA DE BAZA - CARTE
// ============================================
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
    virtual string getTip() const { return "CARTE"; }
    virtual ~Carte() {}
};

// ============================================
// CARTE DIGITALA - derivata din Carte
// ============================================
class CarteDigitala : public Carte {
private:
    string format;
    string linkDownload;
    double dimensiuneMB;

public:
    CarteDigitala(string titlu, string autor, string isbn,
                  string format, string linkDownload, double dimensiuneMB);

    string getFormat() const;
    string getLinkDownload() const;
    double getDimensiuneMB() const;

    void afiseazaDetalii() const override;
    string getTip() const override { return "DIGITAL"; }
};

// ============================================
// AUDIOBOOK - derivata din Carte
// ============================================
class Audiobook : public Carte {
private:
    string narator;
    int durataMinute;

public:
    Audiobook(string titlu, string autor, string isbn,
              string narator, int durataMinute);

    string getNarator() const;
    int getDurataMinute() const;
    string getDurataFormata() const;

    void afiseazaDetalii() const override;
    string getTip() const override { return "AUDIOBOOK"; }
};

// ============================================
// CARTE FIZICA - derivata din Carte (nivel 2)
// ============================================
class CarteFizica : public Carte {
private:
    string stare; // buna, deteriorata, distrusa

public:
    CarteFizica(string titlu, string autor, string isbn,
                string stare = "buna");

    string getStare() const;
    void setStare(const string& stare);

    void afiseazaDetalii() const override;
    string getTip() const override { return "FIZICA"; }
};

// ============================================
// DERIVATE DIN CARTE FIZICA (nivel 3)
// ============================================

class CarteFictiune : public CarteFizica {
private:
    string gen;
public:
    CarteFictiune(string titlu, string autor, string isbn, string gen);
    string getGen() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "FICTIUNE"; }
};

class CarteTehnica : public CarteFizica {
private:
    string domeniu;
public:
    CarteTehnica(string titlu, string autor, string isbn, string domeniu);
    string getDomeniu() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "TEHNICA"; }
};

class Revista : public CarteFizica {
private:
    int numar;
    string luna;
public:
    Revista(string titlu, string autor, string isbn, int numar, string luna);
    int getNumar() const;
    string getLuna() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "REVISTA"; }
};

class Manga : public CarteFizica {
private:
    int volum;
    string mangaka;
public:
    Manga(string titlu, string autor, string isbn, int volum, string mangaka);
    int getVolum() const;
    string getMangaka() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "MANGA"; }
};

class BenziDesonate : public CarteFizica {
private:
    string artist;
public:
    BenziDesonate(string titlu, string autor, string isbn, string artist);
    string getArtist() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "BENZI_DESENATE"; }
};

class RomanGrafic : public CarteFizica {
private:
    string artist;
public:
    RomanGrafic(string titlu, string autor, string isbn, string artist);
    string getArtist() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "ROMAN_GRAFIC"; }
};

class Biografie : public CarteFizica {
private:
    string subiect;
public:
    Biografie(string titlu, string autor, string isbn, string subiect);
    string getSubiect() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "BIOGRAFIE"; }
};

class Stiinta : public CarteFizica {
private:
    string ramura;
public:
    Stiinta(string titlu, string autor, string isbn, string ramura);
    string getRamura() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "STIINTA"; }
};

class Istorie : public CarteFizica {
private:
    string perioada;
public:
    Istorie(string titlu, string autor, string isbn, string perioada);
    string getPerioada() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "ISTORIE"; }
};

class Filozofie : public CarteFizica {
private:
    string curent;
public:
    Filozofie(string titlu, string autor, string isbn, string curent);
    string getCurent() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "FILOZOFIE"; }
};

class Manual : public CarteFizica {
private:
    string materie;
    string nivel; // gimnaziu, liceu, facultate
public:
    Manual(string titlu, string autor, string isbn,
           string materie, string nivel);
    string getMaterie() const;
    string getNivel() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "MANUAL"; }
};

class Enciclopedie : public CarteFizica {
private:
    string domeniu;
public:
    Enciclopedie(string titlu, string autor, string isbn, string domeniu);
    string getDomeniu() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "ENCICLOPEDIE"; }
};

#endif