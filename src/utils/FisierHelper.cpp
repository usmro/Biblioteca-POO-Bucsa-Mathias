#include "FisierHelper.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

bool FisierHelper::salveazaUtilizatori(const vector<Utilizator>& utilizatori,
                                        const string& caleFisier) {
    ofstream fisier(caleFisier);
    if (!fisier.is_open()) {
        cout << "[EROARE] Nu pot deschide fisierul: " << caleFisier << endl;
        return false;
    }

    for (const auto& u : utilizatori) {
        fisier << u.getId() << "|"
               << u.getNume() << "|"
               << u.getUsername() << "|"
               << u.getParolaCriptata() << "\n";
    }

    fisier.close();
    cout << "[LOG] Utilizatori salvati in: " << caleFisier << endl;
    return true;
}

vector<Utilizator> FisierHelper::incarcaUtilizatori(const string& caleFisier) {
    vector<Utilizator> utilizatori;
    ifstream fisier(caleFisier);

    if (!fisier.is_open()) {
        cout << "[LOG] Fisierul " << caleFisier << " nu exista inca." << endl;
        return utilizatori;
    }

    string linie;
    while (getline(fisier, linie)) {
        if (linie.empty()) continue;

        stringstream ss(linie);
        string idStr, nume, username, parolaCriptata;

        getline(ss, idStr, '|');
        getline(ss, nume, '|');
        getline(ss, username, '|');
        getline(ss, parolaCriptata, '|');

        int id = stoi(idStr);
        utilizatori.push_back(Utilizator(nume, id, username, "LOADED"));
    }

    fisier.close();
    cout << "[LOG] " << utilizatori.size()
         << " utilizatori incarcati din fisier." << endl;
    return utilizatori;
}

bool FisierHelper::salveazaCarti(const vector<Carte*>& carti,
                                  const string& caleFisier) {
    ofstream fisier(caleFisier);
    if (!fisier.is_open()) return false;

    for (const auto& c : carti) {
        string tip = c->getTip();
        fisier << tip << "|" << c->getTitlu() << "|"
               << c->getAutor() << "|" << c->getIsbn() << "|";

        if (auto* x = dynamic_cast<CarteFictiune*>(c))
            fisier << x->getGen() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<CarteTehnica*>(c))
            fisier << x->getDomeniu() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<Revista*>(c))
            fisier << x->getNumar() << "|" << x->getLuna() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<Manga*>(c))
            fisier << x->getVolum() << "|" << x->getMangaka() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<BenziDesonate*>(c))
            fisier << x->getArtist() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<RomanGrafic*>(c))
            fisier << x->getArtist() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<Biografie*>(c))
            fisier << x->getSubiect() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<Stiinta*>(c))
            fisier << x->getRamura() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<Istorie*>(c))
            fisier << x->getPerioada() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<Filozofie*>(c))
            fisier << x->getCurent() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<Manual*>(c))
            fisier << x->getMaterie() << "|" << x->getNivel() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<Enciclopedie*>(c))
            fisier << x->getDomeniu() << "|" << x->getStare();
        else if (auto* x = dynamic_cast<CarteDigitala*>(c))
            fisier << x->getFormat() << "|" << x->getLinkDownload()
                   << "|" << x->getDimensiuneMB();
        else if (auto* x = dynamic_cast<Audiobook*>(c))
            fisier << x->getNarator() << "|" << x->getDurataMinute();

        fisier << "\n";
    }

    fisier.close();
    cout << "[LOG] " << carti.size() << " carti salvate." << endl;
    return true;
}

