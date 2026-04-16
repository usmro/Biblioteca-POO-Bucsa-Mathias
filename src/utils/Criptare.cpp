#include "Criptare.h"

string Criptare::cripteaza(const string& text) {
    string rezultat = text;
    for (char& c : rezultat) {
        // Cripteaza literele mari
        if (c >= 'A' && c <= 'Z') {
            c = (c - 'A' + CHEIE) % 26 + 'A';
        }
        // Cripteaza literele mici
        else if (c >= 'a' && c <= 'z') {
            c = (c - 'a' + CHEIE) % 26 + 'a';
        }
        // Cifrele le cripteaza separat
        else if (c >= '0' && c <= '9') {
            c = (c - '0' + CHEIE) % 10 + '0';
        }
        // Restul caracterelor raman neschimbate
    }
    return rezultat;
}

string Criptare::decripteaza(const string& text) {
    string rezultat = text;
    for (char& c : rezultat) {
        if (c >= 'A' && c <= 'Z') {
            c = (c - 'A' - CHEIE + 26) % 26 + 'A';
        }
        else if (c >= 'a' && c <= 'z') {
            c = (c - 'a' - CHEIE + 26) % 26 + 'a';
        }
        else if (c >= '0' && c <= '9') {
            c = (c - '0' - CHEIE + 10) % 10 + '0';
        }
    }
    return rezultat;
}

bool Criptare::verificaParola(const string& parolaIntrodusa,
                               const string& parolaCriptata) {
    return cripteaza(parolaIntrodusa) == parolaCriptata;
}