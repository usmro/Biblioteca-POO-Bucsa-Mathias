#include "Carte.h"
#include <iostream>

Carte::Carte(string titlu, string autor, string isbn)
    : titlu(titlu), autor(autor), isbn(isbn), disponibila(true) {}

string Carte::getTitlu() const { return titlu; }
string Carte::getAutor() const { return autor; }
string Carte::getIsbn() const { return isbn; }
bool Carte::esteDisponibila() const { return disponibila; }
void Carte::setDisponibila(bool status) { disponibila = status; }

void Carte::afiseazaDetalii() const {
    cout << "Carte: " << titlu << " | Autor: " << autor
         << " | ISBN: " << isbn
         << " | " << (disponibila ? "Disponibila" : "Imprumutata") << endl;
}

CarteFictiune::CarteFictiune(string titlu, string autor, string isbn, string gen)
    : Carte(titlu, autor, isbn), gen(gen) {}

void CarteFictiune::afiseazaDetalii() const {
    cout << "[FICTIUNE - " << gen << "] ";
    Carte::afiseazaDetalii();
}

CarteTehnica::CarteTehnica(string titlu, string autor, string isbn, string domeniu)
    : Carte(titlu, autor, isbn), domeniu(domeniu) {}

void CarteTehnica::afiseazaDetalii() const {
    cout << "[TEHNIC - " << domeniu << "] ";
    Carte::afiseazaDetalii();
}
// Implementare CarteDigitala
CarteDigitala::CarteDigitala(string titlu, string autor, string isbn,
                              string format, string linkDownload, double dimensiuneMB)
    : Carte(titlu, autor, isbn), format(format), 
      linkDownload(linkDownload), dimensiuneMB(dimensiuneMB) {}

string CarteDigitala::getFormat() const { return format; }
string CarteDigitala::getLinkDownload() const { return linkDownload; }
double CarteDigitala::getDimensiuneMB() const { return dimensiuneMB; }

void CarteDigitala::afiseazaDetalii() const {
    cout << "[DIGITAL - " << format << " | "
         << dimensiuneMB << "MB] ";
    Carte::afiseazaDetalii();
    cout << "  Download: " << linkDownload << endl;
}

// Implementare Audiobook
Audiobook::Audiobook(string titlu, string autor, string isbn,
                     string narator, int durataMinte)
    : Carte(titlu, autor, isbn), narator(narator), durataMinte(durataMinte) {}

string Audiobook::getNarator() const { return narator; }
int Audiobook::getDurataMinute() const { return durataMinte; }

string Audiobook::getDurataFormata() const {
    int ore = durataMinte / 60;
    int minute = durataMinte % 60;
    return to_string(ore) + "h " + to_string(minute) + "min";
}

void Audiobook::afiseazaDetalii() const {
    cout << "[AUDIOBOOK | " << getDurataFormata() 
         << " | Narator: " << narator << "] ";
    Carte::afiseazaDetalii();
}