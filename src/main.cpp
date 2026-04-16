#include <iostream>
#include "Biblioteca.h"
#include "Autentificare.h"
#include "utils/FisierHelper.h"

int main() {
    const string FISIER_UTILIZATORI = "date/utilizatori.txt";

    Biblioteca biblioteca;

    // Incarcam utilizatorii din fisier
    vector<Utilizator> utilizatori = 
        FisierHelper::incarcaUtilizatori(FISIER_UTILIZATORI);

    // Sistem autentificare
    Autentificare auth(utilizatori);

    // Cream conturi noi
    auth.creeazaCont("Ion Popescu", "ion123", "parola123");
    auth.creeazaCont("Maria Ionescu", "maria99", "test456");
    auth.creeazaCont("ion123", "ion123", "altaparola"); // username duplicat!

    // Salvam utilizatorii in fisier
    FisierHelper::salveazaUtilizatori(utilizatori, FISIER_UTILIZATORI);

    // Testam login
    cout << "\n--- Testare Login ---" << endl;
    Utilizator* user1 = auth.login("ion123", "parola123");   // corect
    Utilizator* user2 = auth.login("maria99", "gresit");     // parola gresita
    Utilizator* user3 = auth.login("inexistent", "ceva");    // user inexistent

    if (user1) cout << "Bun venit, " << user1->getNume() << "!" << endl;

    // Adaugam carti
    biblioteca.adaugaCarte(new CarteFictiune("Dune", "Frank Herbert", 
                                              "ISBN001", "SF"));
    biblioteca.adaugaCarte(new CarteDigitala("Clean Code", "Robert Martin",
                                              "ISBN002", "PDF", 
                                              "https://lib.ro/clean.pdf", 8.5));
    biblioteca.adaugaCarte(new Audiobook("Atomic Habits", "James Clear",
                                          "ISBN003", "Mike Chamberlain", 270));
    biblioteca.afiseazaCarti();

    return 0;
}