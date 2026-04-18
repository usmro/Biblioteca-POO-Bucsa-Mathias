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
    if (!fisier.is_open()) {
        cout << "[EROARE] Nu pot deschide fisierul: " << caleFisier << endl;
        return false;
    }

    for (const auto& carte : carti) {
        if (auto* cf = dynamic_cast<CarteFictiune*>(carte)) {
            fisier << "FICTIUNE|" << cf->getTitlu() << "|"
                   << cf->getAutor() << "|" << cf->getIsbn() << "|"
                   << cf->getGen() << "\n";
        } else if (auto* ct = dynamic_cast<CarteTehnica*>(carte)) {
            fisier << "TEHNICA|" << ct->getTitlu() << "|"
                   << ct->getAutor() << "|" << ct->getIsbn() << "|"
                   << ct->getDomeniu() << "\n";
        } else if (auto* cd = dynamic_cast<CarteDigitala*>(carte)) {
            fisier << "DIGITAL|" << cd->getTitlu() << "|"
                   << cd->getAutor() << "|" << cd->getIsbn() << "|"
                   << cd->getFormat() << "|" << cd->getLinkDownload() << "|"
                   << cd->getDimensiuneMB() << "\n";
        } else if (auto* ab = dynamic_cast<Audiobook*>(carte)) {
            fisier << "AUDIOBOOK|" << ab->getTitlu() << "|"
                   << ab->getAutor() << "|" << ab->getIsbn() << "|"
                   << ab->getNarator() << "|" << ab->getDurataMinute() << "\n";
        }
    }

    fisier.close();
    cout << "[LOG] " << carti.size() << " carti salvate in: "
         << caleFisier << endl;
    return true;
}

vector<Carte*> FisierHelper::incarcaCarti(const string& caleFisier) {
    vector<Carte*> carti;
    ifstream fisier(caleFisier);

    if (!fisier.is_open()) {
        cout << "[LOG] Fisierul " << caleFisier << " nu exista inca." << endl;
        return carti;
    }

    string linie;
    while (getline(fisier, linie)) {
        if (linie.empty()) continue;

        stringstream ss(linie);
        string tip, titlu, autor, isbn;

        getline(ss, tip, '|');
        getline(ss, titlu, '|');
        getline(ss, autor, '|');
        getline(ss, isbn, '|');

        Carte* carte = nullptr;

        if (tip == "FICTIUNE") {
            string gen;
            getline(ss, gen, '|');
            carte = new CarteFictiune(titlu, autor, isbn, gen);
        } else if (tip == "TEHNICA") {
            string domeniu;
            getline(ss, domeniu, '|');
            carte = new CarteTehnica(titlu, autor, isbn, domeniu);
        } else if (tip == "DIGITAL") {
            string format, link, dimStr;
            getline(ss, format, '|');
            getline(ss, link, '|');
            getline(ss, dimStr, '|');
            double dim = stod(dimStr);
            carte = new CarteDigitala(titlu, autor, isbn, format, link, dim);
        } else if (tip == "AUDIOBOOK") {
            string narator, durataStr;
            getline(ss, narator, '|');
            getline(ss, durataStr, '|');
            int durata = stoi(durataStr);
            carte = new Audiobook(titlu, autor, isbn, narator, durata);
        }

        if (carte) carti.push_back(carte);
    }

    fisier.close();
    cout << "[LOG] " << carti.size()
         << " carti incarcate din fisier." << endl;
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