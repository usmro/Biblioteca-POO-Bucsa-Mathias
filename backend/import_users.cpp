#include "src/db/Database.h"
#include <iostream>
using namespace std;

int main() {
    Database db("../date/biblioteca.db");
    db.conecteaza();

    db.adaugaUtilizatorCuRol("Ana Popescu", "director",
                              "director123", "director");
    db.adaugaUtilizatorCuRol("Mihai Ionescu", "bibliotecar",
                              "biblio123", "bibliotecar");
    db.adaugaUtilizatorCuRol("Ion Popescu", "ion123",
                              "parola123", "utilizator");
    db.adaugaAngajat("Ana Popescu", "director", "director123", "director", 8000, "Conducere");
    db.adaugaAngajat("Mihai Ionescu", "bibliotecar", "biblio123", "bibliotecar", 4000, "Gestiune Carti");
    
    bool ok1 = db.adaugaAngajat("Ana Popescu", "director", "director123", "director", 8000, "Conducere");
    bool ok2 = db.adaugaAngajat("Mihai Ionescu", "bibliotecar", "biblio123", "bibliotecar", 4000, "Gestiune Carti");
    cout << "Director adaugat: " << (ok1 ? "DA" : "NU") << endl;
    cout << "Bibliotecar adaugat: " << (ok2 ? "DA" : "NU") << endl;

    cout << "Conturi create cu succes!" << endl;
    return 0;
}
