#include "Meniu.h"
#include <iostream>
#include <limits>
using namespace std;

Meniu::Meniu(Biblioteca& biblioteca, Autentificare& auth,
             vector<Utilizator>& utilizatori)
    : biblioteca(biblioteca), auth(auth), utilizatori(utilizatori) {}

void Meniu::clearScreen() {
    cout << "\033[2J\033[1;1H"; // ANSI escape code - curata ecranul
}

void Meniu::asteaptaEnter() {
    cout << "\nApasa ENTER pentru a continua...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void Meniu::afiseazaMenuPrincipal() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "        BIBLIOTECA VIRTUALA v1.0        " << endl;
    cout << "========================================" << endl;
    cout << "  1. Login" << endl;
    cout << "  2. Creare cont nou" << endl;
    cout << "  3. Iesire" << endl;
    cout << "========================================" << endl;
    cout << "Alege optiunea: ";
}

void Meniu::afiseazaMenuUtilizator() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "  Bun venit, " << utilizatorCurent->getNume() << "!" << endl;
    cout << "========================================" << endl;
    cout << "  1. Vezi catalog carti" << endl;
    cout << "  2. Imprumuta o carte" << endl;
    cout << "  3. Returneaza o carte" << endl;
    cout << "  4. Imprumuturile mele" << endl;
    cout << "  5. Raport penalitati" << endl;
    cout << "  6. Logout" << endl;
    cout << "========================================" << endl;
    cout << "Alege optiunea: ";
}

void Meniu::handleLogin() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "                LOGIN                   " << endl;
    cout << "========================================" << endl;

    string username, parola;
    cout << "Username: ";
    cin >> username;
    cout << "Parola: ";
    cin >> parola;

    utilizatorCurent = auth.login(username, parola);

    if (!utilizatorCurent) {
        cout << "\nUsername sau parola incorecta!" << endl;
        asteaptaEnter();
    }
}

void Meniu::handleCreareCont() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "            CREARE CONT NOU             " << endl;
    cout << "========================================" << endl;

    string nume, username, parola;
    cout << "Nume complet: ";
    cin.ignore();
    getline(cin, nume);
    cout << "Username: ";
    cin >> username;
    cout << "Parola: ";
    cin >> parola;

    if (auth.creeazaCont(nume, username, parola)) {
    // Adaugam si in biblioteca ultimul utilizator creat
    biblioteca.adaugaUtilizator(utilizatori.back());
    FisierHelper::salveazaUtilizatori(utilizatori, FISIER_UTILIZATORI);
    cout << "\nCont creat cu succes! Te poti autentifica acum." << endl;
    }
    asteaptaEnter();
}

void Meniu::handleVezeCatalog() {
    clearScreen();
    biblioteca.afiseazaCarti();
    asteaptaEnter();
}

void Meniu::handleImprumut() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "           IMPRUMUT CARTE               " << endl;
    cout << "========================================" << endl;

    biblioteca.afiseazaCarti();

    string isbn;
    int zile;
    cout << "ISBN-ul cartii dorite: ";
    cin >> isbn;
    cout << "Numar de zile pentru imprumut (max 30): ";
    cin >> zile;

    if (zile < 1) zile = 1;
    if (zile > 30) zile = 30;

    if (biblioteca.imprumutaCarte(utilizatorCurent->getId(), isbn, zile)) {
        cout << "\nCarte imprumutata cu succes!" << endl;
    } else {
        cout << "\nCartea nu este disponibila!" << endl;
    }
    asteaptaEnter();
}

void Meniu::handleReturnare() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "           RETURNARE CARTE              " << endl;
    cout << "========================================" << endl;

    utilizatorCurent->afiseazaImprumuturi();

    string isbn;
    cout << "\nISBN-ul cartii de returnat: ";
    cin >> isbn;

    if (biblioteca.returneazaCarte(utilizatorCurent->getId(), isbn)) {
        cout << "\nCarte returnata cu succes!" << endl;
    } else {
        cout << "\nEroare la returnare!" << endl;
    }
    asteaptaEnter();
}

void Meniu::handleImprumuturileMele() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "         IMPRUMUTURILE MELE             " << endl;
    cout << "========================================" << endl;
    utilizatorCurent->afiseazaImprumuturi();
    asteaptaEnter();
}

void Meniu::handleRaportPenalitati() {
    clearScreen();
    biblioteca.afiseazaRaportPenalitati();
    asteaptaEnter();
}

void Meniu::ruleaza() {
    // Incarcam utilizatorii din fisier la pornire
    utilizatori = FisierHelper::incarcaUtilizatori(FISIER_UTILIZATORI);

    // Sincronizam utilizatorii cu biblioteca
    for (auto& u : utilizatori)
        biblioteca.adaugaUtilizator(u);

    // Adaugam carti demo in biblioteca
    biblioteca.adaugaCarte(new CarteFictiune("Dune", "Frank Herbert",
                                              "ISBN001", "SF"));
    biblioteca.adaugaCarte(new CarteTehnica("Clean Code", "Robert Martin",
                                             "ISBN002", "Programare"));
    biblioteca.adaugaCarte(new CarteDigitala("Design Patterns", "Gang of Four",
                                              "ISBN003", "PDF",
                                              "https://lib.ro/dp.pdf", 12.5));
    biblioteca.adaugaCarte(new Audiobook("Atomic Habits", "James Clear",
                                          "ISBN004", "Mike Chamberlain", 270));

    int optiune;

    // Loop principal - ruleaza cat timp nu iese
    while (true) {

        // Meniu dupa login
        while (utilizatorCurent != nullptr) {
            afiseazaMenuUtilizator();
            cin >> optiune;

            switch (optiune) {
                case 1: handleVezeCatalog(); break;
                case 2: handleImprumut(); break;
                case 3: handleReturnare(); break;
                case 4: handleImprumuturileMele(); break;
                case 5: handleRaportPenalitati(); break;
                case 6:
                    cout << "La revedere, " 
                         << utilizatorCurent->getNume() << "!" << endl;
                    utilizatorCurent = nullptr;
                    asteaptaEnter();
                    break;
                default:
                    cout << "Optiune invalida!" << endl;
                    asteaptaEnter();
            }
        }

        // Meniu principal
        afiseazaMenuPrincipal();
        cin >> optiune;

        switch (optiune) {
            case 1: handleLogin(); break;
            case 2: handleCreareCont(); break;
            case 3:
                cout << "\nLa revedere!" << endl;
                return;
            default:
                cout << "Optiune invalida!" << endl;
                asteaptaEnter();
        }
    }
}