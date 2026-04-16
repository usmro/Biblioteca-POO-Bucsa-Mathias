#include <iostream>
#include "Biblioteca.h"

int main() {
    Biblioteca biblioteca;

    // Carti fizice
    biblioteca.adaugaCarte(new CarteFictiune("Dune", "Frank Herbert", "ISBN001", "SF"));
    biblioteca.adaugaCarte(new CarteTehnica("Clean Code", "Robert Martin", "ISBN002", "Programare"));

    // Carti digitale
    biblioteca.adaugaCarte(new CarteDigitala(
        "Design Patterns", "Gang of Four", "ISBN003",
        "PDF", "https://biblioteca.ro/design-patterns.pdf", 12.5
    ));
    biblioteca.adaugaCarte(new CarteDigitala(
        "The Pragmatic Programmer", "David Thomas", "ISBN004",
        "EPUB", "https://biblioteca.ro/pragmatic.epub", 8.2
    ));

    // Audiobook-uri
    biblioteca.adaugaCarte(new Audiobook(
        "Atomic Habits", "James Clear", "ISBN005",
        "Mike Chamberlain", 270  // 4h 30min
    ));
    biblioteca.adaugaCarte(new Audiobook(
        "Sapiens", "Yuval Noah Harari", "ISBN006",
        "Derek Perkins", 915  // 15h 15min
    ));

    // Utilizatori
    biblioteca.adaugaUtilizator(Utilizator("Ion Popescu", 1));
    biblioteca.adaugaUtilizator(Utilizator("Maria Ionescu", 2));

    // Afisam tot catalogul - polimorfism in actiune!
    biblioteca.afiseazaCarti();

    // Imprumuturi
    biblioteca.imprumutaCarte(1, "ISBN001", 14);
    biblioteca.imprumutaCarte(2, "ISBN003", 30); // carte digitala - 30 zile
    biblioteca.imprumutaCarte(1, "ISBN005", 7);  // audiobook - 7 zile

    biblioteca.afiseazaCarti();
    biblioteca.afiseazaRaportPenalitati();

    return 0;
}