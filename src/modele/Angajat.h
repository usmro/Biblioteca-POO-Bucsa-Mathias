#ifndef ANGAJAT_H
#define ANGAJAT_H

#include <string>
using namespace std;

// Tipuri de roluri disponibile
enum class RolAngajat {
    BIBLIOTECAR,
    DIRECTOR
};

class Angajat {
private:
    string nume;
    int id;
    string username;
    string parolaCriptata;
    double salariu;
    string departament;
    RolAngajat rol;

public:
    Angajat(string nume, int id, string username, string parola,
            double salariu, string departament, RolAngajat rol);

    string getNume() const;
    int getId() const;
    string getUsername() const;
    string getParolaCriptata() const;
    double getSalariu() const;
    string getDepartament() const;
    RolAngajat getRol() const;
    string getRolString() const;
    void setNume(const string& numeNou);

    void setSalariu(double salariu);

    void setParolaCriptata(const string& hash);

    bool verificaParola(const string& parola) const;
    virtual void afiseazaInfo() const;
    virtual ~Angajat() {}
};

#endif