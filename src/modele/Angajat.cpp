#include "../modele/Angajat.h"
#include "../utils/Criptare.h"
#include <iostream>

Angajat::Angajat(string nume, int id, string username, string parola,
                 double salariu, string departament, RolAngajat rol)
    : nume(nume), id(id), username(username),
      salariu(salariu), departament(departament), rol(rol) {
    parolaCriptata = Criptare::sha256(parola);
}

string Angajat::getNume() const { return nume; }
int Angajat::getId() const { return id; }
string Angajat::getUsername() const { return username; }
string Angajat::getParolaCriptata() const { return parolaCriptata; }
double Angajat::getSalariu() const { return salariu; }
string Angajat::getDepartament() const { return departament; }
RolAngajat Angajat::getRol() const { return rol; }

string Angajat::getRolString() const {
    switch (rol) {
        case RolAngajat::BIBLIOTECAR: return "Bibliotecar";
        case RolAngajat::DIRECTOR:    return "Director";
        default:                       return "Necunoscut";
    }
}

void Angajat::setSalariu(double s) { salariu = s; }

bool Angajat::verificaParola(const string& parola) const {
    return Criptare::verificaParola(parola, parolaCriptata);
}

void Angajat::afiseazaInfo() const {
    cout << "[" << getRolString() << "] "
         << nume << " | Departament: " << departament
         << " | Salariu: " << salariu << " lei" << endl;
}

void Angajat::setParolaCriptata(const string& hash) {
    parolaCriptata = hash;
}

void Angajat::setNume(const string& numeNou) {
    nume = numeNou;
}