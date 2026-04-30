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
    virtual string getGen() const { return ""; }
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
    string getGen() const override { return format; }
    
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
    void afiseazaDetalii() const override;
    string getTip() const override { return "FICTIUNE"; }
    string getGen() const override { return gen; }

};

class CarteTehnica : public CarteFizica {
private:
    string domeniu;
public:
    CarteTehnica(string titlu, string autor, string isbn, string domeniu);
    string getDomeniu() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "TEHNICA"; }
    string getGen() const override { return domeniu; }

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
    string getGen() const override { return luna; }

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
    string getGen() const override { return mangaka; }

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
    string getGen() const override { return subiect; }

};

class Stiinta : public CarteFizica {
private:
    string ramura;
public:
    Stiinta(string titlu, string autor, string isbn, string ramura);
    string getRamura() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "STIINTA"; }
    string getGen() const override { return ramura; }

};

class Istorie : public CarteFizica {
private:
    string perioada;
public:
    Istorie(string titlu, string autor, string isbn, string perioada);
    string getPerioada() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "ISTORIE"; }
    string getGen() const override { return perioada; }

};

class Filozofie : public CarteFizica {
private:
    string curent;
public:
    Filozofie(string titlu, string autor, string isbn, string curent);
    string getCurent() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "FILOZOFIE"; }
    string getGen() const override { return curent; }

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
    string getGen() const override { return materie; }

};

class Enciclopedie : public CarteFizica {
private:
    string domeniu;
public:
    Enciclopedie(string titlu, string autor, string isbn, string domeniu);
    string getDomeniu() const;
    void afiseazaDetalii() const override;
    string getTip() const override { return "ENCICLOPEDIE"; }
    string getGen() const override { return domeniu; }

};

#endif