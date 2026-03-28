#include <iostream>
#include "Biblioteca.h"

int main() {
    Biblioteca biblioteca;

    biblioteca.adaugaCarte(new CarteFictiune("Dune", "Frank Herbert", "ISBN001", "SF"));
    biblioteca.adaugaCarte(new CarteTehnica("Clean Code", "Robert Martin", "ISBN002", "Programare"));
    biblioteca.adaugaCarte(new CarteFictiune("1984", "George Orwell", "ISBN003", "Distopie"));

    biblioteca.adaugaUtilizator(Utilizator("Ion Popescu", 1));
    biblioteca.adaugaUtilizator(Utilizator("Maria Ionescu", 2));

    biblioteca.afiseazaCarti();

    // Imprumut normal - 14 zile
    biblioteca.imprumutaCarte(1, "ISBN001", 14);
    // Imprumut urgent - doar 7 zile
    biblioteca.imprumutaCarte(2, "ISBN002", 7);

    // Afisam raportul de penalitati
    // (acum nu sunt intarzieri, dar sistemul e functional)
    biblioteca.afiseazaRaportPenalitati();

    biblioteca.returneazaCarte(1, "ISBN001");
    biblioteca.afiseazaCarti();

    return 0;
}