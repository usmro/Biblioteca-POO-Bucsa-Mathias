#include "Meniu.h"
#include "../utils/Exceptii.h"
#include <iostream>
#include <limits>
using namespace std;

Meniu::Meniu(Biblioteca& biblioteca, Autentificare& auth,
             vector<Utilizator>& utilizatori)
    : biblioteca(biblioteca), auth(auth), utilizatori(utilizatori),
      angajatCurent(nullptr), director(nullptr), bibliotecar(nullptr) {}

Meniu::~Meniu() {
    delete director;
    delete bibliotecar;
}

void Meniu::clearScreen() {
    cout << "\033[2J\033[1;1H";
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

void Meniu::afiseazaMenuBibliotecar() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "   PANOU BIBLIOTECAR - " << angajatCurent->getNume() << endl;
    cout << "========================================" << endl;
    cout << "  1. Vezi catalog carti" << endl;
    cout << "  2. Adauga carte noua" << endl;
    cout << "  3. Elimina carte din catalog" << endl;
    cout << "  4. Vezi toti utilizatorii" << endl;
    cout << "  5. Raport penalitati" << endl;
    cout << "  6. Logout" << endl;
    cout << "========================================" << endl;
    cout << "Alege optiunea: ";
}

void Meniu::afiseazaMenuDirector() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "    PANOU DIRECTOR - " << angajatCurent->getNume() << endl;
    cout << "========================================" << endl;
    cout << "  1. Vezi catalog carti" << endl;
    cout << "  2. Raport penalitati" << endl;
    cout << "  3. Raport financiar complet" << endl;
    cout << "  4. Acorda bonus angajat" << endl;
    cout << "  5. Vezi echipa" << endl;
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
    cout << "Username: "; cin >> username;
    cout << "Parola: "; cin >> parola;

    if (director && director->getUsername() == username &&
        director->verificaParola(parola)) {
        angajatCurent = director;
        cout << "\nBun venit, Director " << director->getNume() << "!" << endl;
        asteaptaEnter();
        return;
    }

    if (bibliotecar && bibliotecar->getUsername() == username &&
        bibliotecar->verificaParola(parola)) {
        angajatCurent = bibliotecar;
        cout << "\nBun venit, " << bibliotecar->getNume() << "!" << endl;
        asteaptaEnter();
        return;
    }

    try {
        Utilizator* authUser = auth.login(username, parola);
        utilizatorCurent = biblioteca.getUtilizator(authUser->getId());
        if (!utilizatorCurent)
            cout << "\nEroare interna!" << endl;
    } catch (BibliotecaException& e) {
        cout << "\nEroare: " << e.what() << endl;
    }
    asteaptaEnter();
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
    cout << "Username: "; cin >> username;
    cout << "Parola: "; cin >> parola;

    try {
        auth.creeazaCont(nume, username, parola);
        biblioteca.adaugaUtilizator(utilizatori.back());
        FisierHelper::salveazaUtilizatori(utilizatori, FISIER_UTILIZATORI);
        cout << "\nCont creat cu succes!" << endl;
    } catch (BibliotecaException& e) {
        cout << "\nEroare: " << e.what() << endl;
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
    cout << "ISBN-ul cartii dorite: "; cin >> isbn;
    cout << "Numar de zile (max 30): "; cin >> zile;

    if (zile < 1) zile = 1;
    if (zile > 30) zile = 30;

    try {
        biblioteca.imprumutaCarte(utilizatorCurent->getId(), isbn, zile);
        cout << "\nCarte imprumutata cu succes!" << endl;
    } catch (BibliotecaException& e) {
        cout << "\nEroare: " << e.what() << endl;
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
    cout << "\nISBN-ul cartii de returnat: "; cin >> isbn;

    try {
        biblioteca.returneazaCarte(utilizatorCurent->getId(), isbn);
        cout << "\nCarte returnata cu succes!" << endl;
    } catch (BibliotecaException& e) {
        cout << "\nEroare: " << e.what() << endl;
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

void Meniu::handleAdaugaCarte() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "          ADAUGA CARTE NOUA             " << endl;
    cout << "========================================" << endl;
    cout << "Tip: 1=Fictiune, 2=Tehnica, 3=Digitala, 4=Audiobook" << endl;
    int tip; cin >> tip;

    string titlu, autor, isbn;
    cout << "Titlu: "; cin.ignore(); getline(cin, titlu);
    cout << "Autor: "; getline(cin, autor);
    cout << "ISBN: "; cin >> isbn;

    Carte* carte = nullptr;
    if (tip == 1) {
        string gen;
        cout << "Gen: "; cin >> gen;
        carte = new CarteFictiune(titlu, autor, isbn, gen);
    } else if (tip == 2) {
        string domeniu;
        cout << "Domeniu: "; cin >> domeniu;
        carte = new CarteTehnica(titlu, autor, isbn, domeniu);
    } else if (tip == 3) {
        string format, link; double dim;
        cout << "Format (PDF/EPUB): "; cin >> format;
        cout << "Link: "; cin >> link;
        cout << "Dimensiune MB: "; cin >> dim;
        carte = new CarteDigitala(titlu, autor, isbn, format, link, dim);
    } else if (tip == 4) {
        string narator; int durata;
        cout << "Narator: "; cin >> narator;
        cout << "Durata (minute): "; cin >> durata;
        carte = new Audiobook(titlu, autor, isbn, narator, durata);
    }

    if (carte) {
        bibliotecar->adaugaCarte(biblioteca, carte);
        cout << "\nCarte adaugata cu succes!" << endl;
    }
    asteaptaEnter();
}

void Meniu::handleEliminaCarte() {
    clearScreen();
    biblioteca.afiseazaCarti();
    string isbn;
    cout << "ISBN-ul cartii de eliminat: "; cin >> isbn;
    try {
        bibliotecar->eliminaCarte(biblioteca, isbn);
    } catch (BibliotecaException& e) {
        cout << "\nEroare: " << e.what() << endl;
    }
    asteaptaEnter();
}

void Meniu::handleVeziUtilizatori() {
    clearScreen();
    bibliotecar->afiseazaTotiUtilizatorii(utilizatori);
    asteaptaEnter();
}

void Meniu::handleRaportFinanciar() {
    clearScreen();
    director->afiseazaRaportFinanciar(biblioteca, utilizatori);
    asteaptaEnter();
}

void Meniu::handleAcordaBonus() {
    clearScreen();
    director->afiseazaEchipa();
    cout << "\nAcest feature va fi disponibil in versiunea urmatoare." << endl;
    asteaptaEnter();
}

void Meniu::handleVeziEchipa() {
    clearScreen();
    cout << "\n=== Echipa Bibliotecii ===" << endl;
    director->afiseazaEchipa();
    asteaptaEnter();
}

void Meniu::ruleaza() {
    utilizatori = FisierHelper::incarcaUtilizatori(FISIER_UTILIZATORI);
    for (auto& u : utilizatori)
        biblioteca.adaugaUtilizator(u);

    director = new Director("Ana Popescu", 100, "director", "director123", 8000);
    bibliotecar = new Bibliotecar("Mihai Ionescu", 101, "bibliotecar",
                                   "biblio123", 4000);
    director->adaugaAngajat(bibliotecar);

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

    while (true) {
        while (angajatCurent != nullptr &&
               angajatCurent->getRol() == RolAngajat::DIRECTOR) {
            afiseazaMenuDirector();
            cin >> optiune;
            switch (optiune) {
                case 1: handleVezeCatalog(); break;
                case 2: handleRaportPenalitati(); break;
                case 3: handleRaportFinanciar(); break;
                case 4: handleAcordaBonus(); break;
                case 5: handleVeziEchipa(); break;
                case 6:
                    cout << "La revedere, " << angajatCurent->getNume() << "!" << endl;
                    angajatCurent = nullptr;
                    asteaptaEnter();
                    break;
                default:
                    cout << "Optiune invalida!" << endl;
                    asteaptaEnter();
            }
        }

        while (angajatCurent != nullptr &&
               angajatCurent->getRol() == RolAngajat::BIBLIOTECAR) {
            afiseazaMenuBibliotecar();
            cin >> optiune;
            switch (optiune) {
                case 1: handleVezeCatalog(); break;
                case 2: handleAdaugaCarte(); break;
                case 3: handleEliminaCarte(); break;
                case 4: handleVeziUtilizatori(); break;
                case 5: handleRaportPenalitati(); break;
                case 6:
                    cout << "La revedere, " << angajatCurent->getNume() << "!" << endl;
                    angajatCurent = nullptr;
                    asteaptaEnter();
                    break;
                default:
                    cout << "Optiune invalida!" << endl;
                    asteaptaEnter();
            }
        }

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