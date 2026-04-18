#ifndef EXCEPTII_H
#define EXCEPTII_H

#include <exception>
#include <string>
using namespace std;

// Exceptie de baza pentru biblioteca
class BibliotecaException : public exception {
protected:
    string mesaj;
public:
    BibliotecaException(const string& mesaj) : mesaj(mesaj) {}
    const char* what() const noexcept override {
        return mesaj.c_str();
    }
};

// Carte indisponibila (deja imprumutata)
class CarteIndisponibilaException : public BibliotecaException {
public:
    CarteIndisponibilaException(const string& isbn)
        : BibliotecaException("Cartea cu ISBN " + isbn +
                              " nu este disponibila!") {}
};

// Carte negasita in catalog
class CarteNegasitaException : public BibliotecaException {
public:
    CarteNegasitaException(const string& isbn)
        : BibliotecaException("Cartea cu ISBN " + isbn +
                              " nu exista in catalog!") {}
};

// Utilizator negasit
class UtilizatorNegasitException : public BibliotecaException {
public:
    UtilizatorNegasitException(int id)
        : BibliotecaException("Utilizatorul cu ID " +
                              to_string(id) + " nu exista!") {}
};

// Username deja existent
class UsernameExistentException : public BibliotecaException {
public:
    UsernameExistentException(const string& username)
        : BibliotecaException("Username-ul '" + username +
                              "' este deja folosit!") {}
};

// Parola incorecta
class ParolaIncorectaException : public BibliotecaException {
public:
    ParolaIncorectaException()
        : BibliotecaException("Parola incorecta!") {}
};

// Imprumut inexistent la returnare
class ImprumutInexistentException : public BibliotecaException {
public:
    ImprumutInexistentException(const string& isbn)
        : BibliotecaException("Nu exista un imprumut activ pentru ISBN " +
                              isbn + "!") {}
};

#endif