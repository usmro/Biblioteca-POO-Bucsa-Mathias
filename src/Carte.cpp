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