vector<Carte*> FisierHelper::incarcaCarti(const string& caleFisier) {
    vector<Carte*> carti;
    ifstream fisier(caleFisier);
    if (!fisier.is_open()) return carti;

    string linie;
    while (getline(fisier, linie)) {
        if (linie.empty()) continue;
        stringstream ss(linie);
        string tip, titlu, autor, isbn;

        getline(ss, tip, '|');
        getline(ss, titlu, '|');
        getline(ss, autor, '|');
        getline(ss, isbn, '|');

        Carte* c = nullptr;

        if (tip == "FICTIUNE") {
            string gen, stare;
            getline(ss, gen, '|'); getline(ss, stare, '|');
            auto* x = new CarteFictiune(titlu, autor, isbn, gen);
            x->setStare(stare); c = x;
        } else if (tip == "TEHNICA") {
            string domeniu, stare;
            getline(ss, domeniu, '|'); getline(ss, stare, '|');
            auto* x = new CarteTehnica(titlu, autor, isbn, domeniu);
            x->setStare(stare); c = x;
        } else if (tip == "REVISTA") {
            string nrStr, luna, stare;
            getline(ss, nrStr, '|'); getline(ss, luna, '|');
            getline(ss, stare, '|');
            auto* x = new Revista(titlu, autor, isbn, stoi(nrStr), luna);
            x->setStare(stare); c = x;
        } else if (tip == "MANGA") {
            string volStr, mangaka, stare;
            getline(ss, volStr, '|'); getline(ss, mangaka, '|');
            getline(ss, stare, '|');
            auto* x = new Manga(titlu, autor, isbn, stoi(volStr), mangaka);
            x->setStare(stare); c = x;
        } else if (tip == "BENZI_DESENATE") {
            string artist, stare;
            getline(ss, artist, '|'); getline(ss, stare, '|');
            auto* x = new BenziDesonate(titlu, autor, isbn, artist);
            x->setStare(stare); c = x;
        } else if (tip == "ROMAN_GRAFIC") {
            string artist, stare;
            getline(ss, artist, '|'); getline(ss, stare, '|');
            auto* x = new RomanGrafic(titlu, autor, isbn, artist);
            x->setStare(stare); c = x;
        } else if (tip == "BIOGRAFIE") {
            string subiect, stare;
            getline(ss, subiect, '|'); getline(ss, stare, '|');
            auto* x = new Biografie(titlu, autor, isbn, subiect);
            x->setStare(stare); c = x;
        } else if (tip == "STIINTA") {
            string ramura, stare;
            getline(ss, ramura, '|'); getline(ss, stare, '|');
            auto* x = new Stiinta(titlu, autor, isbn, ramura);
            x->setStare(stare); c = x;
        } else if (tip == "ISTORIE") {
            string perioada, stare;
            getline(ss, perioada, '|'); getline(ss, stare, '|');
            auto* x = new Istorie(titlu, autor, isbn, perioada);
            x->setStare(stare); c = x;
        } else if (tip == "FILOZOFIE") {
            string curent, stare;
            getline(ss, curent, '|'); getline(ss, stare, '|');
            auto* x = new Filozofie(titlu, autor, isbn, curent);
            x->setStare(stare); c = x;
        } else if (tip == "MANUAL") {
            string materie, nivel, stare;
            getline(ss, materie, '|'); getline(ss, nivel, '|');
            getline(ss, stare, '|');
            auto* x = new Manual(titlu, autor, isbn, materie, nivel);
            x->setStare(stare); c = x;
        } else if (tip == "ENCICLOPEDIE") {
            string domeniu, stare;
            getline(ss, domeniu, '|'); getline(ss, stare, '|');
            auto* x = new Enciclopedie(titlu, autor, isbn, domeniu);
            x->setStare(stare); c = x;
        } else if (tip == "DIGITAL") {
            string format, link, dimStr;
            getline(ss, format, '|'); getline(ss, link, '|');
            getline(ss, dimStr, '|');
            c = new CarteDigitala(titlu, autor, isbn, format, link, stod(dimStr));
        } else if (tip == "AUDIOBOOK") {
            string narator, durStr;
            getline(ss, narator, '|'); getline(ss, durStr, '|');
            c = new Audiobook(titlu, autor, isbn, narator, stoi(durStr));
        }

        if (c) carti.push_back(c);
    }

    fisier.close();
    cout << "[LOG] " << carti.size() << " carti incarcate." << endl;
    return carti;
}

bool FisierHelper::salveazaAngajati(Director* director,
                                     Bibliotecar* bibliotecar,
                                     const string& caleFisier) {
    ofstream fisier(caleFisier);
    if (!fisier.is_open()) {
        cout << "[EROARE] Nu pot deschide fisierul: " << caleFisier << endl;
        return false;
    }

    if (director) {
        fisier << "DIRECTOR|" << director->getNume() << "|"
               << director->getId() << "|"
               << director->getUsername() << "|"
               << director->getParolaCriptata() << "|"
               << director->getSalariu() << "|"
               << director->getDepartament() << "\n";
    }

    if (bibliotecar) {
        fisier << "BIBLIOTECAR|" << bibliotecar->getNume() << "|"
               << bibliotecar->getId() << "|"
               << bibliotecar->getUsername() << "|"
               << bibliotecar->getParolaCriptata() << "|"
               << bibliotecar->getSalariu() << "|"
               << bibliotecar->getDepartament() << "\n";
    }

    fisier.close();
    cout << "[LOG] Angajati salvati in: " << caleFisier << endl;
    return true;
}

pair<Director*, Bibliotecar*> FisierHelper::incarcaAngajati(
        const string& caleFisier) {
    Director* director = nullptr;
    Bibliotecar* bibliotecar = nullptr;

    ifstream fisier(caleFisier);
    if (!fisier.is_open()) {
        cout << "[LOG] Fisierul " << caleFisier << " nu exista inca." << endl;
        return {nullptr, nullptr};
    }

    string linie;
    while (getline(fisier, linie)) {
        if (linie.empty()) continue;

        stringstream ss(linie);
        string rol, nume, idStr, username, parolaCriptata, salariuStr, dept;

        getline(ss, rol, '|');
        getline(ss, nume, '|');
        getline(ss, idStr, '|');
        getline(ss, username, '|');
        getline(ss, parolaCriptata, '|');
        getline(ss, salariuStr, '|');
        getline(ss, dept, '|');

        int id = stoi(idStr);
        double salariu = stod(salariuStr);

        if (rol == "DIRECTOR") {
            // Cream cu parola placeholder si setam direct hash-ul
            director = new Director(nume, id, username, "PLACEHOLDER", salariu);
            director->setParolaCriptata(parolaCriptata);
        } else if (rol == "BIBLIOTECAR") {
            bibliotecar = new Bibliotecar(nume, id, username, "PLACEHOLDER", salariu);
            bibliotecar->setParolaCriptata(parolaCriptata);
        }
    }

    fisier.close();
    return {director, bibliotecar};
}