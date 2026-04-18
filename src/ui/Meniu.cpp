#include "Meniu.h"
#include "../utils/Exceptii.h"
#include <iostream>
#include <limits>
#include "../utils/Criptare.h"
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

int Meniu::citesteOptiune() {
    int optiune;
    while (!(cin >> optiune)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Optiune invalida! Introdu un numar: ";
    }
    return optiune;
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
    cout << "  2. Cauta / Filtreaza carti" << endl;
    cout << "  3. Imprumuta o carte" << endl;
    cout << "  4. Returneaza o carte" << endl;
    cout << "  5. Imprumuturile mele" << endl;
    cout << "  6. Raport penalitati" << endl;
    cout << "  7. Logout" << endl;
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
    cout << "  5. Sterge cont utilizator" << endl;
    cout << "  6. Raport penalitati" << endl;
    cout << "  7. Logout" << endl;
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
    cout << "  4. Gestionare angajati" << endl;
    cout << "  5. Logout" << endl;
    cout << "========================================" << endl;
    cout << "Alege optiunea: ";
}

void Meniu::afiseazaMenuCautare() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "         CAUTARE SI FILTRARE            " << endl;
    cout << "========================================" << endl;
    cout << "  1. Cauta dupa autor" << endl;
    cout << "  2. Cauta dupa titlu" << endl;
    cout << "  3. Vezi doar cartile disponibile" << endl;
    cout << "  4. Vezi doar cartile imprumutate" << endl;
    cout << "  5. Filtreaza dupa tip" << endl;
    cout << "  6. Inapoi" << endl;
    cout << "========================================" << endl;
    cout << "Alege optiunea: ";
}

void Meniu::handleCautare() {
    int optiune;
    while (true) {
        afiseazaMenuCautare();
        optiune = citesteOptiune();

        if (optiune == 6) return;

        clearScreen();
        switch (optiune) {
            case 1: {
                string autor;
                cout << "Introdu autorul: ";
                cin.ignore();
                getline(cin, autor);
                biblioteca.cautaDupaAutor(autor);
                break;
            }
            case 2: {
                string titlu;
                cout << "Introdu titlul: ";
                cin.ignore();
                getline(cin, titlu);
                biblioteca.cautaDupaTitlu(titlu);
                break;
            }
            case 3:
                biblioteca.filtreazaDupaDisponibilitate(true);
                break;
            case 4:
                biblioteca.filtreazaDupaDisponibilitate(false);
                break;
            case 5: {
                cout << "Tip: 1=Fictiune, 2=Tehnica, 3=Digital, 4=Audiobook" << endl;
                int tip; cin >> tip;
                string tipStr;
                if (tip == 1) tipStr = "FICTIUNE";
                else if (tip == 2) tipStr = "TEHNICA";
                else if (tip == 3) tipStr = "DIGITAL";
                else if (tip == 4) tipStr = "AUDIOBOOK";
                else { cout << "Tip invalid!" << endl; break; }
                biblioteca.filtreazaDupaTip(tipStr);
                break;
            }
            default:
                cout << "Optiune invalida!" << endl;
        }
        asteaptaEnter();
    }
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
    handleFiltrareCatalog();
    asteaptaEnter();
}

void Meniu::handleImprumut() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "           IMPRUMUT CARTE               " << endl;
    cout << "========================================" << endl;

    // Cautare inainte de imprumut
    cout << "Vrei sa cauti o carte inainte? (1=Da, 0=Nu): ";
    int optCautare; cin >> optCautare;
    if (optCautare == 1) {
        handleFiltrareCatalog();
        asteaptaEnter();
    }

    // Afisam toate cartile disponibile
    biblioteca.filtreazaDupaDisponibilitate(true);

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
    biblioteca.salveazaCarti(FISIER_CARTI);
    asteaptaEnter();
}

void Meniu::handleFiltrareCatalog() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "            CATALOG CARTI               " << endl;
    cout << "========================================" << endl;
    cout << "  1. Vezi toate cartile" << endl;
    cout << "  2. Cauta dupa autor" << endl;
    cout << "  3. Cauta dupa titlu" << endl;
    cout << "  4. Doar disponibile" << endl;
    cout << "  5. Doar imprumutate" << endl;
    cout << "  6. Filtreaza dupa tip" << endl;
    cout << "========================================" << endl;
    cout << "Alege optiunea: ";

    int optiune;
    optiune = citesteOptiune();
    clearScreen();

    switch (optiune) {
        case 1:
            biblioteca.afiseazaCarti();
            break;
        case 2: {
            string autor;
            cout << "Autor: ";
            cin.ignore();
            getline(cin, autor);
            biblioteca.cautaDupaAutor(autor);
            break;
        }
        case 3: {
            string titlu;
            cout << "Titlu: ";
            cin.ignore();
            getline(cin, titlu);
            biblioteca.cautaDupaTitlu(titlu);
            break;
        }
        case 4:
            biblioteca.filtreazaDupaDisponibilitate(true);
            break;
        case 5:
            biblioteca.filtreazaDupaDisponibilitate(false);
            break;
        case 6: {
            cout << "1=Fictiune  2=Tehnica  3=Digital  4=Audiobook" << endl;
            cout << "Alege tipul: ";
            int tip; cin >> tip;
            string tipStr;
            if (tip == 1) tipStr = "FICTIUNE";
            else if (tip == 2) tipStr = "TEHNICA";
            else if (tip == 3) tipStr = "DIGITAL";
            else if (tip == 4) tipStr = "AUDIOBOOK";
            else { cout << "Tip invalid!" << endl; break; }
            biblioteca.filtreazaDupaTip(tipStr);
            break;
        }
        default:
            cout << "Optiune invalida!" << endl;
    }
}

