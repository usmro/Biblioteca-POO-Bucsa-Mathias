#include "src/db/Database.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

int main() {
    Database db("../date/biblioteca.db");
    db.conecteaza();

    ifstream fisier("../date/carti.txt");
    if (!fisier.is_open()) {
        cout << "Nu pot deschide carti.txt!" << endl;
        return 1;
    }

    int count = 0;
    string linie;
    while (getline(fisier, linie)) {
        if (linie.empty()) continue;

        stringstream ss(linie);
        string tip, titlu, autor, isbn, extra1, extra2;

        getline(ss, tip, '|');
        getline(ss, titlu, '|');
        getline(ss, autor, '|');
        getline(ss, isbn, '|');
        getline(ss, extra1, '|');
        getline(ss, extra2, '|');

        db.adaugaCarte(titlu, autor, isbn, tip, extra1, extra2);
        count++;
    }

    cout << count << " carti importate!" << endl;

    db.adaugaAngajat("Ana Popescu", "director", "director123",
                  "director", 8000, "Conducere");
    db.adaugaAngajat("Mihai Ionescu", "bibliotecar", "biblio123",
                  "bibliotecar", 4000, "Gestiune Carti");
    return 0;
}
