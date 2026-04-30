#include "Carte.h"
#include <iostream>
using namespace std;

// ============================================
// CARTE - baza
// ============================================
Carte::Carte(string titlu, string autor, string isbn)
    : titlu(titlu), autor(autor), isbn(isbn), disponibila(true) {}

string Carte::getTitlu() const { return titlu; }
string Carte::getAutor() const { return autor; }
string Carte::getIsbn() const { return isbn; }
bool Carte::esteDisponibila() const { return disponibila; }
void Carte::setDisponibila(bool status) { disponibila = status; }

void Carte::afiseazaDetalii() const {
    cout << titlu << " | Autor: " << autor
         << " | ISBN: " << isbn
         << " | " << (disponibila ? "Disponibila" : "Imprumutata");
}

// ============================================
// CARTE DIGITALA
// ============================================
CarteDigitala::CarteDigitala(string titlu, string autor, string isbn,
                              string format, string linkDownload,
                              double dimensiuneMB)
    : Carte(titlu, autor, isbn), format(format),
      linkDownload(linkDownload), dimensiuneMB(dimensiuneMB) {}

string CarteDigitala::getFormat() const { return format; }
string CarteDigitala::getLinkDownload() const { return linkDownload; }
double CarteDigitala::getDimensiuneMB() const { return dimensiuneMB; }

void CarteDigitala::afiseazaDetalii() const {
    cout << "[DIGITAL - " << format << " | " << dimensiuneMB << "MB] ";
    Carte::afiseazaDetalii();
    cout << endl;
    cout << "  Download: " << linkDownload << endl;
}

// ============================================
// AUDIOBOOK
// ============================================
Audiobook::Audiobook(string titlu, string autor, string isbn,
                     string narator, int durataMinute)
    : Carte(titlu, autor, isbn), narator(narator),
      durataMinute(durataMinute) {}

string Audiobook::getNarator() const { return narator; }
int Audiobook::getDurataMinute() const { return durataMinute; }

string Audiobook::getDurataFormata() const {
    int ore = durataMinute / 60;
    int minute = durataMinute % 60;
    return to_string(ore) + "h " + to_string(minute) + "min";
}

void Audiobook::afiseazaDetalii() const {
    cout << "[AUDIOBOOK | " << getDurataFormata()
         << " | Narator: " << narator << "] ";
    Carte::afiseazaDetalii();
    cout << endl;
}

// ============================================
// CARTE FIZICA
// ============================================
CarteFizica::CarteFizica(string titlu, string autor, string isbn, string stare)
    : Carte(titlu, autor, isbn), stare(stare) {}

string CarteFizica::getStare() const { return stare; }
void CarteFizica::setStare(const string& s) { stare = s; }

void CarteFizica::afiseazaDetalii() const {
    cout << "[FIZICA | Stare: " << stare << "] ";
    Carte::afiseazaDetalii();
    cout << endl;
}

// ============================================
// DERIVATE DIN CARTE FIZICA
// ============================================

CarteFictiune::CarteFictiune(string titlu, string autor, string isbn, string gen)
    : CarteFizica(titlu, autor, isbn), gen(gen) {}
void CarteFictiune::afiseazaDetalii() const {
    cout << "[FICTIUNE - " << gen << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

CarteTehnica::CarteTehnica(string titlu, string autor, string isbn, string domeniu)
    : CarteFizica(titlu, autor, isbn), domeniu(domeniu) {}
string CarteTehnica::getDomeniu() const { return domeniu; }
void CarteTehnica::afiseazaDetalii() const {
    cout << "[TEHNICA - " << domeniu << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

Revista::Revista(string titlu, string autor, string isbn, int numar, string luna)
    : CarteFizica(titlu, autor, isbn), numar(numar), luna(luna) {}
int Revista::getNumar() const { return numar; }
string Revista::getLuna() const { return luna; }
void Revista::afiseazaDetalii() const {
    cout << "[REVISTA - Nr." << numar << " " << luna << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

Manga::Manga(string titlu, string autor, string isbn, int volum, string mangaka)
    : CarteFizica(titlu, autor, isbn), volum(volum), mangaka(mangaka) {}
int Manga::getVolum() const { return volum; }
string Manga::getMangaka() const { return mangaka; }
void Manga::afiseazaDetalii() const {
    cout << "[MANGA - Vol." << volum << " | Mangaka: " << mangaka << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

BenziDesonate::BenziDesonate(string titlu, string autor, string isbn, string artist)
    : CarteFizica(titlu, autor, isbn), artist(artist) {}
string BenziDesonate::getArtist() const { return artist; }
void BenziDesonate::afiseazaDetalii() const {
    cout << "[BENZI DESENATE | Artist: " << artist << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

RomanGrafic::RomanGrafic(string titlu, string autor, string isbn, string artist)
    : CarteFizica(titlu, autor, isbn), artist(artist) {}
string RomanGrafic::getArtist() const { return artist; }
void RomanGrafic::afiseazaDetalii() const {
    cout << "[ROMAN GRAFIC | Artist: " << artist << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

Biografie::Biografie(string titlu, string autor, string isbn, string subiect)
    : CarteFizica(titlu, autor, isbn), subiect(subiect) {}
string Biografie::getSubiect() const { return subiect; }
void Biografie::afiseazaDetalii() const {
    cout << "[BIOGRAFIE - " << subiect << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

Stiinta::Stiinta(string titlu, string autor, string isbn, string ramura)
    : CarteFizica(titlu, autor, isbn), ramura(ramura) {}
string Stiinta::getRamura() const { return ramura; }
void Stiinta::afiseazaDetalii() const {
    cout << "[STIINTA - " << ramura << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

Istorie::Istorie(string titlu, string autor, string isbn, string perioada)
    : CarteFizica(titlu, autor, isbn), perioada(perioada) {}
string Istorie::getPerioada() const { return perioada; }
void Istorie::afiseazaDetalii() const {
    cout << "[ISTORIE - " << perioada << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

Filozofie::Filozofie(string titlu, string autor, string isbn, string curent)
    : CarteFizica(titlu, autor, isbn), curent(curent) {}
string Filozofie::getCurent() const { return curent; }
void Filozofie::afiseazaDetalii() const {
    cout << "[FILOZOFIE - " << curent << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

Manual::Manual(string titlu, string autor, string isbn,
               string materie, string nivel)
    : CarteFizica(titlu, autor, isbn), materie(materie), nivel(nivel) {}
string Manual::getMaterie() const { return materie; }
string Manual::getNivel() const { return nivel; }
void Manual::afiseazaDetalii() const {
    cout << "[MANUAL - " << materie << " | " << nivel << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}

Enciclopedie::Enciclopedie(string titlu, string autor, string isbn, string domeniu)
    : CarteFizica(titlu, autor, isbn), domeniu(domeniu) {}
string Enciclopedie::getDomeniu() const { return domeniu; }
void Enciclopedie::afiseazaDetalii() const {
    cout << "[ENCICLOPEDIE - " << domeniu << "] ";
    Carte::afiseazaDetalii();
    cout << " | Stare: " << getStare() << endl;
}