void Meniu::handleEliminaCarte() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "          ELIMINA CARTE                 " << endl;
    cout << "========================================" << endl;

    // Cautare inainte de eliminare
    cout << "Vrei sa cauti o carte inainte? (1=Da, 0=Nu): ";
    int optCautare; cin >> optCautare;
    if (optCautare == 1) {
        handleFiltrareCatalog();
        asteaptaEnter();
    }

    // Afisam tot catalogul
    biblioteca.afiseazaCarti();

    string isbn;
    cout << "ISBN-ul cartii de eliminat: "; cin >> isbn;
    try {
        bibliotecar->eliminaCarte(biblioteca, isbn);
        biblioteca.salveazaCarti(FISIER_CARTI);
        cout << "\nCarte eliminata cu succes!" << endl;
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

    // Incarcam cartile din fisier
    biblioteca.incarcaCarti(FISIER_CARTI);

    // Daca nu exista carti salvate, adaugam demo
    // (doar prima rulare)
    if (biblioteca.getNumarCarti() == 0) {
        biblioteca.adaugaCarte(new CarteFictiune("Dune", "Frank Herbert",
                                                  "ISBN001", "SF"));
        biblioteca.adaugaCarte(new CarteTehnica("Clean Code", "Robert Martin",
                                                 "ISBN002", "Programare"));
        biblioteca.adaugaCarte(new CarteDigitala("Design Patterns", "Gang of Four",
                                                  "ISBN003", "PDF",
                                                  "https://lib.ro/dp.pdf", 12.5));
        biblioteca.adaugaCarte(new Audiobook("Atomic Habits", "James Clear",
                                              "ISBN004", "Mike Chamberlain", 270));
        // Salvam cartile demo
        biblioteca.salveazaCarti(FISIER_CARTI);
    }

    // Incarcam angajatii din fisier
auto [dirLoaded, biblLoaded] = FisierHelper::incarcaAngajati(FISIER_ANGAJATI);

if (dirLoaded && biblLoaded) {
    // Incarcati din fisier
    director = dirLoaded;
    bibliotecar = biblLoaded;
    director->adaugaAngajat(bibliotecar);
    cout << "[LOG] Angajati incarcati din fisier." << endl;
} else {
    // Prima rulare - cream angajatii default
    director = new Director("Ana Popescu", 100, "director", "director123", 8000);
    bibliotecar = new Bibliotecar("Mihai Ionescu", 101, "bibliotecar",
                                   "biblio123", 4000);
    director->adaugaAngajat(bibliotecar);
    // Salvam pentru data viitoare
    FisierHelper::salveazaAngajati(director, bibliotecar, FISIER_ANGAJATI);
}

    // ... restul codului rămâne la fel
    int optiune;

    while (true) {
        while (angajatCurent != nullptr &&
               angajatCurent->getRol() == RolAngajat::DIRECTOR) {
            afiseazaMenuDirector();
            optiune = citesteOptiune();
            switch (optiune) {
                case 1: handleVezeCatalog(); break;
                case 2: handleRaportPenalitati(); break;
                case 3: handleRaportFinanciar(); break;
                case 4: handleGestionareAngajati(); break;
                case 5:
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
            optiune = citesteOptiune();
            switch (optiune) {
                case 1: handleVezeCatalog(); break;
                case 2: handleAdaugaCarte(); break;
                case 3: handleEliminaCarte(); break;
                case 4: handleVeziUtilizatori(); break;
                case 5: handleStergeUtilizator(); break;
                case 6: handleRaportPenalitati(); break;
                case 7:
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
            optiune = citesteOptiune();
            switch (optiune) {
                case 1: handleVezeCatalog(); break;
                case 2: handleCautare(); break;
                case 3: handleImprumut(); break;
                case 4: handleReturnare(); break;
                case 5: handleImprumuturileMele(); break;
                case 6: handleRaportPenalitati(); break;
                case 7:
                    cout << "La revedere, " << utilizatorCurent->getNume() << "!" << endl;
                    utilizatorCurent = nullptr;
                    asteaptaEnter();
                    break;
                default:
                    cout << "Optiune invalida!" << endl;
                    asteaptaEnter();
            }
        }

        afiseazaMenuPrincipal();
        optiune = citesteOptiune();
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

void Meniu::handleStergeUtilizator() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "          STERGE UTILIZATOR             " << endl;
    cout << "========================================" << endl;

    bibliotecar->afiseazaTotiUtilizatorii(utilizatori);

    cout << "\nID-ul utilizatorului de sters: ";
    int id; cin >> id;

    try {
        bibliotecar->stergeUtilizator(biblioteca, utilizatori,
                                       FISIER_UTILIZATORI, id);
        cout << "\nUtilizator sters cu succes!" << endl;
    } catch (BibliotecaException& e) {
        cout << "\nEroare: " << e.what() << endl;
    }
    asteaptaEnter();
}

void Meniu::handleGestionareAngajati() {
    int optiune;
    while (true) {
        clearScreen();
        cout << "========================================" << endl;
        cout << "         GESTIONARE ANGAJATI            " << endl;
        cout << "========================================" << endl;
        cout << "  1. Vezi echipa" << endl;
        cout << "  2. Adauga bibliotecar nou" << endl;
        cout << "  3. Schimba parola unui angajat" << endl;
        cout << "  4. Schimba numele unui angajat" << endl;
        cout << "  5. Acorda bonus" << endl;
        cout << "  6. Inapoi" << endl;
        cout << "========================================" << endl;
        cout << "Alege optiunea: ";
        optiune = citesteOptiune();

        if (optiune == 6) return;

        clearScreen();
        switch (optiune) {

            case 1: {
                director->afiseazaEchipa();
                asteaptaEnter();
                break;
            }

            case 2: {
                cout << "========================================" << endl;
                cout << "        ADAUGA BIBLIOTECAR NOU          " << endl;
                cout << "========================================" << endl;
                string nume, username, parola;
                double salariu;
                cout << "Nume complet: ";
                cin.ignore();
                getline(cin, nume);
                cout << "Username: "; cin >> username;
                cout << "Parola: "; cin >> parola;
                cout << "Salariu: "; cin >> salariu;

                // ID nou = 200 + numarul de angajati existenti
                int idNou = 200 + (int)rand() % 1000;
                Bibliotecar* bNou = new Bibliotecar(nume, idNou,
                                                     username, parola, salariu);
                director->adaugaAngajat(bNou);
                FisierHelper::salveazaAngajati(director, bibliotecar,
                                               FISIER_ANGAJATI);
                cout << "\nBibliotecar adaugat cu succes!" << endl;
                asteaptaEnter();
                break;
            }

            case 3: {
                cout << "========================================" << endl;
                cout << "          SCHIMBA PAROLA                " << endl;
                cout << "========================================" << endl;
                director->afiseazaEchipa();
                cout << "\n1. Schimba parola Director" << endl;
                cout << "2. Schimba parola Bibliotecar" << endl;
                cout << "Alege: ";
                int cine = citesteOptiune();

                string parolaNoua;
                cout << "Parola noua: "; cin >> parolaNoua;

                if (cine == 1) {
                    director->setParolaCriptata(
                        Criptare::sha256(parolaNoua));
                    cout << "\nParola Director schimbata!" << endl;
                } else if (cine == 2) {
                    bibliotecar->setParolaCriptata(
                        Criptare::sha256(parolaNoua));
                    cout << "\nParola Bibliotecar schimbata!" << endl;
                }
                FisierHelper::salveazaAngajati(director, bibliotecar,
                                               FISIER_ANGAJATI);
                asteaptaEnter();
                break;
            }

            case 4: {
                cout << "========================================" << endl;
                cout << "          SCHIMBA NUMELE                " << endl;
                cout << "========================================" << endl;
                director->afiseazaEchipa();
                cout << "\n1. Schimba numele Director" << endl;
                cout << "2. Schimba numele Bibliotecar" << endl;
                cout << "Alege: ";
                int cine = citesteOptiune();

                string numeNou;
                cout << "Numele nou: ";
                cin.ignore();
                getline(cin, numeNou);

                if (cine == 1) {
                    director->setNume(numeNou);
                    cout << "\nNume Director schimbat!" << endl;
                } else if (cine == 2) {
                    bibliotecar->setNume(numeNou);
                    cout << "\nNume Bibliotecar schimbat!" << endl;
                }
                FisierHelper::salveazaAngajati(director, bibliotecar,
                                               FISIER_ANGAJATI);
                asteaptaEnter();
                break;
            }

            case 5: {
                cout << "========================================" << endl;
                cout << "            ACORDA BONUS                " << endl;
                cout << "========================================" << endl;
                director->afiseazaEchipa();
                cout << "\n1. Bonus Director" << endl;
                cout << "2. Bonus Bibliotecar" << endl;
                cout << "Alege: ";
                int cine = citesteOptiune();

                double bonus;
                cout << "Suma bonus (lei): "; cin >> bonus;

                if (cine == 1) {
                    director->acordaBonus(director, bonus);
                } else if (cine == 2) {
                    director->acordaBonus(bibliotecar, bonus);
                }
                FisierHelper::salveazaAngajati(director, bibliotecar,
                                               FISIER_ANGAJATI);
                asteaptaEnter();
                break;
            }

            default:
                cout << "Optiune invalida!" << endl;
                asteaptaEnter();
        }
    }
}